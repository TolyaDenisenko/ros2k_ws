// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#include "quasi_r2_perception/perception_node.hpp"
#include <pcl_conversions/pcl_conversions.h>
#include <pcl_ros/transforms.hpp>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/search/octree.h>
#include <geometric_shapes/body_operations.h>
#include <geometric_shapes/shape_operations.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <random>

using namespace quasi;
using namespace std::string_literals;

void PerceptionNode::execute_find_place_pose_goal(const std::shared_ptr<rclcpp_action::ServerGoalHandle<quasi_r2_msgs::action::FindPlacePose>> goal_handle) {
  auto goal = goal_handle->get_goal();
  auto result = std::make_shared<quasi_r2_msgs::action::FindPlacePose::Result>();
  auto feedback = std::make_shared<quasi_r2_msgs::action::FindPlacePose::Feedback>();

  try {
    rclcpp::CallbackGroup::SharedPtr callback_group = create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive, false);
    rclcpp::executors::SingleThreadedExecutor callback_group_executor;
    callback_group_executor.add_callback_group(callback_group, get_node_base_interface());

    // Subscribe to point clouds using the executor associated with this thread
    auto subscription_options = rclcpp_lifecycle::create_default_subscription_options<std::allocator<void>>();
    subscription_options.callback_group = callback_group;

    std::promise<std::shared_ptr<std::vector<geometry_msgs::msg::PoseStamped>>> find_place_pose_result_promise;
    auto find_place_pose_result_future = find_place_pose_result_promise.get_future();
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr pcl_subscription =
        create_subscription<sensor_msgs::msg::PointCloud2>(get_parameter("pointcloud_topic").as_string(), rclcpp::SensorDataQoS(),
                                                           [&, this] (sensor_msgs::msg::PointCloud2::SharedPtr msg) {
                                                             if(!pcl_subscription)
                                                               return;
                                                             msg->is_dense = false;  // cannot rely on dense flag because ignition gazebo incorrectly sets it to true
                                                             auto object_place_poses = find_place_pose_on_pointcloud_message(std::move(msg), goal);
                                                             if(object_place_poses) {
                                                               pcl_subscription.reset();
                                                               find_place_pose_result_promise.set_value(object_place_poses);
                                                             }
                                                           }, subscription_options);

    // TODO: add cancellation
    auto rc = callback_group_executor.spin_until_future_complete(find_place_pose_result_future, std::chrono::milliseconds(get_parameter("cloud_segmentation_timeout").as_int()));
    if( rc == rclcpp::FutureReturnCode::TIMEOUT)
      throw std::runtime_error("cloud segmentation timed out");
    else if( rc != rclcpp::FutureReturnCode::SUCCESS)
      throw std::runtime_error("cloud segmentation failed");

    result->object_poses = *find_place_pose_result_future.get();

    if(goal_handle->is_canceling()) {
      RCLCPP_INFO(get_logger(), "Find Place Pose goal execution canceled");
      goal_handle->canceled(result);
    }
    else
      goal_handle->succeed(result);
  }
  catch(const std::exception& ex) {
    RCLCPP_FATAL(get_logger(), "%s", ex.what());
    goal_handle->abort(result);
  }
  catch(...) {
    RCLCPP_FATAL(get_logger(), "generic exception caught while processing Find Place Pose goal");
    goal_handle->abort(result);
  }

}


std::shared_ptr<std::vector<geometry_msgs::msg::PoseStamped>> PerceptionNode::find_place_pose_on_pointcloud_message(
    sensor_msgs::msg::PointCloud2::SharedPtr msg,
    const quasi_r2_msgs::action::FindPlacePose::Goal::ConstSharedPtr& goal) {
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud = pcl::make_shared<pcl::PointCloud<pcl::PointXYZRGB>>();
  pcl::fromROSMsg(*msg, *cloud);

  auto frame_id = get_parameter("frame_id").as_string();

  // Transform point cloud to our target frame
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_transformed(new pcl::PointCloud<pcl::PointXYZRGB>);
  try {
    auto transform = tf_buffer_->lookupTransform(frame_id, cloud->header.frame_id, msg->header.stamp, rclcpp::Duration::from_seconds(0.5));
    pcl_ros::transformPointCloud(*cloud, *cloud_transformed, transform);
    cloud_transformed->header.frame_id = frame_id;
  }
  catch (tf2::TransformException& ex)
  {
    RCLCPP_ERROR_STREAM(get_logger(), "Error getting transform for received cloud: " << ex.what());
    return nullptr;
  }
  // Crop workspace, if needed
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_cropped = filter_cloud_by_workspace(cloud_transformed, goal->workspace);

  size_t plane_cloud_size_threshold = get_parameter("plane_points_threshold").as_int();

  // Estimate point normals
  pcl::PointCloud<pcl::PointNormal>::Ptr cloud_normals(new pcl::PointCloud<pcl::PointNormal>);
  pcl::NormalEstimationOMP<pcl::PointXYZRGB, pcl::PointNormal> ne;
  ne.setInputCloud(cloud_cropped);
  ne.setKSearch(20);
  //ne.setRadiusSearch(0.05);
  auto camera_transform = tf_buffer_->lookupTransform(frame_id, "realsense_color_frame", tf2::TimePointZero);
  ne.setViewPoint((float)camera_transform.transform.translation.x, (float)camera_transform.transform.translation.y, (float)camera_transform.transform.translation.z);
  ne.compute(*cloud_normals);

  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_plane;
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr non_horizontal_planes = pcl::make_shared<pcl::PointCloud<pcl::PointXYZRGB>>();
  pcl::PointCloud<pcl::PointNormal>::Ptr non_horizontal_planes_normals = pcl::make_shared<pcl::PointCloud<pcl::PointNormal>>();
  pcl::ModelCoefficients::Ptr plane_coefs;

  auto plane_distance_threshold = get_parameter("plane_distance_threshold").as_double();
  auto plane_angle_threshold = get_parameter("plane_angle_threshold").as_double();
  auto area_height_threshold = get_parameter("area_height_threshold").as_double();

  std::vector<double> horizontal_plane_heights;
  const auto& area_height = goal->area_height;

  while(cloud_cropped->size() > 200 && !cloud_plane) {
    // Segment the largest planar components from the remaining cloud
    pcl::PointIndices::Ptr plane_indices = std::make_shared<pcl::PointIndices>();
    pcl::SACSegmentation<pcl::PointXYZRGB> segment;
    segment.setOptimizeCoefficients(true);
    segment.setMethodType(pcl::SAC_RANSAC);
    segment.setModelType(pcl::SACMODEL_PLANE); //pcl::SACMODEL_NORMAL_PLANE);
    segment.setMaxIterations(100);
    segment.setDistanceThreshold(plane_distance_threshold);
    //segment.setNormalDistanceWeight(0.2);
    segment.setInputCloud(cloud_cropped);
    //segment.setInputNormals(cloud_normals_filtered);
    auto plane_coefs_current =  std::make_shared<pcl::ModelCoefficients>();
    segment.segment(*plane_indices, *plane_coefs_current);

    RCLCPP_DEBUG(get_logger(), "Found plane with %zu points. Threshold: %jd. Plane height %f", plane_indices->indices.size(), plane_cloud_size_threshold, -plane_coefs_current->values[3]/plane_coefs_current->values[2]);
    if(plane_indices->indices.size() < (size_t)plane_cloud_size_threshold)
      break;

    // Check plane is mostly horizontal
    Eigen::Vector3f plane_normal(plane_coefs_current->values[0], plane_coefs_current->values[1], plane_coefs_current->values[2]);
    float angle = acos(Eigen::Vector3f::UnitZ().dot(plane_normal));
    if (angle >= plane_angle_threshold) {
      RCLCPP_DEBUG(get_logger(), "** Found plane is non-horizontal: %f, threshold: %f", angle, plane_angle_threshold);

      pcl::PointCloud<pcl::PointXYZRGB>::Ptr non_horizontal_plane = pcl::make_shared<pcl::PointCloud<pcl::PointXYZRGB>>();
      pcl::PointCloud<pcl::PointNormal>::Ptr non_horizontal_plane_normals = pcl::make_shared<pcl::PointCloud<pcl::PointNormal>>();
      pcl::ExtractIndices<pcl::PointXYZRGB> extract;
      extract.setInputCloud(cloud_cropped);
      extract.setIndices(plane_indices);
      extract.setNegative(false);
      extract.filter(*non_horizontal_plane);

      pcl::ExtractIndices<pcl::PointNormal> extract_normals;
      extract_normals.setInputCloud(cloud_normals);
      extract_normals.setIndices(plane_indices);
      extract_normals.setNegative(false);
      extract_normals.filter(*non_horizontal_plane_normals);

      // we will need to re-add these planes back to the cloud_cropped later after extraction of horizontal plane
      *non_horizontal_planes += *non_horizontal_plane;
      *non_horizontal_planes_normals += *non_horizontal_plane_normals;
    }
    else {
      double plane_height = -plane_coefs_current->values[3] / plane_coefs_current->values[2];
      horizontal_plane_heights.push_back(plane_height);
      // horizontal planes
      if (area_height < 0 || std::abs(area_height - plane_height) <= area_height_threshold) {
        RCLCPP_DEBUG(get_logger(), "Found horizontal plane for placement. Plain height: %f", plane_height);
        cloud_plane = std::make_shared<pcl::PointCloud<pcl::PointXYZRGB>>();
        pcl::ExtractIndices<pcl::PointXYZRGB> extract;
        extract.setInputCloud(cloud_cropped);
        extract.setIndices(plane_indices);
        extract.setNegative(false);
        extract.filter(*cloud_plane);
        plane_coefs = std::make_shared<pcl::ModelCoefficients>();
        *plane_coefs = *plane_coefs_current;
      }
      else
        RCLCPP_DEBUG(get_logger(), "** Found plane is at wrong height: %f, needed height: %f with %f tolerance", plane_height, area_height, area_height_threshold);
    }

    // Remove found plane from cropped and normals clouds
    pcl::ExtractIndices<pcl::PointXYZRGB> extract;
    extract.setInputCloud(cloud_cropped);
    extract.setIndices(plane_indices);
    extract.setNegative(true);
    extract.filter(*cloud_cropped);

    pcl::ExtractIndices<pcl::PointNormal> extract_normals;
    extract_normals.setInputCloud(cloud_normals);
    extract_normals.setIndices(plane_indices);
    extract_normals.setNegative(true);
    extract_normals.filter(*cloud_normals);
  }

  auto place_poses = std::make_shared<std::vector<geometry_msgs::msg::PoseStamped>>();

  if(!cloud_plane) {
    RCLCPP_ERROR(get_logger(), "Unable to find horizontal support plane with at least %zu points", plane_cloud_size_threshold);
    return place_poses;
  }

  // add back non-horizontal planes
  *cloud_cropped += *non_horizontal_planes;
  *cloud_normals += *non_horizontal_planes_normals;

  // Find horizontal plane above if any
  double next_upper_plane = 100;
  if (area_height >= 0) {
    for (double h : horizontal_plane_heights) {
      if (h > area_height + area_height_threshold)
        next_upper_plane = std::min(h, next_upper_plane);
    }
  }

  // Keep only points on top of the plane
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_top(new pcl::PointCloud<pcl::PointXYZRGB>());
  Eigen::Vector4f m(plane_coefs->values[0], plane_coefs->values[1], plane_coefs->values[2], plane_coefs->values[3]);
  for(const auto& pt : *cloud_cropped) {
    auto pp = pt.getVector4fMap();
    double distance_to_plane = pp.dot(m);
    if(distance_to_plane > 0.0 && pt.z < next_upper_plane)
      cloud_top->push_back(pt);
  }

  // Project everything on top onto the plane
  Eigen::Matrix3f inv_plane_rotation;
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr flat_projected = project_onto_plane(cloud_top, plane_coefs, inv_plane_rotation);

  pcl::search::Octree<pcl::PointXYZRGB> plane_search(0.005);
  plane_search.setInputCloud(cloud_plane);

  pcl::search::Octree<pcl::PointXYZRGB> space_search(0.005);
  space_search.setInputCloud(flat_projected);

  double gripper_outer_radius = get_parameter("gripper_outer_radius").as_double();
  size_t max_place_poses = goal->max_poses > 0 ? (size_t)goal->max_poses : (size_t)get_parameter("max_place_poses").as_int();
  size_t num_samples = get_parameter("num_samples").as_int();
  double z_padding = get_parameter("z_padding").as_double();

  if(cloud_viewer_) {
    cloud_viewer_->runOnVisualizationThreadOnce([=] (pcl::visualization::PCLVisualizer& pcl_visualizer) {
      pcl_visualizer.removeAllShapes();
      pcl_visualizer.removeAllPointClouds();

      pcl_visualizer.addPointCloud(flat_projected, "flat_projected");
    });
  }

  geometry_msgs::msg::Pose object_pose;
  const auto& original_orientation = goal->original_orientation;
  const auto& object = goal->attached_object;
  if(!original_orientation.header.frame_id.empty() && original_orientation.quaternion.w != 0.0) {
    RCLCPP_DEBUG(get_logger(), "Will be preserving original object orientation [%f,%f,%f,%f] in frame %s instead of current orientation [%f,%f,%f,%f] in frame %s",
                 original_orientation.quaternion.x, original_orientation.quaternion.y, original_orientation.quaternion.z, original_orientation.quaternion.w, original_orientation.header.frame_id.c_str(),
                 object.object.pose.orientation.x, object.object.pose.orientation.y, object.object.pose.orientation.z, object.object.pose.orientation.w, object.object.header.frame_id.c_str());
    tf2::doTransform(original_orientation.quaternion, object_pose.orientation, tf_buffer_->lookupTransform(frame_id, original_orientation.header.frame_id, tf2::TimePointZero));
  }
  else  // use current pose relative to hand_base link
    tf2::doTransform(object.object.pose.orientation, object_pose.orientation, tf_buffer_->lookupTransform(frame_id, object.object.header.frame_id, tf2::TimePointZero));


  std::shared_ptr<bodies::Body> body;
  if(!object.object.primitives.empty())
    body.reset(bodies::constructBodyFromMsg(object.object.primitives[0], object_pose));
  else if(!object.object.meshes.empty())
    body.reset(bodies::constructBodyFromMsg(object.object.meshes[0], object_pose));
  else if(!object.object.planes.empty())
    body.reset(bodies::constructBodyFromMsg(object.object.planes[0], object_pose));
  else
    throw std::runtime_error("shape of attached object is not defined");  // this should never happen if attached object is taken from MoveIt

  bodies::AABB aabb;
  body->computeBoundingBox(aabb);
  Eigen::Vector3d shape_extents = aabb.sizes();
  Eigen::Quaterniond shape_rotation(body->getPose().rotation());
  RCLCPP_DEBUG(get_logger(), "object pose and extents: [%f,%f,%f,%f] [%f,%f,%f]",
               shape_rotation.x(), shape_rotation.y(), shape_rotation.z(), shape_rotation.w(),
               shape_extents.x(), shape_extents.y(), shape_extents.z());
  auto ex = shape_extents.x()/2.0;
  auto ey = shape_extents.y()/2.0;
  double r = sqrt(ex*ex + ey*ey);

  std::uniform_int_distribution<size_t> d(0, cloud_plane->size()-1);
  std::random_device rnd;
  for(size_t i = 0; i < num_samples && place_poses->size() < max_place_poses; ++i) {
    // Take random point from the plane cloud
    auto pt = cloud_plane->at(d(rnd));

    // Make sure there are no occupied points on top of the plane at given point within gripper_outer_radius
    std::vector<int> k_indices;
    std::vector<float> k_sqr_dist;
    int nn = space_search.radiusSearch(pt, gripper_outer_radius, k_indices, k_sqr_dist, 1);
    if(nn > 0) {
      RCLCPP_DEBUG(get_logger(), "Skipping pose x: %f, y: %f, z: %f because of other points within %f radius", pt.x, pt.y, pt.z, gripper_outer_radius);
      continue;
    }

    Eigen::Vector3f pt3f = inv_plane_rotation.cast<float>() * pt.getVector3fMap();

    // Make sure our object will be fully supported by a plane
    bool supported = true;
    for(int step = 0; step < 8; ++step) {
      double θ = M_PI*2.0/8.0*step;
      pcl::PointXYZ pt2(pt.x + r * cos(θ), pt.y + r * sin(θ), pt.z);

      k_indices.clear();
      k_sqr_dist.clear();
      nn = plane_search.radiusSearchT(pt2, 0.005, k_indices, k_sqr_dist, 1);
      if(nn == 0) {
        RCLCPP_DEBUG(get_logger(), "Skipping pose x: %f, y: %f, z: %f because there are no plane points under x: %f, y: %f, z: %f (θ: %f, radius: %f)", pt.x, pt.y, pt.z, pt2.x, pt2.y, pt2.z, θ, r);
        supported = false;
        break;
      }
    }
    if(!supported)
      continue;

    pt3f.z() += (float)shape_extents.z() / 2.0f;

    if(cloud_viewer_) {
      cloud_viewer_->runOnVisualizationThreadOnce([=] (pcl::visualization::PCLVisualizer& pcl_visualizer) {
        std::string box_name = "cube_"s + std::to_string(i);
        pcl_visualizer.addCube(pt3f, Eigen::Quaternionf::Identity(), shape_extents.x(), shape_extents.y(), shape_extents.z(), box_name);
      });
    }

    pt3f.z() += static_cast<float>(z_padding);
    RCLCPP_DEBUG(get_logger(), "object placing pose x: %f, y: %f, z: %f adjusted for plane and padded for object height x: %f, y: %f, z: %f", pt.x, pt.y, pt.z, pt3f.x(), pt3f.y(), pt3f.z());

    geometry_msgs::msg::PoseStamped& place_pose = place_poses->emplace_back();
    place_pose.header.stamp = msg->header.stamp;
    place_pose.header.frame_id = frame_id;
    place_pose.pose.orientation = object_pose.orientation;
    place_pose.pose.position.x = pt3f.x();
    place_pose.pose.position.y = pt3f.y();
    place_pose.pose.position.z = pt3f.z();
  }

  RCLCPP_INFO(get_logger(), "Found %zu object placing poses", place_poses->size());
  return place_poses;
}

// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#include "quasi_r2_perception/perception_node.hpp"
#include "quasi_r2_perception/pcl_conversions.hpp"

#include <pcl_conversions/pcl_conversions.h>
#include <pcl_ros/transforms.hpp>
#include <pcl/surface/convex_hull.h>
#include <pcl/search/octree.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/common/common.h>

#include <object_msgs/srv/detect_object.hpp>

#include <tf2_eigen/tf2_eigen.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

using namespace quasi;

void PerceptionNode::execute_detect_object_goal(const std::shared_ptr<rclcpp_action::ServerGoalHandle<quasi_r2_msgs::action::DetectObject>> goal_handle) {
  auto goal = goal_handle->get_goal();
  auto result = std::make_shared<quasi_r2_msgs::action::DetectObject::Result>();
  auto feedback = std::make_shared<quasi_r2_msgs::action::DetectObject::Feedback>();

  try {
    rclcpp::CallbackGroup::SharedPtr callback_group = create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive, false);
    rclcpp::executors::SingleThreadedExecutor callback_group_executor;
    callback_group_executor.add_callback_group(callback_group, get_node_base_interface());

    const auto& object_name = goal->object_name;
    RCLCPP_DEBUG(get_logger(), "Detecting object: %s", object_name.c_str());
    object_msgs::msg::ObjectsInBoxes objects;
    if( !object_name.empty() ) {
      // call OpenVino Object Recognition service
      auto client = create_client<object_msgs::srv::DetectObject>(get_parameter("object_detection_service").as_string(), rclcpp::ServicesQoS(), callback_group);
      auto detect_object_request = std::make_shared<object_msgs::srv::DetectObject::Request>();
      auto future_result = client->async_send_request(detect_object_request);

      // TODO: add cancellation
      auto rc = callback_group_executor.spin_until_future_complete(future_result, std::chrono::milliseconds(get_parameter("object_recognition_timeout").as_int()));
      if( rc == rclcpp::FutureReturnCode::TIMEOUT)
        throw std::runtime_error("Object Recognition timed out");
      else if( rc != rclcpp::FutureReturnCode::SUCCESS)
        throw std::runtime_error("async_send_request to /openvino_image_object_server/service failed");

      auto detect_object_response = future_result.get();

      objects = detect_object_response->objects;
      objects.objects_vector.clear();

      if(object_name != "*") {
        // Filter out recognized objects not matching the object specified in request
        for(const auto& object : detect_object_response->objects.objects_vector) {
          if(object_name != object.object.object_name) {
            RCLCPP_DEBUG(get_logger(), "Detected object \"%s\" which is not the expected \"%s\", skipping", object.object.object_name.c_str(), object_name.c_str());
            continue;
          }
          objects.objects_vector.push_back(object);
        }

        if(objects.objects_vector.empty()) {
          RCLCPP_INFO(get_logger(), "Object %s was not detected", goal->object_name.c_str());
          goal_handle->abort(result);
          return;
        }
      }
      else {
        if(detect_object_response->objects.objects_vector.empty()) {
          RCLCPP_INFO(get_logger(), "No objects were detected");
          goal_handle->abort(result);
          return;
        }
        std::copy(detect_object_response->objects.objects_vector.begin(), detect_object_response->objects.objects_vector.end(), std::back_inserter(objects.objects_vector));
      }
    }

    //goal_handle->publish_feedback(feedback);
    for( const auto& object : objects.objects_vector)
      RCLCPP_DEBUG(get_logger(), "Detected object %s at %d, %d [%d, %d]",
                   object.object.object_name.c_str(), object.roi.x_offset, object.roi.y_offset, object.roi.width, object.roi.height);

    // Set workspace
    const vision_msgs::msg::BoundingBox3DArray& workspace = goal->workspace;
    const auto shape_type = goal->shape_type;
    double area_height = goal->area_height;

    // Subscribe to point clouds using the local executor created on this thread
    auto subscription_options = rclcpp_lifecycle::create_default_subscription_options<std::allocator<void>>();
    subscription_options.callback_group = callback_group;

    std::promise<std::shared_ptr<std::vector<quasi_r2_msgs::msg::Object>>> cloud_segmentation_result_promise;
    auto cloud_segmentation_result_future = cloud_segmentation_result_promise.get_future();

    std::vector<sensor_msgs::msg::PointCloud2::ConstSharedPtr> cloud_msgs;
    message_filters::Subscriber<sensor_msgs::msg::PointCloud2, rclcpp_lifecycle::LifecycleNode> pcl_sub;
    message_filters::Subscriber<sensor_msgs::msg::CameraInfo, rclcpp_lifecycle::LifecycleNode> info_sub;
    typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::msg::PointCloud2, sensor_msgs::msg::CameraInfo> PointCloudSyncPolicy;
    message_filters::Synchronizer<PointCloudSyncPolicy> synchronizer(PointCloudSyncPolicy(5), pcl_sub, info_sub);
    synchronizer.registerCallback([&, this] (sensor_msgs::msg::PointCloud2::ConstSharedPtr pcl_msg,
                                             sensor_msgs::msg::CameraInfo::ConstSharedPtr color_camera_info_msg, ...) {
      const_cast<sensor_msgs::msg::PointCloud2*>(pcl_msg.get())->is_dense = false;  // cannot rely on dense flag because ignition gazebo incorrectly sets it to true
      cloud_msgs.push_back(std::move(pcl_msg));
      if(cloud_msgs.size() >= 2) {
        auto graspable_objects = detect_object_on_pointcloud_message(cloud_msgs, std::move(color_camera_info_msg), objects, workspace, area_height, shape_type);
        cloud_msgs.clear();
        if(graspable_objects) {
          pcl_sub.unsubscribe();
          info_sub.unsubscribe();
          cloud_segmentation_result_promise.set_value(graspable_objects);
        }
      }
    });

    pcl_sub.subscribe(this, get_parameter("pointcloud_topic").as_string(), rclcpp::SensorDataQoS().get_rmw_qos_profile(), subscription_options);
    info_sub.subscribe(this, get_parameter("color_camera_info_topic").as_string(), rclcpp::SensorDataQoS().get_rmw_qos_profile(), subscription_options);

    // TODO: add cancellation
    auto rc = callback_group_executor.spin_until_future_complete(cloud_segmentation_result_future, std::chrono::milliseconds(get_parameter("cloud_segmentation_timeout").as_int()));
    if( rc == rclcpp::FutureReturnCode::TIMEOUT)
      throw std::runtime_error("cloud segmentation timed out");
    else if( rc != rclcpp::FutureReturnCode::SUCCESS)
      throw std::runtime_error("cloud segmentation failed");

    result->objects = *cloud_segmentation_result_future.get();

    if(goal_handle->is_canceling()) {
      RCLCPP_INFO(get_logger(), "Detect Object goal execution canceled");
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
    RCLCPP_FATAL(get_logger(), "generic exception caught while processing Detect Object goal");
    goal_handle->abort(result);
  }
}

std::shared_ptr<std::vector<quasi_r2_msgs::msg::Object>> PerceptionNode::detect_object_on_pointcloud_message(std::vector<sensor_msgs::msg::PointCloud2::ConstSharedPtr> cloud_msgs,
                                                                                                             sensor_msgs::msg::CameraInfo::ConstSharedPtr color_camera_info_msg,
                                                                                                             const object_msgs::msg::ObjectsInBoxes& recognized_objects,
                                                                                                             const vision_msgs::msg::BoundingBox3DArray& workspace,
                                                                                                             float area_height,
                                                                                                             uint8_t shape_type) {
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud;
  for(const auto& cloud_msg : cloud_msgs) {
    if(cloud) {
      pcl::PointCloud<pcl::PointXYZRGB> c;
      pcl::fromROSMsg(*cloud_msg, c);
      *cloud += c;
    }
    else {
      cloud = pcl::make_shared<pcl::PointCloud<pcl::PointXYZRGB>>();
      pcl::fromROSMsg(*cloud_msg, *cloud);
    }
  }

  const auto& last_cloud_msg = cloud_msgs.back();

  image_geometry::PinholeCameraModel color_camera_model;
  color_camera_model.fromCameraInfo(color_camera_info_msg);

  auto processing_frame_id = get_parameter("frame_id").as_string();
  geometry_msgs::msg::TransformStamped processing_to_color_optical_transform;

  // Obtain transforms and transform point cloud to our target frame
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_transformed(new pcl::PointCloud<pcl::PointXYZRGB>);
  try {
    auto cloud_camera_to_processing_transform = tf_buffer_->lookupTransform(processing_frame_id, cloud->header.frame_id, last_cloud_msg->header.stamp, rclcpp::Duration::from_seconds(0.5));
    pcl_ros::transformPointCloud(*cloud, *cloud_transformed, cloud_camera_to_processing_transform);
    cloud_transformed->header.frame_id = processing_frame_id;

    processing_to_color_optical_transform = tf_buffer_->lookupTransform(color_camera_model.tfFrame(), processing_frame_id, last_cloud_msg->header.stamp, rclcpp::Duration::from_seconds(0.5));

  }
  catch (tf2::TransformException& ex)
  {
    RCLCPP_ERROR_STREAM(get_logger(), "Error getting transform for received cloud: " << ex.what());
    return nullptr;
  }

  // Crop workspace, if needed
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_cropped = filter_cloud_by_workspace(cloud_transformed, workspace);

  std::string camera_non_optical_frame_id = cloud->header.frame_id;
  {
    auto pos = camera_non_optical_frame_id.find("optical_");
    if(pos != std::string::npos)
      camera_non_optical_frame_id.erase(pos, strlen("optical_"));
  }

  // Estimate point normals
  pcl::PointCloud<pcl::PointNormal>::Ptr cloud_normals(new pcl::PointCloud<pcl::PointNormal>);
  pcl::NormalEstimationOMP<pcl::PointXYZRGB, pcl::PointNormal> ne;
  ne.setInputCloud(cloud_cropped);
  ne.setKSearch(30);
  //ne.setRadiusSearch(0.05);
  auto camera_transform = tf_buffer_->lookupTransform(processing_frame_id, camera_non_optical_frame_id, tf2::TimePointZero);
  ne.setViewPoint((float)camera_transform.transform.translation.x, (float)camera_transform.transform.translation.y, (float)camera_transform.transform.translation.z);
  ne.compute(*cloud_normals);

  // Run object segmentation pipeline to get clusters corresponding to individual scene objects
  if( !object_segmentation_->recognize(cloud_cropped, cloud_normals, camera_transform, cloud_viewer_) )
    return nullptr;

  auto area_height_threshold = get_parameter("area_height_threshold").as_double();
  std::string save_clouds_prefix = get_parameter("save_object_clouds").as_string();

  std::vector<std::pair<const object_msgs::msg::ObjectInBox*, std::shared_ptr<ObjectSegmentation::SegmentedObject>>> detected_objects;

  if(!save_clouds_prefix.empty()) {
    // save segmented objects to PCD file for debugging
    const auto& segmented_objects = object_segmentation_->get_segmented_objects();
    for( size_t i = 0; i < segmented_objects.size(); ++i ) {
      pcl::io::savePCDFile(save_clouds_prefix + "/segment" + std::to_string(i) + ".pcd",
                           *segmented_objects[i]->cluster_cloud, true);
    }
  }

  // Intersect point clouds of all segmented objects with ROI(s) of recognized objects
  if(!recognized_objects.objects_vector.empty()) {
    for( const auto& recognized_object : recognized_objects.objects_vector) {
      auto roi = recognized_object.roi;
      rclcpp::Parameter reduce_roi_parameter;
      if(get_parameter("reduce_roi", reduce_roi_parameter)) {   // Adjust ROI if needed
        auto reduce_roi = reduce_roi_parameter.get_value<int32_t>();
        roi.x_offset += (roi.width - roi.width / reduce_roi) / 2;
        roi.y_offset += (roi.height - roi.height / reduce_roi) / 2;
        roi.width = roi.width / reduce_roi;
        roi.height = roi.height / reduce_roi;
      }

      auto intersecting_segmented_objects = intersect_segmented_objects_with_roi2(color_camera_model, processing_to_color_optical_transform,
                                                                                  roi, object_segmentation_->get_segmented_objects());
      if (area_height >= 0 && !intersecting_segmented_objects.empty()) {
        intersecting_segmented_objects.erase(std::remove_if(intersecting_segmented_objects.begin(), intersecting_segmented_objects.end(),
          [&](const auto o) {
            pcl::PointXYZRGB min_pt, max_pt;
            pcl::getMinMax3D(*o->cluster_cloud, min_pt, max_pt);
            return std::abs(area_height - min_pt.z) > area_height_threshold;
          }), intersecting_segmented_objects.end());
      }

      for(const auto& intersecting_segmented_object : intersecting_segmented_objects)
        detected_objects.emplace_back(&recognized_object, intersecting_segmented_object);

  //    if(cloud_viewer_) {
  //      auto body = extract_roi_body(roi, cloud_transformed);
  //      cloud_viewer_->runOnVisualizationThreadOnce([=] (pcl::visualization::PCLVisualizer& pcl_visualizer) {
  //        std::string box_name = "cube_"s + std::to_string(1);
  //        Eigen::Isometry3f center = body->getPose().cast<float>();
  //        pcl_visualizer.addCube((Eigen::Vector3f) center.translation(), (Eigen::Quaternionf) center.rotation(),
  //                               body->getDimensions()[0], body->getDimensions()[1], body->getDimensions()[2], box_name);
  //      });
  //    }
    }
  }
  else {
    for(const auto& segmented_object : object_segmentation_->get_segmented_objects())
      detected_objects.emplace_back(nullptr, segmented_object);
  }

  RCLCPP_INFO(get_logger(), "Found %zu segmented objects intersecting with ROI(s) of recognized object(s)", detected_objects.size());

  auto graspable_objects = std::make_shared<std::vector<quasi_r2_msgs::msg::Object>>();
  // Convert detected objects to quasi_r2_msgs::msg::Object
  for(size_t i = 0; i < detected_objects.size(); ++i) {
    const auto& detected_object = detected_objects[i];
    auto& graspable_object = graspable_objects->emplace_back();
    graspable_object.header.frame_id = processing_frame_id;
    graspable_object.header.stamp = last_cloud_msg->header.stamp;
    if(detected_object.first) {
      const auto& object_name = detected_object.first->object.object_name;
      graspable_object.name = object_name;

      if(!save_clouds_prefix.empty()) {
        // save object cloud to PCD file for debugging
        pcl::io::savePCDFile(save_clouds_prefix + "/" + detected_object.first->object.object_name + std::to_string(i) + ".pcd",
                             *detected_object.second->cluster_cloud, true);
      }

      if(shape_type == 0 && object_types_config_.is_object()) {
        // try to obtain Object Shape from modl object types config, if available
        auto otit = object_types_config_.find(object_name);
        if(otit != object_types_config_.end()) {
          const auto& object_type_info = *otit;
          if(object_type_info.is_object()) {
            auto osit = object_type_info.find("shape");
            if(osit != object_type_info.end() && osit->is_string()) {
              std::string object_shape = *osit;
              if(!object_shape.empty())
                shape_type = parse_object_shape(object_shape);
            }
          }
        }
      }
    }

    RCLCPP_DEBUG(get_logger(), "Merging segmented object RGB cloud with %zu point with Normals cloud with %zu points",
                 detected_object.second->cluster_cloud->size(), detected_object.second->cluster_normals->size());
    { // concatenate RGB cluster cloud with normals and convert to message
      pcl::PointCloud<pcl::PointXYZRGBNormal> combined_cluster_cloud;
      pcl::copyPointCloud(*detected_object.second->cluster_cloud, combined_cluster_cloud);
      //pcl::concatenateFields(*detected_object.second->cluster_cloud, *detected_object.second->cluster_normals, combined_cluster_cloud);
      // pcl::concatenateFields does not properly merge XYZRGB and Normal points, so we have to merge in normals manually here
      for(size_t p = 0; p < combined_cluster_cloud.size(); ++p)
        std::copy(std::begin(detected_object.second->cluster_normals->points[p].normal), std::end(detected_object.second->cluster_normals->points[p].normal), 
                  std::begin(combined_cluster_cloud.points[p].normal));
      pcl::toROSMsg(combined_cluster_cloud, graspable_object.point_cluster);
    }

    if(detected_object.second->plane_coefficients) {
      // add surface coefficients
      for(size_t i = 0; i < detected_object.second->plane_coefficients->values.size(); ++i)
        graspable_object.surface.coef[i] = detected_object.second->plane_coefficients->values[i];
    }

    // use centroid for mesh pose and translate mesh accordingly
    geometry_msgs::msg::Pose mesh_pose;
    mesh_pose.position = tf2::toMsg((Eigen::Vector3d) detected_object.second->centroid.cast<double>().block<3, 1>(0, 0));
    RCLCPP_DEBUG(get_logger(), "Segment mesh pose (centroid) in %s frame: [%f,%f,%f] [%f,%f,%f,%f]", processing_frame_id.c_str(),
                 mesh_pose.position.x, mesh_pose.position.y, mesh_pose.position.z,
                 mesh_pose.orientation.x, mesh_pose.orientation.y, mesh_pose.orientation.z, mesh_pose.orientation.w);
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr centered_cloud(new pcl::PointCloud<pcl::PointXYZRGB>());
    pcl::demeanPointCloud(*detected_object.second->cluster_cloud, detected_object.second->centroid, *centered_cloud);

    // extract mesh
    pcl::PolygonMesh pcl_mesh;
    pcl::ConvexHull<pcl::PointXYZRGB> hull;
    hull.setInputCloud(centered_cloud);
    hull.setDimension(3);
    hull.setComputeAreaVolume(true);
    hull.MeshConstruction<pcl::PointXYZRGB>::reconstruct(pcl_mesh);
    shape_msgs::msg::Mesh mesh_msg;
    pcl_mesh_to_shape_mesh(pcl_mesh, mesh_msg);

    graspable_object.mesh_poses.push_back(mesh_pose);
    graspable_object.meshes.emplace_back(mesh_msg);

    RCLCPP_INFO(get_logger(), "Object shape type: %s (%d)", object_shape_name(shape_type).c_str(), int(shape_type));
    if(shape_type == shape_msgs::msg::SolidPrimitive::SPHERE ||
       shape_type == shape_msgs::msg::SolidPrimitive::BOX ||
       shape_type == shape_msgs::msg::SolidPrimitive::CYLINDER ||
       shape_type == 255/* auto-detect */) {
      // extract shape

      shape_msgs::msg::SolidPrimitive primitive_msg;
      geometry_msgs::msg::Pose primitive_pose;

      if((shape_type == shape_msgs::msg::SolidPrimitive::BOX ||
          shape_type == shape_msgs::msg::SolidPrimitive::CYLINDER ||
          shape_type == 255 /*auto-detect*/) &&
         detected_object.second->plane_coefficients)
        // use shape extraction algorithm originally taken from simple_grasping library
        extract_primitive_shape_bc(shape_type,
                                   detected_object.second->cluster_cloud, detected_object.second->plane_coefficients,
                                   primitive_msg, primitive_pose);
      else
        // use geometric_shapes library for shape extraction
        extract_primitive_shape(shape_type,
                                graspable_object.mesh_poses[0], graspable_object.meshes[0],
                                primitive_pose, primitive_msg);

      padd_primitive(primitive_msg);
      graspable_object.primitives.push_back(primitive_msg);
      graspable_object.primitive_poses.push_back(primitive_pose);
    }
  }

  return graspable_objects;
}

void PerceptionNode::padd_primitive(shape_msgs::msg::SolidPrimitive& solid_primitive) {
  double padding = get_parameter("primitive_size_padding").as_double();
  if(padding == 0)
    return;
  size_t ndims = 0;
  switch(solid_primitive.type) {
    case shape_msgs::msg::SolidPrimitive::SPHERE:
      ndims = 1;
      break;
    case shape_msgs::msg::SolidPrimitive::CYLINDER:
      ndims = 2;
      break;
    case shape_msgs::msg::SolidPrimitive::BOX:
      ndims = 3;
      break;
  }
  for(size_t i = 0; i < ndims; ++i)
    solid_primitive.dimensions[i] += padding;
}

std::shared_ptr<bodies::Body> PerceptionNode::extract_roi_body(const sensor_msgs::msg::RegionOfInterest& roi, const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud) {
  double min_x = std::numeric_limits<double>::max(), max_x = -std::numeric_limits<double>::max(),
      min_y = std::numeric_limits<double>::max(), max_y = -std::numeric_limits<double>::max(),
      min_z = std::numeric_limits<double>::max(), max_z = -std::numeric_limits<double>::max();
  for(size_t y = 0; y < roi.height; y++) {  // rows
    size_t idx = (y + roi.y_offset) * cloud->width + roi.x_offset;
    for(size_t x = 0; x < roi.width; x++) {  // columns
      const auto& point = cloud->points[idx + x];
      if(!isnan(point.x) && !isnan(point.y) && !isnan(point.z)) {
        if(point.x < min_x)
          min_x = point.x;
        if(point.x > max_x)
          max_x = point.x;
        if(point.y < min_y)
          min_y = point.y;
        if(point.y > max_y)
          max_y = point.y;
        if(point.z < min_z)
          min_z = point.z;
        if(point.z > max_z)
          max_z = point.z;
      }
    }
  }

  RCLCPP_DEBUG(get_logger(), "min/max: %f %f   %f %f   %f %f", min_x, max_x, min_y, max_y, min_z, max_z);
  if(min_x == std::numeric_limits<double>::max())
    return nullptr;

  shapes::Box shape(max_x-min_x, max_y-min_y, max_z-min_z);
  std::shared_ptr<bodies::Body> box = std::make_shared<bodies::Box>(&shape);

  Eigen::Isometry3d pose = Eigen::Translation3d((min_x+max_x)/2.0, (min_y+max_y)/2.0, (min_z+max_z)/2.0) * Eigen::Quaterniond(1.0, 0.0, 0.0, 0.0);
  RCLCPP_DEBUG(get_logger(), "pose in map frame: %f %f %f  %f %f %f", pose.translation().x(), pose.translation().y(), pose.translation().z(),
               box->getDimensions()[0], box->getDimensions()[1], box->getDimensions()[2]);
  auto frame_id = get_parameter("frame_id").as_string();
  if(frame_id != cloud->header.frame_id)
    tf2::doTransform(pose, pose, tf_buffer_->lookupTransform(frame_id, cloud->header.frame_id, tf2::TimePointZero));
  box->setPose(pose);
  return box;
}

std::vector<std::shared_ptr<ObjectSegmentation::SegmentedObject>> PerceptionNode::intersect_segmented_objects_with_roi(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud_transformed,
                                                                                                                       const sensor_msgs::msg::RegionOfInterest& roi,
                                                                                                                       const std::vector<std::shared_ptr<ObjectSegmentation::SegmentedObject>>& segmented_objects) {
  std::vector<std::shared_ptr<ObjectSegmentation::SegmentedObject>> detected_objects;

  // Extract indices of points corresponding to ROI
  pcl::IndicesPtr roi_indices = pcl::make_shared<pcl::Indices>();
  for(size_t y = 0; y < roi.height; y++) {  // rows
    size_t idx = (y + roi.y_offset) * cloud_transformed->width + roi.x_offset;
    for(size_t x = 0; x < roi.width; x++) {  // columns
      const auto& point = cloud_transformed->points[idx + x];
      if(!isnan(point.x) && !isnan(point.y) && !isnan(point.z))
        roi_indices->push_back(idx + x);
    }
  }

  RCLCPP_DEBUG(get_logger(), "ROI contains %zu total points", roi_indices->size());

  if(roi_indices->empty())
    return detected_objects;

  double min_roi_intersection = get_parameter("min_roi_intersection").as_double();
  double roi_intersection_search_radius = get_parameter("roi_intersection_search_radius").as_double();

  // Intersect each segmented object with ROI using Octree
  pcl::search::Octree<pcl::PointXYZRGB> roi_search(0.001);
  roi_search.setInputCloud(cloud_transformed, roi_indices);
  for( size_t i = 0; i < segmented_objects.size(); ++i ) {
    const auto& segmented_object = segmented_objects[i];
    size_t intersection = 0;
    for(const auto& pt : *segmented_object->cluster_cloud) {
      std::vector<int> k_indices;
      std::vector<float> k_sqr_distances;
      if(roi_search.radiusSearch(pt, roi_intersection_search_radius, k_indices, k_sqr_distances) > 0)
        ++intersection;
    }
    RCLCPP_DEBUG(get_logger(), "Segment %zu intersection with ROI %zu out of %zu total points. Threshold: %f",
                 i, intersection, segmented_object->cluster_cloud->size(), segmented_object->cluster_cloud->size() * min_roi_intersection);
    if((double)intersection / (double)segmented_object->cluster_cloud->size() >= min_roi_intersection)
      detected_objects.push_back(segmented_object);
  }

  return detected_objects;
}


std::vector<std::shared_ptr<ObjectSegmentation::SegmentedObject>> PerceptionNode::intersect_segmented_objects_with_roi2(
    const image_geometry::PinholeCameraModel& color_camera_model,
    const geometry_msgs::msg::TransformStamped& transform,
    const sensor_msgs::msg::RegionOfInterest& roi_msg,
    const std::vector<std::shared_ptr<ObjectSegmentation::SegmentedObject>>& segmented_objects) {
  std::vector<std::shared_ptr<ObjectSegmentation::SegmentedObject>> detected_objects;
  cv::Rect roi(roi_msg.x_offset, roi_msg.y_offset, roi_msg.width, roi_msg.height);

  double min_roi_intersection = get_parameter("min_roi_intersection").as_double();

  // Intersect each segmented object with ROI
  for( size_t i = 0; i < segmented_objects.size(); ++i ) {
    const auto& segmented_object = segmented_objects[i];

    // transform segment cloud to color optical frame
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr segment_cloud_color_optical(new pcl::PointCloud<pcl::PointXYZRGB>);
    pcl_ros::transformPointCloud(*segmented_object->cluster_cloud, *segment_cloud_color_optical, transform);
    segment_cloud_color_optical->header.frame_id = color_camera_model.tfFrame();  // just in-case, record correct frame

    size_t intersection = 0;
    for(const auto& pt : *segment_cloud_color_optical) {
      // project each point to color camera matrix and check if it is within ROI
      auto px_color = color_camera_model.project3dToPixel(cv::Point3d(pt.x, pt.y, pt.z));
      //RCLCPP_DEBUG(get_logger(), "Point %f,%f,%f projected to %f,%f", pt.x, pt.y, pt.z, px_color.x, px_color.y);
      if( roi.contains(px_color) )
        ++intersection;
    }

    RCLCPP_DEBUG(get_logger(), "Segment %zu intersection with ROI %zu out of %zu total points. Threshold: %f",
                 i, intersection, segmented_object->cluster_cloud->size(), segmented_object->cluster_cloud->size() * min_roi_intersection);
    if((double)intersection / (double)segmented_object->cluster_cloud->size() >= min_roi_intersection)
      detected_objects.push_back(segmented_object);
  }

  return detected_objects;
}
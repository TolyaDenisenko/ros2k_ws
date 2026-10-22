// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#include "gpd_grasp_engine.h"
#include "rclcpp/rclcpp.hpp"

#include <tf2_eigen/tf2_eigen.hpp>
#include <pcl_ros/transforms.hpp>

#define GPD_CAMERA_FRAME "realsense_depth_frame"

GpdGraspEngine::GpdGraspEngine(grasp_ros2::GraspParameters& params, std::shared_ptr<tf2_ros::Buffer> tf_buffer,
                               rclcpp::Publisher<quasi_r2_msgs::msg::GraspConfigList>::SharedPtr grasps_pub,
                               rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr grasps_rviz_pub,
                               rclcpp::Clock::SharedPtr clock)
: grasp_params_(params), tf_buffer_(std::move(tf_buffer)),
  grasps_pub_(std::move(grasps_pub)), grasps_rviz_pub_(std::move(grasps_rviz_pub))
, logger_(rclcpp::get_logger("quasi_r2_find_grasp_service")), clock_(clock)
{
}

void GpdGraspEngine::findGrasps(const GraspPlanning::Request::SharedPtr& request, std::vector<moveit_msgs::msg::Grasp>& result)
{
  Eigen::Vector3d view_point_;
  std::vector<double> camera_position(std::initializer_list<double>({0, 0, 0}));
  view_point_ << camera_position[0], camera_position[1], camera_position[2];
  Eigen::Matrix3Xd view_points(3, 1);
  view_points.col(0) = view_point_;

  const auto& pointCloud = request->object.point_cluster;
  pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
  pcl::fromROSMsg(pointCloud, *cloud);
  RCLCPP_INFO(logger_, "Received request with cloud size %zu in %s frame", cloud->size(), pointCloud.header.frame_id.c_str());

  // GraspDetector expect PointCloud in camera optical frame with the origin at camera lens
  pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr cloud_transformed(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
  if( !pcl_ros::transformPointCloud(GPD_CAMERA_FRAME, *cloud, *cloud_transformed, *tf_buffer_) ) {
    RCLCPP_ERROR(logger_, "Failed to transform pointcloud from %s to %s frame", cloud->header.frame_id.c_str(), GPD_CAMERA_FRAME);
    return;
  }
  auto cloud_camera = std::make_unique<gpd::util::Cloud>(cloud_transformed, 0, view_points);

  // Get grasp approach vector either from request or default from the parameters and transform it to camera frame
  geometry_msgs::msg::Vector3 grasp_vector;
  std::string grasp_vector_frame_id = grasp_params_.grasp_frame_id;
  if( request->grasp_approach.direction.vector.x != 0.0 || request->grasp_approach.direction.vector.y != 0.0 || request->grasp_approach.direction.vector.z != 0.0) {
    // Use grasp approach vector from parameters
    grasp_vector = request->grasp_approach.direction.vector;
    if(!request->grasp_approach.direction.header.frame_id.empty())
      grasp_vector_frame_id = request->grasp_approach.direction.header.frame_id;
  }
  else
    grasp_vector = tf2::toMsg(grasp_params_.grasp_approach);  // use default grasp approach vector from parameters
  RCLCPP_INFO(logger_, "Grasp vector in frame %s: %f, %f, %f", grasp_vector_frame_id.c_str(), grasp_vector.x, grasp_vector.y, grasp_vector.z);

  auto transform = tf_buffer_->lookupTransform(GPD_CAMERA_FRAME, grasp_vector_frame_id, tf2::TimePointZero);
  geometry_msgs::msg::Vector3 grasp_vector_in_camera_frame;
  tf2::doTransform(grasp_vector, grasp_vector_in_camera_frame, transform);
  RCLCPP_INFO(logger_, "Grasp vector in camera frame: %f, %f, %f", grasp_vector_in_camera_frame.x, grasp_vector_in_camera_frame.y, grasp_vector_in_camera_frame.z);

  std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("find_grasp_server", rclcpp::NodeOptions().allow_undeclared_parameters(true).automatically_declare_parameters_from_overrides(true));
  gpd::GraspDetectionParameters detection_param;
  grasp_ros2::ROSParameters::getDetectionParams(node.get(), detection_param);
  detection_param.direction_ = {grasp_vector_in_camera_frame.x, grasp_vector_in_camera_frame.y, grasp_vector_in_camera_frame.z};
  if(!std::isnan(request->grasp_approach_direction_threshold) && request->grasp_approach_direction_threshold > 0.0 )
    detection_param.thresh_rad_ = request->grasp_approach_direction_threshold;
  auto grasp_detector_ = std::make_unique<gpd::GraspDetector>(detection_param);
  grasp_detector_->preprocessPointCloud(*cloud_camera);
  auto grasps = grasp_detector_->detectGrasps(*cloud_camera);
  fflush(stdout); // gpd::GraspDetector writes logs to stdout which is buffered, need to flush buffer here to see those logs

  RCLCPP_INFO(logger_, "Found grasps count = %zu", grasps.size());
  std_msgs::msg::Header grasp_config_header = pointCloud.header;
  grasp_config_header.frame_id = GPD_CAMERA_FRAME;
  quasi_r2_msgs::msg::GraspConfigList::SharedPtr selected_grasps_msg = createGraspConfigListMsg(grasps, request->max_grasps, grasp_config_header);
  if(grasps_pub_->get_subscription_count() > 0)
    grasps_pub_->publish(*selected_grasps_msg);
  convertGraspConfigToGraspMsg(selected_grasps_msg, result);
  if (grasps_rviz_pub_) {
    const gpd::candidate::HandSearch::Parameters & params = grasp_detector_->getHandSearchParameters();
    grasps_rviz_pub_->publish(convertToVisualGraspMsg(grasps, params.hand_geometry_.outer_diameter_,
                                                      params.hand_geometry_.depth_,
                                                      params.hand_geometry_.finger_width_, params.hand_geometry_.height_, grasp_config_header));
  }
}

moveit_msgs::msg::Grasp GpdGraspEngine::toMoveIt(
  const quasi_r2_msgs::msg::GraspConfig & grasp,
  const std_msgs::msg::Header & header)
{
  moveit_msgs::msg::Grasp msg;
  msg.grasp_pose.header = header;
  msg.grasp_quality = grasp.score;

  double offset = grasp_params_.eef_offset;
  // set grasp position, translation from hand-base to the parent-link of EEF
  msg.grasp_pose.pose.position.x = grasp.bottom.x + grasp_params_.grasp_offset[0] - grasp.approach.x * offset;
  msg.grasp_pose.pose.position.y = grasp.bottom.y + grasp_params_.grasp_offset[1] - grasp.approach.y * offset;
  msg.grasp_pose.pose.position.z = grasp.bottom.z + grasp_params_.grasp_offset[2] - grasp.approach.z * offset;

  // rotation matrix https://github.com/atenpas/gpd/blob/master/tutorials/hand_frame.png
  tf2::Matrix3x3 r(
      grasp.approach.x, grasp.binormal.x, grasp.axis.x,
      grasp.approach.y, grasp.binormal.y, grasp.axis.y,
      grasp.approach.z, grasp.binormal.z, grasp.axis.z);
  tf2::Quaternion quat;
  r.getRotation(quat);
  // EEF yaw-offset to its parent-link (last link of arm)
  quat *= tf2::Quaternion(tf2::Vector3(0, 0, 1), grasp_params_.eef_yaw_offset);
  quat.normalize();
  // set grasp orientation
  msg.grasp_pose.pose.orientation = tf2::toMsg(quat);
  RCLCPP_INFO(logger_, "==============offset is %f quat [%f %f %f %f]", offset,
    msg.grasp_pose.pose.orientation.x, msg.grasp_pose.pose.orientation.y,
    msg.grasp_pose.pose.orientation.z, msg.grasp_pose.pose.orientation.w);

  // set pre-grasp approach
  msg.pre_grasp_approach.direction.header = header;
  msg.pre_grasp_approach.direction.vector = grasp.approach;
  msg.pre_grasp_approach.min_distance = grasp_params_.grasp_min_distance;
  msg.pre_grasp_approach.desired_distance = grasp_params_.grasp_desired_distance;

  // set post-grasp retreat
  msg.post_grasp_retreat.direction.header = header;
  msg.post_grasp_retreat.direction.vector.x = -grasp.approach.x;
  msg.post_grasp_retreat.direction.vector.y = -grasp.approach.y;
  msg.post_grasp_retreat.direction.vector.z = -grasp.approach.z;
  msg.post_grasp_retreat.min_distance = grasp_params_.grasp_min_distance;
  msg.post_grasp_retreat.desired_distance = grasp_params_.grasp_desired_distance;

  // set pre-grasp posture
  msg.pre_grasp_posture.joint_names = grasp_params_.finger_joint_names;
  msg.pre_grasp_posture.points.push_back(grasp_params_.finger_points_open);

  // set grasp posture
  msg.grasp_posture.joint_names = grasp_params_.finger_joint_names;
  msg.grasp_posture.points.push_back(grasp_params_.finger_points_close);

  return msg;
}

bool GpdGraspEngine::check_boundary(const geometry_msgs::msg::Point & p)
{
  return p.x >= grasp_params_.grasp_boundry[0] && p.x <= grasp_params_.grasp_boundry[1] &&
         p.y >= grasp_params_.grasp_boundry[2] && p.y <= grasp_params_.grasp_boundry[3] &&
         p.z >= grasp_params_.grasp_boundry[4] && p.z <= grasp_params_.grasp_boundry[5];
}

bool GpdGraspEngine::transform(
  const quasi_r2_msgs::msg::GraspConfig & from,
  quasi_r2_msgs::msg::GraspConfig & to,
  const std_msgs::msg::Header & header)
{
  geometry_msgs::msg::PointStamped from_top, to_top, from_surface, to_surface, from_bottom, to_bottom;
  geometry_msgs::msg::Vector3Stamped from_approach, to_approach, from_binormal, to_binormal, from_axis, to_axis;

  to = from;

  from_top.point = from.top;
  from_top.header = header;
  from_surface.point = from.surface;
  from_surface.header = header;
  from_bottom.point = from.bottom;
  from_bottom.header = header;
  from_approach.vector = from.approach;
  from_approach.header = header;
  from_binormal.vector = from.binormal;
  from_binormal.header = header;
  from_axis.vector = from.axis;
  from_axis.header = header;

  try {
    tf_buffer_->transform(from_top, to_top, grasp_params_.grasp_frame_id);
    tf_buffer_->transform(from_surface, to_surface, grasp_params_.grasp_frame_id);
    tf_buffer_->transform(from_bottom, to_bottom, grasp_params_.grasp_frame_id);
    tf_buffer_->transform(from_approach, to_approach, grasp_params_.grasp_frame_id);
    tf_buffer_->transform(from_binormal, to_binormal, grasp_params_.grasp_frame_id);
    tf_buffer_->transform(from_axis, to_axis, grasp_params_.grasp_frame_id);
  } catch (tf2::TransformException & ex) {
    RCLCPP_WARN(logger_, "error transforming grasp: %s", ex.what());
    return false;
  }

  to.top = to_top.point;
  to.surface = to_surface.point;
  to.bottom = to_bottom.point;
  to.approach = to_approach.vector;
  to.binormal = to_binormal.vector;
  to.axis = to_axis.vector;
  return true;
}

void GpdGraspEngine::convertToGraspConfigMsg(const std::unique_ptr<gpd::candidate::Hand> & hand, quasi_r2_msgs::msg::GraspConfig& msg)
{
  msg.bottom = tf2::toMsg(hand->getGraspBottom());
  msg.top = tf2::toMsg(hand->getGraspTop());
  msg.surface = tf2::toMsg(hand->getGraspSurface());
  tf2::toMsg(hand->getApproach(), msg.approach);
  tf2::toMsg(hand->getBinormal(), msg.binormal);
  tf2::toMsg(hand->getAxis(), msg.axis);
  msg.width = hand->getGraspWidth();
  msg.score = hand->getScore();
  msg.sample = tf2::toMsg(hand->getSample());
}

quasi_r2_msgs::msg::GraspConfigList::SharedPtr GpdGraspEngine::createGraspConfigListMsg(const std::vector<std::unique_ptr<gpd::candidate::Hand>> & hands,
                                                                                        int16_t max_grasps,
                                                                                        const std_msgs::msg::Header& header)
{
  quasi_r2_msgs::msg::GraspConfigList::SharedPtr config_list_msg = std::make_shared<quasi_r2_msgs::msg::GraspConfigList>();

  static bool tf_needed = (grasp_params_.grasp_frame_id != header.frame_id);
  RCLCPP_INFO(logger_, "tf_needed %d", tf_needed);

  for (const auto & hand : hands) {
    // skip low score grasp
    if (hand->getScore() < grasp_params_.grasp_score_threshold) {
      RCLCPP_INFO(logger_, "skip low score grasps %f", hand->getScore());
      continue;
    }

    auto& grasp_config_msg = config_list_msg->grasps.emplace_back();

    // transform grasp to grasp_frame_id
    if (tf_needed) {
      quasi_r2_msgs::msg::GraspConfig tmp_msg;
      convertToGraspConfigMsg(hand, tmp_msg);
      if(!transform(tmp_msg, grasp_config_msg, header)) {
        // skip grasp on transformation failure
        continue;
      }
    } else
      convertToGraspConfigMsg(hand, grasp_config_msg);

    if(max_grasps > 0 && config_list_msg->grasps.size() >= max_grasps)
      break;
  }

  config_list_msg->header = header;
  config_list_msg->header.frame_id = grasp_params_.grasp_frame_id;

  return config_list_msg;
}

void GpdGraspEngine::convertGraspConfigToGraspMsg(
    const quasi_r2_msgs::msg::GraspConfigList::SharedPtr& msg,
    std::vector<moveit_msgs::msg::Grasp>& result_grasps)
{
  std_msgs::msg::Header header;
  header.frame_id = msg->header.frame_id;
  header.stamp = msg->header.stamp;
  for (const auto& grasp : msg->grasps) {
    if (grasp_params_.grasp_approach_angle < M_PI) {
      // skip unacceptable approach
      tf2::Vector3 approach(grasp.approach.x, grasp.approach.y, grasp.approach.z);
      double ang = tf2::tf2Angle(grasp_params_.grasp_approach, approach);
      if (std::isnan(ang) ||
          ang < -grasp_params_.grasp_approach_angle || ang > grasp_params_.grasp_approach_angle)
      {
        RCLCPP_INFO(logger_, "skip unacceptable approach %f, %f, %f, angle: %f", grasp.approach.x, grasp.approach.y, grasp.approach.z, ang);
        continue;
      }
    }

    // skip out of boundary grasps
    if (check_boundary(grasp.bottom))
    {
      // translate into moveit grasp
      RCLCPP_INFO(logger_, "point [%f %f %f]", grasp.bottom.x, grasp.bottom.y, grasp.bottom.z);
      result_grasps.push_back(toMoveIt(grasp, header));
    }
    else
      RCLCPP_INFO(logger_, "point [%f %f %f] is outside of boundaries", grasp.bottom.x, grasp.bottom.y, grasp.bottom.z);
  }
}

visualization_msgs::msg::MarkerArray GpdGraspEngine::convertToVisualGraspMsg(
    const std::vector<std::unique_ptr<gpd::candidate::Hand>> & hands,
    double outer_diameter, double hand_depth, double finger_width, double hand_height,
    std_msgs::msg::Header header)
{
  double width = outer_diameter;
  double hw = 0.5 * width;

  visualization_msgs::msg::MarkerArray marker_array;
  visualization_msgs::msg::Marker left_finger, right_finger, base, approach;
  Eigen::Vector3d left_bottom, right_bottom, left_top, right_top, left_center, right_center, approach_center, base_center;
  std_msgs::msg::ColorRGBA color;

  header.stamp = clock_->now() - rclcpp::Duration(std::chrono::milliseconds(50));

  for (uint32_t i = 0; i < hands.size(); i++) {
    left_bottom = hands[i]->getGraspBottom() - (hw - 0.5 * finger_width) * hands[i]->getBinormal();
    right_bottom = hands[i]->getGraspBottom() + (hw - 0.5 * finger_width) * hands[i]->getBinormal();
    left_top = left_bottom + hand_depth * hands[i]->getApproach();
    right_top = right_bottom + hand_depth * hands[i]->getApproach();
    left_center = left_bottom + 0.5 * (left_top - left_bottom);
    right_center = right_bottom + 0.5 * (right_top - right_bottom);
    base_center = left_bottom + 0.5 * (right_bottom - left_bottom) - 0.01 * hands[i]->getApproach();
    approach_center = base_center - 0.04 * hands[i]->getApproach();

    setMarkerColor(color, *hands[i]);

    base = createHandBaseMarker(left_bottom, right_bottom, color, hands[i]->getFrame(), 0.02, hand_height, i, header);
    left_finger = createFingerMarker(left_center, color, hands[i]->getFrame(), hand_depth, finger_width, hand_height, i * 3, header);
    right_finger = createFingerMarker(right_center, color, hands[i]->getFrame(), hand_depth, finger_width, hand_height, i * 3 + 1, header);
    approach = createFingerMarker(approach_center, color, hands[i]->getFrame(), 0.08, finger_width, hand_height, i * 3 + 2, header);

    marker_array.markers.push_back(left_finger);
    marker_array.markers.push_back(right_finger);
    marker_array.markers.push_back(approach);
    marker_array.markers.push_back(base);
  }

  return marker_array;
}

visualization_msgs::msg::Marker GpdGraspEngine::createFingerMarker(
    const Eigen::Vector3d & center, const std_msgs::msg::ColorRGBA& color,
    const Eigen::Matrix3d & frame, double length, double width, double height, int id,
    const std_msgs::msg::Header& header)
{
  visualization_msgs::msg::Marker marker;
  marker.header = header;
  marker.ns = "finger";
  marker.id = id;
  marker.type = visualization_msgs::msg::Marker::CUBE;
  marker.action = visualization_msgs::msg::Marker::ADD;
  marker.pose.position.x = center(0);
  marker.pose.position.y = center(1);
  marker.pose.position.z = center(2);
  marker.lifetime = rclcpp::Duration(60.0, 0);

  // use orientation of hand frame
  Eigen::Quaterniond quat(frame);
  marker.pose.orientation.x = quat.x();
  marker.pose.orientation.y = quat.y();
  marker.pose.orientation.z = quat.z();
  marker.pose.orientation.w = quat.w();

  // these scales are relative to the hand frame (unit: meters)
  marker.scale.x = length;  // forward direction
  marker.scale.y = width;  // hand closing direction
  marker.scale.z = height;  // hand vertical direction

  marker.color = color;

  return marker;
}

visualization_msgs::msg::Marker GpdGraspEngine::createHandBaseMarker(
    const Eigen::Vector3d & start, const Eigen::Vector3d & end, const std_msgs::msg::ColorRGBA& color,
    const Eigen::Matrix3d & frame, double length, double height, int id,
    const std_msgs::msg::Header& header)
{
  Eigen::Vector3d center = start + 0.5 * (end - start);

  visualization_msgs::msg::Marker marker;
  marker.header = header;
  marker.ns = "gripper_base";
  marker.id = id;
  marker.type = visualization_msgs::msg::Marker::CUBE;
  marker.action = visualization_msgs::msg::Marker::ADD;
  marker.pose.position.x = center(0);
  marker.pose.position.y = center(1);
  marker.pose.position.z = center(2);
  marker.lifetime = rclcpp::Duration(60.0, 0);

  // use orientation of hand frame
  Eigen::Quaterniond quat(frame);
  marker.pose.orientation.x = quat.x();
  marker.pose.orientation.y = quat.y();
  marker.pose.orientation.z = quat.z();
  marker.pose.orientation.w = quat.w();

  // these scales are relative to the hand frame (unit: meters)
  marker.scale.x = length;  // forward direction
  marker.scale.y = (end - start).norm();  // hand closing direction
  marker.scale.z = height;  // hand vertical direction

  marker.color = color;

  return marker;
}

void GpdGraspEngine::setMarkerColor(std_msgs::msg::ColorRGBA& color, const gpd::candidate::Hand& hand) {
  if(hand.getScore() < grasp_params_.grasp_score_threshold) {
    color.a = 0.5;
    color.r = 153.0 / 255.0;
    color.g = 82.0 / 255.0;
    color.b = 44.0 / 255.0;
  }
/*  else if() {

  }
*/
else {
    color.a = 0.6;
    color.r = 0.0;
    color.g = 0.1;
    color.b = 1.0;
  }
}
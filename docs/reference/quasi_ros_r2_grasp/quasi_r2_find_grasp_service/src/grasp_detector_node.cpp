// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#include "grasp_detector_node.h"


GraspDetectorNode::GraspDetectorNode(const rclcpp::NodeOptions & options)
: Node("plan_grasps_node", options)
{
  tf_buffer_ = std::make_shared<tf2_ros::Buffer>(get_clock(), std::chrono::minutes(2));
  tf_buffer_->setUsingDedicatedThread(true);
  tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

  grasps_pub_ = create_publisher<quasi_r2_msgs::msg::GraspConfigList>("/grasps", rclcpp::SystemDefaultsQoS());
  bool rviz = get_parameter_or("rviz", false);
  if (rviz)
    grasps_rviz_pub_ = create_publisher<visualization_msgs::msg::MarkerArray>("/grasp_library/grasps_rviz", rclcpp::SystemDefaultsQoS());

  grasp_ros2::GraspParameters params;
  grasp_ros2::ROSParameters::getGraspParams(this, params);
  engine_ = std::make_unique<GpdGraspEngine>(params, tf_buffer_, grasps_pub_, grasps_rviz_pub_, get_clock());

  service_= create_service<GraspPlanning>("plan_grasps_service",
      [this](const GraspPlanning::Request::SharedPtr& request, GraspPlanning::Response::SharedPtr&& response) {
        engine_->findGrasps(request, response->grasps);
      });
}

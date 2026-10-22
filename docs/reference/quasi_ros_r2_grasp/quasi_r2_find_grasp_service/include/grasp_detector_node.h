// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#pragma once

#include "rclcpp/rclcpp.hpp"
#include <pcl_conversions/pcl_conversions.h>
#include <quasi_r2_msgs/msg/grasp_config_list.hpp>
#include "ros_params.hpp"
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>
#include <tf2_eigen/tf2_eigen.hpp>
#include "gpd_grasp_engine.h"

class GraspDetectorNode : public rclcpp::Node {
public:
  explicit GraspDetectorNode(const rclcpp::NodeOptions &options);

private:
  std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
  rclcpp::Service<GraspPlanning>::SharedPtr service_;
  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr grasps_rviz_pub_;
  rclcpp::Publisher<quasi_r2_msgs::msg::GraspConfigList>::SharedPtr grasps_pub_;
  std::unique_ptr<GpdGraspEngine> engine_;
};
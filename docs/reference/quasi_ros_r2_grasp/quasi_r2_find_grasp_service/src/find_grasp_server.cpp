// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#include "rclcpp/rclcpp.hpp"
#include <pcl_conversions/pcl_conversions.h>
#include <quasi_r2_msgs/msg/grasp_config_list.hpp>
#include "ros_params.hpp"
#include <tf2_eigen/tf2_eigen.hpp>
#include "gpd_grasp_engine.h"
#include "grasp_detector_node.h"

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);

  auto node_options = rclcpp::NodeOptions().allow_undeclared_parameters(true).automatically_declare_parameters_from_overrides(true);
  auto detect_node = std::make_shared<GraspDetectorNode>(node_options);
  auto LOGGER = rclcpp::get_logger("plan_grasps");
  RCLCPP_INFO(LOGGER, "Ready to find grasp");

  rclcpp::spin(detect_node);
  rclcpp::shutdown();
}
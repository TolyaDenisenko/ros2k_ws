// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#pragma once
#include "quasi_r2_msgs/srv/grasp_planning.hpp"
#include <visualization_msgs/msg/marker_array.hpp>
#include <pcl_conversions/pcl_conversions.h>
#include "ros_params.hpp"
#include <quasi_r2_msgs/msg/grasp_config_list.hpp>
#include <tf2_ros/buffer.h>

using GraspPlanning = quasi_r2_msgs::srv::GraspPlanning;

class GpdGraspEngine
{
public:
    GpdGraspEngine(grasp_ros2::GraspParameters& params, std::shared_ptr<tf2_ros::Buffer> tf_buffer,
                   rclcpp::Publisher<quasi_r2_msgs::msg::GraspConfigList>::SharedPtr grasps_pub,
                   rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr grasps_rviz_pub,
                   rclcpp::Clock::SharedPtr clock);
    void findGrasps(const GraspPlanning::Request::SharedPtr& request, std::vector<moveit_msgs::msg::Grasp>& result);

private:
    moveit_msgs::msg::Grasp toMoveIt(const quasi_r2_msgs::msg::GraspConfig& grasp, const std_msgs::msg::Header& header);

    bool check_boundary(const geometry_msgs::msg::Point & p);

    bool transform(const quasi_r2_msgs::msg::GraspConfig & from,
        quasi_r2_msgs::msg::GraspConfig & to,
        const std_msgs::msg::Header & header);

    void convertGraspConfigToGraspMsg(
        const quasi_r2_msgs::msg::GraspConfigList::SharedPtr& msg,
        std::vector<moveit_msgs::msg::Grasp>& result_grasps);

    static void convertToGraspConfigMsg(const std::unique_ptr<gpd::candidate::Hand> & hand, quasi_r2_msgs::msg::GraspConfig& msg);
    quasi_r2_msgs::msg::GraspConfigList::SharedPtr createGraspConfigListMsg(
         const std::vector<std::unique_ptr<gpd::candidate::Hand>> & hands,
         int16_t max_grasps,
         const std_msgs::msg::Header& header);

  /**
   * \brief Convert GPD Grasps into visual grasp messages.
   */
  visualization_msgs::msg::MarkerArray convertToVisualGraspMsg(
      const std::vector<std::unique_ptr<gpd::candidate::Hand>> & hands,
      double outer_diameter, double hand_depth, double finger_width, double hand_height,
      std_msgs::msg::Header header);

  /**
   * \brief Create finger marker for visual grasp messages
   */
  visualization_msgs::msg::Marker createFingerMarker(
      const Eigen::Vector3d & center, const std_msgs::msg::ColorRGBA& color,
      const Eigen::Matrix3d & frame, double length, double width, double height, int id,
      const std_msgs::msg::Header& header);

  /**
   * \brief Create hand base marker for visual grasp messages
   */
  visualization_msgs::msg::Marker createHandBaseMarker(
      const Eigen::Vector3d & start, const Eigen::Vector3d & end, const std_msgs::msg::ColorRGBA& color,
      const Eigen::Matrix3d & frame, double length, double height,
      int id,
      const std_msgs::msg::Header& header);

  void setMarkerColor(std_msgs::msg::ColorRGBA& color, const gpd::candidate::Hand& hand);

private:
    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    rclcpp::Logger logger_;
    grasp_ros2::GraspParameters grasp_params_;
    rclcpp::Publisher<quasi_r2_msgs::msg::GraspConfigList>::SharedPtr grasps_pub_;
    rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr grasps_rviz_pub_;
    rclcpp::Clock::SharedPtr clock_;
};
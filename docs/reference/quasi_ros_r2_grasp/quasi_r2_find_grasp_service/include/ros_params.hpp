// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#pragma once

#include <rclcpp/rclcpp.hpp>
#include <gpd/grasp_detector.h>

#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <trajectory_msgs/msg/joint_trajectory_point.hpp>

namespace grasp_ros2
{
  struct GraspParameters
  {
      /** minimum score expected for grasps returned from this service*/
      double grasp_score_threshold;
      /** frame id expected for grasps returned from this service*/
      std::string grasp_frame_id;
      /** approach direction in grasp_frame_id_ expected for grasps*/
      tf2::Vector3 grasp_approach;
      // /** maxmimum angle in radian acceptable between the expected 'approach_' and
      //* the real approach returned from this service*/
      double grasp_approach_angle;
      // /** offset [x, y, z] in metres applied to the grasps detected*/
      std::vector<double> grasp_offset;
      /** boundry cube in grasp_frame_id_ expected for grasps returned from this service*/
      std::vector<double> grasp_boundry;
      /** offset in metres from the gripper base (finger root) to the parent link of gripper*/
      double eef_offset;
      /** gripper yaw offset to its parent link, in radian (e.g. 0.0, or M_PI/4)*/
      double eef_yaw_offset;
      /** trajectory points in 'close' status, for joints in the same order as 'finger_joint_names_'*/
      trajectory_msgs::msg::JointTrajectoryPoint finger_points_close;
      /** trajectory points in 'open' status, for joints in the same order as 'finger_joint_names_'*/
      trajectory_msgs::msg::JointTrajectoryPoint finger_points_open;
      /** desired distance in metres for a grasp to approach and retreat*/
      double grasp_desired_distance;
      /** minimum distance in metres for a grasp to approach and retreat*/
      double grasp_min_distance;
      /** joint names of gripper fingers*/
      std::vector<std::string> finger_joint_names;
  };
/** ROSParameters class
 *
 * \brief A class to bridge parameters passed from ROS.
 *
*/
class ROSParameters
{
public:
  static void getDetectionParams(
    rclcpp::Node * node,
    gpd::GraspDetectionParameters & param);
    static void getGraspParams(rclcpp::Node * Node, GraspParameters & param);
};

}  // namespace grasp_ros2

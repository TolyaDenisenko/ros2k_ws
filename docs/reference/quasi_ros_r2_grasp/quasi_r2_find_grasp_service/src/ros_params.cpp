// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#include "ros_params.hpp"

namespace grasp_ros2
{

void ROSParameters::getDetectionParams(
  rclcpp::Node * node,
  gpd::GraspDetectionParameters & param)
{
  // Read hand geometry parameters.
  node->get_parameter_or("finger_width", param.hand_search_params.hand_geometry_.finger_width_, 0.01);
  node->get_parameter_or("hand_outer_diameter", param.hand_search_params.hand_geometry_.outer_diameter_, 0.12);
  node->get_parameter_or("hand_depth", param.hand_search_params.hand_geometry_.depth_, 0.06);
  node->get_parameter_or("hand_height", param.hand_search_params.hand_geometry_.height_, 0.02);
  node->get_parameter_or("init_bite", param.hand_search_params.hand_geometry_.init_bite_, 0.01);

  // Read local hand search parameters
  node->get_parameter_or("nn_radius", param.hand_search_params.nn_radius_frames_, 0.01);
  node->get_parameter_or("num_orientations", param.hand_search_params.num_orientations_, 8);
  node->get_parameter_or("num_samples", param.hand_search_params.num_samples_, 500);
  node->get_parameter_or("num_threads", param.hand_search_params.num_threads_, 4);
  node->get_parameter_or("friction_cone", param.hand_search_params.friction_coeff_, 20.0);
  node->get_parameter_or("min_viable", param.hand_search_params.min_viable_, 6);
  node->get_parameter_or("num_finger_placements", param.hand_search_params.num_finger_placements_, 10);

  std::vector<long> axes;
  node->get_parameter_or("rotation_axes", axes, {2l});
  std::copy(axes.begin(), axes.end(), std::back_inserter(param.hand_search_params.hand_axes_));
  param.generator_params.num_threads_ = param.hand_search_params.num_threads_;
  param.generator_params.num_samples_ = param.hand_search_params.num_samples_;

  node->get_parameter_or("deepen_hand", param.hand_search_params.deepen_hand_, true);

  // Read plotting parameters.
  node->get_parameter_or("plot_samples", param.plot_samples_, false);
  node->get_parameter_or("plot_normals", param.plot_normals_, false);
  node->get_parameter_or("plot_filtered_grasps", param.plot_filtered_grasps_, false);
  node->get_parameter_or("plot_valid_grasps", param.plot_valid_grasps_, false);
  node->get_parameter_or("plot_clusters", param.plot_clusters_, false);
  node->get_parameter_or("plot_selected_grasps", param.plot_selected_grasps_, false);

  // Read general parameters.
  node->get_parameter_or("plot_candidates", param.plot_candidates_, false);

  // Read preprocessing parameters.
  node->get_parameter_or("remove_outliers", param.generator_params.remove_statistical_outliers_, false);
  node->get_parameter_or("voxel_size", param.generator_params.voxel_size_, 0.003);
  node->get_parameter_or("refine_normals_k", param.generator_params.refine_normals_k_, 0);
  node->get_parameter_or("normals_radius", param.generator_params.normals_radius_, 0.030);
  node->get_parameter_or("voxelize", param.generator_params.voxelize_, true);
  node->get_parameter_or("sample_above_plane", param.generator_params.sample_above_plane_, false);
  node->get_parameter_or("workspace", param.generator_params.workspace_, {-1.0, 1.0, -1.0, 1.0, -1.0, 1.0});

  // Read classification parameters and create classifier.
  node->get_parameter_or("model_file", param.model_file_, std::string(""));
  node->get_parameter_or("trained_file", param.weights_file_, std::string(""));
  node->get_parameter_or("min_score_diff", param.min_score_diff_, 500.0);
  node->get_parameter_or("create_image_batches", param.create_image_batches_, false);
  node->get_parameter_or("device", param.device_, 0);

  // Read grasp image parameters.
  node->get_parameter_or("image_outer_diameter", param.image_params.outer_diameter_,
    param.hand_search_params.hand_geometry_.outer_diameter_);
  node->get_parameter_or("image_depth", param.image_params.depth_,
    param.hand_search_params.hand_geometry_.depth_);
  node->get_parameter_or("image_height", param.image_params.height_,
    param.hand_search_params.hand_geometry_.height_);

  node->get_parameter_or("image_size", param.image_params.size_, 60);
  node->get_parameter_or("image_num_channels", param.image_params.num_channels_, 15);

  // Read learning parameters.
  node->get_parameter_or("remove_plane_before_image_calculation", param.remove_plane_, false);

  // Read grasp filtering parameters
  node->get_parameter_or("filter_grasps", param.filter_grasps_, false);
  node->get_parameter_or("filter_half_antipodal", param.filter_half_antipodal_, false);

  // Read clustering parameters
  node->get_parameter_or("min_inliers", param.min_inliers_, 1);

  // Read grasp selection parameters
  node->get_parameter_or("num_selected", param.num_selected_, 100);
  node->get_parameter_or("min_aperture", param.min_aperture_, 0.0);
  node->get_parameter_or("max_aperture", param.max_aperture_, 0.085);
  param.workspace_grasps_ = param.generator_params.workspace_;

  node->get_parameter_or("filter_approach_direction", param.filter_approach_direction_, false);
  node->get_parameter_or("direction", param.direction_, {1, 0, 0});
  node->get_parameter_or("thresh_rad", param.thresh_rad_, 2.0);
  node->get_parameter_or("batch_size", param.batch_size_, 100);
}

void ROSParameters::getGraspParams(
  rclcpp::Node * node,
  GraspParameters & param)
{
     node->get_parameter_or("grasp_score_threshold", param.grasp_score_threshold, 200.0);
     node->get_parameter_or("grasp_frame_id", param.grasp_frame_id, std::string("base"));
     std::vector<double> approach;
     node->get_parameter_or("grasp_approach", approach,
     std::vector<double>(std::initializer_list<double>({0.0, 0.0, -1.0})));
     param.grasp_approach = tf2::Vector3(approach[0], approach[1], approach[2]);
     node->get_parameter_or("grasp_approach_angle", param.grasp_approach_angle, M_PI);
     node->get_parameter_or("grasp_offset", param.grasp_offset, std::vector<double>(std::initializer_list<double>({0.0, 0.0, 0.0})));
     node->get_parameter_or("grasp_boundry", param.grasp_boundry, std::vector<double>(std::initializer_list<double>({-1.0, 1.0, -1.0, 1.0, -1.0, 1.0})));
     node->get_parameter_or("eef_offset", param.eef_offset, 0.154);
     node->get_parameter_or("eef_yaw_offset", param.eef_yaw_offset, 0.0);
     node->get_parameter_or("grasp_min_distance", param.grasp_min_distance, 0.06);
     node->get_parameter_or("grasp_desired_distance", param.grasp_desired_distance, 0.1);
     //gripper parameters
     node->get_parameter_or("finger_joint_names", param.finger_joint_names,
     std::vector<std::string>(std::initializer_list<std::string>({std::string("panda_finger_joint1"),
     std::string("panda_finger_joint2")})));
     node->get_parameter_or("finger_positions_open", param.finger_points_open.positions, std::vector<double>(std::initializer_list<double>({-0.01, 0.01})));
     node->get_parameter_or("finger_positions_close", param.finger_points_close.positions, std::vector<double>(std::initializer_list<double>({-0.0, 0.0})));
}

}  // namespace grasp_ros2

// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#ifndef R2_WS_OBJECT_RECOGNITION_TEST_NODE_HPP
#define R2_WS_OBJECT_RECOGNITION_TEST_NODE_HPP

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_lifecycle/lifecycle_node.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <bondcpp/bond.hpp>
#include <quasi_utils/basic_action_server.hpp>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <object_msgs/msg/objects_in_boxes.hpp>
#include <quasi_r2_msgs/action/detect_object.hpp>
#include <quasi_r2_msgs/action/find_place_pose.hpp>
#include <geometric_shapes/shape_messages.h>
#include <geometric_shapes/bodies.h>
#include <pcl/visualization/cloud_viewer.h>
#include "quasi_r2_perception/object_segmentation.hpp"
#include <image_geometry/pinhole_camera_model.hpp>
#include <nlohmann/json.hpp>

namespace quasi {

  class PerceptionNode : public rclcpp_lifecycle::LifecycleNode {
  public:
    explicit PerceptionNode(const rclcpp::NodeOptions & options = rclcpp::NodeOptions()) :
        LifecycleNode("perception", options)
    {
      declare_parameter("use_bond", true);
      declare_parameter(bond::msg::Constants::DISABLE_HEARTBEAT_TIMEOUT_PARAM, true);
      set_parameter(rclcpp::Parameter(bond::msg::Constants::DISABLE_HEARTBEAT_TIMEOUT_PARAM, true));

      declare_parameter("frame_id", "base_footprint");
      declare_parameter("pointcloud_topic", "/realsense/depth/color/points");
      declare_parameter("color_camera_info_topic", "/realsense/color/camera_info");
      declare_parameter("object_detection_service", "/openvino_image_object_server/service");
      declare_parameter("default_workspace.width", 1.4);
      declare_parameter("default_workspace.length", 1.2);
      declare_parameter("default_workspace.height", 1.4);
      declare_parameter("default_workspace.origin_z", 0.0);
      declare_parameter("plane_distance_threshold", 0.02);
      declare_parameter("plane_angle_threshold", 0.15);
      declare_parameter("cloud_segmentation_timeout", -1);  // in milliseconds
      declare_parameter("area_height_threshold", 0.05);
      declare_parameter("object_types_file_path", "");

      declare_parameter("reduce_roi", rclcpp::ParameterType::PARAMETER_INTEGER);
      declare_parameter("object_recognition_timeout", -1);  // in milliseconds
      declare_parameter("min_roi_intersection", 0.8);
      declare_parameter("roi_intersection_search_radius", 0.002);
      declare_parameter("primitive_size_padding", 0.002);

      declare_parameter("gripper_outer_radius", 0.11);
      declare_parameter("max_place_poses", 10);
      declare_parameter("num_samples", 100);
      declare_parameter("z_padding", 0.003);

      declare_parameter<std::string>("save_object_clouds", "");

      if(declare_parameter("use_cloud_viewer", false)) {
        RCLCPP_INFO(get_logger(), "Cloud viewer enabled");
        cloud_viewer_ = pcl::make_shared<pcl::visualization::CloudViewer>("Simple Cloud Viewer");
      }
    }

    CallbackReturn on_configure(const rclcpp_lifecycle::State &previous_state) override;
    CallbackReturn on_activate(const rclcpp_lifecycle::State &previous_state) override;
    CallbackReturn on_deactivate(const rclcpp_lifecycle::State &previous_state) override;
    CallbackReturn on_cleanup(const rclcpp_lifecycle::State &/*previous_state*/) override;
    CallbackReturn on_shutdown(const rclcpp_lifecycle::State & previous_state) override;

  protected:
    void execute_detect_object_goal(const std::shared_ptr<rclcpp_action::ServerGoalHandle<quasi_r2_msgs::action::DetectObject>> goal_handle);
    void execute_find_place_pose_goal(const std::shared_ptr<rclcpp_action::ServerGoalHandle<quasi_r2_msgs::action::FindPlacePose>> goal_handle);


    std::shared_ptr<std::vector<quasi_r2_msgs::msg::Object>> detect_object_on_pointcloud_message(std::vector<sensor_msgs::msg::PointCloud2::ConstSharedPtr> msgs,
                                                                                                 sensor_msgs::msg::CameraInfo::ConstSharedPtr color_camera_info_msg,
                                                                                                 const object_msgs::msg::ObjectsInBoxes& recognized_objects,
                                                                                                 const vision_msgs::msg::BoundingBox3DArray& workspace,
                                                                                                 float area_height,
                                                                                                 uint8_t shape_type);

    std::shared_ptr<std::vector<geometry_msgs::msg::PoseStamped>> find_place_pose_on_pointcloud_message(sensor_msgs::msg::PointCloud2::SharedPtr msg,
                                                                                                        const quasi_r2_msgs::action::FindPlacePose::Goal::ConstSharedPtr& goal);

    std::shared_ptr<bodies::Body> extract_roi_body(const sensor_msgs::msg::RegionOfInterest& roi, const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud);
    std::vector<std::shared_ptr<ObjectSegmentation::SegmentedObject>> intersect_segmented_objects_with_roi(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud_transformed,
                                                                                                           const sensor_msgs::msg::RegionOfInterest& roi,
                                                                                                           const std::vector<std::shared_ptr<ObjectSegmentation::SegmentedObject>>& segmented_objects);
    std::vector<std::shared_ptr<ObjectSegmentation::SegmentedObject>> intersect_segmented_objects_with_roi2(const image_geometry::PinholeCameraModel& color_camera_model,
                                                                                                            const geometry_msgs::msg::TransformStamped& transform,
                                                                                                            const sensor_msgs::msg::RegionOfInterest& roi,
                                                                                                            const std::vector<std::shared_ptr<ObjectSegmentation::SegmentedObject>>& segmented_objects);

    static pcl::PointCloud<pcl::PointXYZRGB>::Ptr project_onto_plane(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& input, const pcl::ModelCoefficients::Ptr& plane, Eigen::Matrix3f& inv_plane_rotation);
    static bool extract_primitive_shape(uint8_t shape_type,
                                        const geometry_msgs::msg::Pose& mesh_pose, const shape_msgs::msg::Mesh& mesh,
                                        geometry_msgs::msg::Pose& pose, shape_msgs::msg::SolidPrimitive& shape);

    static bool extract_primitive_shape_bc(uint8_t shape_type,
                                           const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& input,
                                           const pcl::ModelCoefficients::Ptr& plane,
                                           shape_msgs::msg::SolidPrimitive& shape,
                                           geometry_msgs::msg::Pose& pose);
    void padd_primitive(shape_msgs::msg::SolidPrimitive& solid_primitive);
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr filter_cloud_by_workspace(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_transformed, const vision_msgs::msg::BoundingBox3DArray& workspace);

    static uint8_t parse_object_shape(const std::string& shape_type);
    static std::string object_shape_name(uint8_t shape_type);

    // Connection to tell Lifecycle Manager that server is still up
    std::unique_ptr<bond::Bond> bond_{nullptr};

    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;

    pcl::visualization::CloudViewer::Ptr cloud_viewer_;

    std::shared_ptr<ObjectSegmentation> object_segmentation_;

    std::shared_ptr<BasicActionServer<quasi_r2_msgs::action::DetectObject>> detect_object_action_server_;
    std::shared_ptr<BasicActionServer<quasi_r2_msgs::action::FindPlacePose>> find_place_pose_action_server_;

    nlohmann::json object_types_config_;
  };
}

#endif //R2_WS_OBJECT_RECOGNITION_TEST_NODE_HPP

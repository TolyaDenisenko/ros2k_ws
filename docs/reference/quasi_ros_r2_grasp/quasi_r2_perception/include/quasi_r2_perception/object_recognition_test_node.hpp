// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#ifndef R2_WS_OBJECT_RECOGNITION_TEST_NODE_HPP
#define R2_WS_OBJECT_RECOGNITION_TEST_NODE_HPP

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>
#include <object_msgs/msg/objects_in_boxes.hpp>
#include "quasi_r2_perception/object_recognition.hpp"

namespace quasi {
  class ObjectRecognitionTestNode : public rclcpp::Node {
  public:
    explicit ObjectRecognitionTestNode(const rclcpp::NodeOptions & options = rclcpp::NodeOptions()) :
        Node("object_recognition_test", options)
    {
      declare_parameter("reduce_roi", rclcpp::ParameterType::PARAMETER_INTEGER);
      declare_parameter("algorithm", "hough");
      get_parameter("algorithm", algorithm_);

      cloud_viewer_ = pcl::make_shared<pcl::visualization::CloudViewer>("Simple Cloud Viewer");
    }

    bool init();

    void on_pointcloud_message(sensor_msgs::msg::PointCloud2::SharedPtr msg);
    void on_object_detected_message(object_msgs::msg::ObjectsInBoxes::SharedPtr msg);

    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr pcl_subscription_;
    pcl::visualization::CloudViewer::Ptr cloud_viewer_;

    rclcpp::Subscription<object_msgs::msg::ObjectsInBoxes>::SharedPtr objects_subscription_;
    std::string algorithm_;
    std::shared_ptr<ObjectRecognition> object_recognition_;

    std::vector<object_msgs::msg::ObjectInBox> objects_;
  };
}

#endif //R2_WS_OBJECT_RECOGNITION_TEST_NODE_HPP

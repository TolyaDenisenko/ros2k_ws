// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#ifndef R2_WS_OBJECT_RECOGNIZER_HPP
#define R2_WS_OBJECT_RECOGNIZER_HPP

#include <rclcpp/rclcpp.hpp>
#include <rclcpp/node.hpp>
#include <pcl/point_types.h>
#include <pcl/pcl_base.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <pcl/visualization/cloud_viewer.h>

namespace quasi {
  class ObjectRecognition {
  public:
    template<class NodeT>
    explicit ObjectRecognition(const std::shared_ptr<NodeT>& node) :
      node_logging_(node->get_node_logging_interface()), node_parameters_(node->get_node_parameters_interface()) {}
    virtual ~ObjectRecognition() = default;

    virtual bool init(const std::string& ) { return true; };
    virtual bool recognize(const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr& cloud_cropped, const pcl::PointCloud<pcl::PointNormal>::ConstPtr& cloud_normals,
                           geometry_msgs::msg::TransformStamped camera_to_map_transform,
                           const pcl::visualization::CloudViewer::Ptr& viewer) = 0;

  protected:
    std::shared_ptr<rclcpp::node_interfaces::NodeLoggingInterface> node_logging_;
    std::shared_ptr<rclcpp::node_interfaces::NodeParametersInterface> node_parameters_;
  };
}

#endif //R2_WS_OBJECT_RECOGNIZER_HPP

// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.
#ifndef R2_PERCEPTION_OBJECT_SEGMENTATION_HPP
#define R2_PERCEPTION_OBJECT_SEGMENTATION_HPP

#include "quasi_r2_perception/object_recognition.hpp"
#include <pcl/point_types.h>
#include <pcl/pcl_base.h>

namespace quasi {
  class ObjectSegmentation : public ObjectRecognition {
  public:
    template<class NodeT>
    explicit ObjectSegmentation(const std::shared_ptr<NodeT>& node) : ObjectRecognition(node) {
      if(!node->has_parameter("cluster_tolerance"))
        node->template declare_parameter<double>("cluster_tolerance", 0.02);
      if(!node->has_parameter("cluster_min_size"))
        node->template declare_parameter<int>("cluster_min_size", 50);
      if(!node->has_parameter("plane_distance_threshold"))
        node->template declare_parameter<double>("plane_distance_threshold", 0.02);
      if(!node->has_parameter("plane_angle_threshold"))
        node->template declare_parameter<double>("plane_angle_threshold", 0.15);
      if(!node->has_parameter("plane_points_threshold"))
        node->template declare_parameter<int64_t>("plane_points_threshold", 9000);

      if(!node->has_parameter("show_box"))
        node->template declare_parameter<bool>("show_box", false);

      node->get_parameter("cluster_tolerance", cluster_tolerance_);
      node->get_parameter("cluster_min_size", cluster_min_size_);
      node->get_parameter("plane_distance_threshold", plane_distance_threshold_);
      node->get_parameter("plane_angle_threshold", plane_angle_threshold_);
      node->get_parameter("plane_points_threshold", plane_points_threshold_);
    }

    bool init(const std::string& model_file) override;
    bool recognize(const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr& cloud_cropped, const pcl::PointCloud<pcl::PointNormal>::ConstPtr& cloud_normals,
                   geometry_msgs::msg::TransformStamped camera_to_map_transform,
                   const pcl::visualization::CloudViewer::Ptr& viewer) override;

    struct SegmentedObject {
      pcl::PointCloud<pcl::PointXYZRGB>::Ptr cluster_cloud {new pcl::PointCloud<pcl::PointXYZRGB>};
      pcl::PointCloud<pcl::PointNormal>::Ptr cluster_normals {new pcl::PointCloud<pcl::PointNormal>};
      Eigen::Vector4f centroid;
      pcl::ModelCoefficients::Ptr plane_coefficients;
    };

    [[nodiscard]] const std::vector<std::shared_ptr<SegmentedObject>>& get_segmented_objects() const { return segmented_objects_; }

  protected:
    double cluster_tolerance_;
    int cluster_min_size_;
    double plane_distance_threshold_;
    int64_t plane_points_threshold_;
    double plane_angle_threshold_;

    std::vector<std::shared_ptr<SegmentedObject>> segmented_objects_;
  };
}


#endif

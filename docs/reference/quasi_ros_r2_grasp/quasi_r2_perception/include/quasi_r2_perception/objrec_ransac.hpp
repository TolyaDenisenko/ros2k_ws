// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.
#ifndef R2_PERCEPTION_RANSAC_HPP
#define R2_PERCEPTION_RANSAC_HPP

#include "quasi_r2_perception/object_recognition.hpp"
#include <pcl/point_types.h>
#include <pcl/pcl_base.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/visualization/cloud_viewer.h>
#include "pcl/filters/voxel_grid.h"
#include "pcl/filters/extract_indices.h"
#include "pcl/segmentation/extract_clusters.h"
#include "pcl/segmentation/sac_segmentation.h"
#include "pcl/segmentation/crf_normal_segmentation.h"
#include <pcl/features/normal_3d_omp.h>
#include <pcl/features/shot_omp.h>
#include <pcl/io/ply_io.h>
#include <pcl/filters/uniform_sampling.h>
#include <pcl/features/board.h>
#include <pcl/console/parse.h>
#include <geometry_msgs/msg/transform_stamped.hpp>

namespace quasi {
  class RANSACRecognition : public ObjectRecognition {
  public:
    template<class NodeT>
    explicit RANSACRecognition(const std::shared_ptr<NodeT>& node) : ObjectRecognition(node) {
      node->template declare_parameter<float>("max_coplanary_angle", 3.0);
      node->template declare_parameter<float>("pair_width", 0.01);
      node->template declare_parameter<float>("voxel_size", 0.003);
      node->template declare_parameter<double>("success_probability_", 0.99);

      node->get_parameter("max_coplanary_angle", max_coplanary_angle_);
      node->get_parameter("pair_width", pair_width_);
      node->get_parameter("voxel_size", voxel_size_);
      node->get_parameter("success_probability", success_probability_);
    }

    bool init(const std::string& model_file) override;
    bool recognize(const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr& cloud_cropped, const pcl::PointCloud<pcl::PointNormal>::ConstPtr& cloud_normals,
                   geometry_msgs::msg::TransformStamped camera_to_map_transform,
                   const pcl::visualization::CloudViewer::Ptr& viewer) override;

  protected:

    pcl::PointCloud<pcl::PointXYZ>::Ptr model_cloud_;
    pcl::PointCloud<pcl::Normal>::Ptr model_normals_;
    //pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr model_keypoints_;

    float max_coplanary_angle_;
    float pair_width_;
    float voxel_size_;
    double success_probability_;
  };
}


#endif

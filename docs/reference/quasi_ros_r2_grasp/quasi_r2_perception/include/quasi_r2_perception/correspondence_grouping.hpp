// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.
#ifndef R2_PERCEPTION_OBJECT_GROUPING_HPP
#define R2_PERCEPTION_OBJECT_GROUPING_HPP

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
#include <pcl/recognition/cg/hough_3d.h>
#include <pcl/recognition/cg/geometric_consistency.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/features/board.h>
#include <pcl/console/parse.h>
#include <geometry_msgs/msg/transform_stamped.hpp>

namespace quasi {
  class CorrespondenceGroupingRecognition : public ObjectRecognition {
  public:
    template <class NodeT>
    CorrespondenceGroupingRecognition(const std::shared_ptr<NodeT>& node, const std::string& algorithm) :
        ObjectRecognition(node), algorithm_(algorithm)
    {
      node->template declare_parameter<double>("model_ss", 0.003);
      node->template declare_parameter<double>("scene_ss", 0.004);
      node->template declare_parameter<double>("descr_rad", 0.033);
      node->template declare_parameter<double>("rf_rad", 0.02);
      node->template declare_parameter<double>("cg_size", 0.1);
      node->template declare_parameter<double>("cg_thresh", 30.0);
      node->template declare_parameter<int>("icp_max_iter_", 5);
      node->template declare_parameter<double>("icp_corr_distance", 0.005);

      node->get_parameter("model_ss", model_ss_);
      node->get_parameter("scene_ss", scene_ss_);
      node->get_parameter("descr_rad", descr_rad_);
      node->get_parameter("rf_rad", rf_rad_);
      node->get_parameter("cg_size", cg_size_);
      node->get_parameter("cg_thresh", cg_thresh_);
      node->get_parameter("icp_max_iter", icp_max_iter_);
      node->get_parameter("icp_corr_distance", icp_corr_distance_);
    }

    bool init(const std::string& model_file) override;
    bool recognize(const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr& cloud_cropped, const pcl::PointCloud<pcl::PointNormal>::ConstPtr& cloud_normals,
                   geometry_msgs::msg::TransformStamped camera_to_map_transform,
                   const pcl::visualization::CloudViewer::Ptr& viewer) override;

  protected:

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr model_cloud_;
    pcl::PointCloud<pcl::PointNormal>::Ptr model_normals_;
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr model_keypoints_;
    pcl::PointCloud<pcl::SHOT352>::Ptr model_descriptors_;
    pcl::PointCloud<pcl::ReferenceFrame>::Ptr model_rf_;

    std::string algorithm_;
    double model_ss_;
    double scene_ss_;
    double descr_rad_;
    double rf_rad_;
    double cg_size_;
    double cg_thresh_;
    int icp_max_iter_;
    double icp_corr_distance_;
  };
}


#endif

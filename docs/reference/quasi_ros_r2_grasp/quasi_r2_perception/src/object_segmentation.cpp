// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#include "quasi_r2_perception/object_segmentation.hpp"
#include <pcl/visualization/cloud_viewer.h>
#include <pcl_conversions/pcl_conversions.h>
#include "pcl/filters/extract_indices.h"
#include "pcl/segmentation/extract_clusters.h"
#include "pcl/segmentation/sac_segmentation.h"
#include "pcl/segmentation/crf_normal_segmentation.h"
#include <pcl/features/normal_3d_omp.h>
#include <pcl/features/board.h>
#include <pcl/console/parse.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <geometric_shapes/shapes.h>
#if __has_include(<tf2_eigen/tf2_eigen.hpp>)
#include <tf2_eigen/tf2_eigen.hpp>
#else
#include <tf2_eigen/tf2_eigen.h>
#endif

using namespace std::string_literals;

namespace quasi {
  bool ObjectSegmentation::init(const std::string& ) {

    return true;
  }



  bool ObjectSegmentation::recognize(const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr& cloud_cropped, const pcl::PointCloud<pcl::PointNormal>::ConstPtr& cloud_normals,
                                     geometry_msgs::msg::TransformStamped /*camera_to_map_transform*/,
                                     const pcl::visualization::CloudViewer::Ptr& cloud_viewer) {
    segmented_objects_.clear();

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZRGB>(*cloud_cropped));
    pcl::PointCloud<pcl::PointNormal>::Ptr cloud_normals_filtered(new pcl::PointCloud<pcl::PointNormal>(*cloud_normals));
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr non_horizontal_planes = pcl::make_shared<pcl::PointCloud<pcl::PointXYZRGB>>();
    pcl::PointCloud<pcl::PointNormal>::Ptr non_horizontal_planes_normals = pcl::make_shared<pcl::PointCloud<pcl::PointNormal>>();
    std::vector<pcl::ModelCoefficients::Ptr> plane_coefficients;
    while(cloud_filtered->size() > 200) {
      // Segment the largest planar components from the remaining cloud
      pcl::ModelCoefficients::Ptr coefs(new pcl::ModelCoefficients);
      pcl::PointIndices::Ptr plane_indices(new pcl::PointIndices);
      pcl::SACSegmentation<pcl::PointXYZRGB> segment;
      segment.setOptimizeCoefficients(true);
      segment.setMethodType(pcl::SAC_RANSAC);
      segment.setModelType(pcl::SACMODEL_PLANE); //pcl::SACMODEL_NORMAL_PLANE);
      segment.setMaxIterations(100);
      segment.setDistanceThreshold(plane_distance_threshold_);
      //segment.setNormalDistanceWeight(0.2);
      segment.setInputCloud(cloud_filtered);
      //segment.setInputNormals(cloud_normals_filtered);
      segment.segment(*plane_indices, *coefs);

      double plane_height = -coefs->values[3] / coefs->values[2];
      Eigen::Vector3f plane_normal(coefs->values[0], coefs->values[1], coefs->values[2]);
      float angle = acos(Eigen::Vector3f::UnitZ().dot(plane_normal));

      RCLCPP_DEBUG(node_logging_->get_logger(), "Found plane with %zu points at height %f, angle %f. Threshold: %jd",
                                             plane_indices->indices.size(), plane_height, angle, plane_points_threshold_);
      if(plane_indices->indices.size() < (size_t)plane_points_threshold_)
        break;

      // Check plane is mostly horizontal
      if (angle >= plane_angle_threshold_) {
        RCLCPP_DEBUG(node_logging_->get_logger(), "... plane is non-horizontal: %f, threshold: %f", angle, plane_angle_threshold_);

        pcl::PointCloud<pcl::PointXYZRGB>::Ptr non_horizontal_plane = pcl::make_shared<pcl::PointCloud<pcl::PointXYZRGB>>();
        pcl::PointCloud<pcl::PointNormal>::Ptr non_horizontal_plane_normals = pcl::make_shared<pcl::PointCloud<pcl::PointNormal>>();
        pcl::ExtractIndices<pcl::PointXYZRGB> extract;
        extract.setInputCloud(cloud_filtered);
        extract.setIndices(plane_indices);
        extract.setNegative(false);
        extract.filter(*non_horizontal_plane);

        pcl::ExtractIndices<pcl::PointNormal> extract_normals;
        extract_normals.setInputCloud(cloud_normals_filtered);
        extract_normals.setIndices(plane_indices);
        extract_normals.setNegative(false);
        extract_normals.filter(*non_horizontal_plane_normals);

        *non_horizontal_planes += *non_horizontal_plane;
        *non_horizontal_planes_normals += *non_horizontal_plane_normals;
      }
      else
        plane_coefficients.push_back(coefs);

        // Remove plane from cropped and normals clouds
      pcl::ExtractIndices<pcl::PointXYZRGB> extract;
      extract.setInputCloud(cloud_filtered);
      extract.setIndices(plane_indices);
      extract.setNegative(true);
      extract.filter(*cloud_filtered);

      pcl::ExtractIndices<pcl::PointNormal> extract_normals;
      extract_normals.setInputCloud(cloud_normals_filtered);
      extract_normals.setIndices(plane_indices);
      extract_normals.setNegative(true);
      extract_normals.filter(*cloud_normals_filtered);
    }
    RCLCPP_DEBUG(node_logging_->get_logger(), "Found and removed %zu planes", plane_coefficients.size());

    // add back non-horizontal planes
    *cloud_filtered += *non_horizontal_planes;
    *cloud_normals_filtered += *non_horizontal_planes_normals;

    if(cloud_viewer)
      cloud_viewer->runOnVisualizationThreadOnce([=] (pcl::visualization::PCLVisualizer& pcl_visualizer) {
        pcl_visualizer.removeAllShapes();
        pcl_visualizer.removeAllPointClouds();
        pcl_visualizer.addPointCloud<pcl::PointXYZRGB>(cloud_filtered, "cloud_filtered");
        pcl_visualizer.addPointCloudNormals<pcl::PointXYZRGB, pcl::PointNormal>(cloud_filtered, cloud_normals_filtered, 100, 0.02f, "cloud_normals");
      });

    std::vector<pcl::PointIndices> clusters;
    pcl::EuclideanClusterExtraction<pcl::PointXYZRGB> extract_clusters;
    extract_clusters.setClusterTolerance(cluster_tolerance_);
    extract_clusters.setMinClusterSize(cluster_min_size_);
    extract_clusters.setInputCloud(cloud_filtered);
    extract_clusters.extract(clusters);
    RCLCPP_DEBUG(node_logging_->get_logger(), "Extracted %d clusters.", static_cast<int>(clusters.size()));
    if(clusters.empty())
      return false;

    pcl::ExtractIndices<pcl::PointXYZRGB> extract_indices;
    pcl::ExtractIndices<pcl::PointNormal> extract_normals;
    extract_indices.setInputCloud(cloud_filtered);
    extract_normals.setInputCloud(cloud_normals_filtered);
    for (size_t i = 0; i < clusters.size(); ++i) {
      std::shared_ptr<SegmentedObject> segmented_object = std::make_shared<SegmentedObject>();
      // Extract object
      extract_indices.setIndices(pcl::PointIndicesPtr(new pcl::PointIndices(clusters[i])));
      extract_indices.filter(*segmented_object->cluster_cloud);

      // Extract normals for object
      extract_normals.setIndices(extract_indices.getIndices());
      extract_normals.filter(*segmented_object->cluster_normals);

      // Find centroid
      pcl::compute3DCentroid(*segmented_object->cluster_cloud, segmented_object->centroid);

      // Find the closest support plane
      int64_t support_plane_index = -1;
      double min_support_plane_distance = std::numeric_limits<double>::max();
      for (size_t p = 0; p < plane_coefficients.size(); ++p)
      {
        Eigen::Vector4f pp(segmented_object->centroid);
        pp[3] = 1.0;
        const auto& plane = *plane_coefficients[p];
        Eigen::Vector4f m(plane.values[0], plane.values[1], plane.values[2], plane.values[3]);
        double distance = pp.dot(m);
        if (distance > 0.0 && distance < min_support_plane_distance) {
          min_support_plane_distance = distance;
          support_plane_index = (int64_t)p;
        }
      }
      if(support_plane_index != -1) {
        RCLCPP_DEBUG(node_logging_->get_logger(), "segmented object %zu is supported by plane: %ld", i, support_plane_index);
        segmented_object->plane_coefficients = plane_coefficients[support_plane_index];
      }

      segmented_objects_.push_back(segmented_object);

      if(cloud_viewer)
        cloud_viewer->runOnVisualizationThreadOnce([=] (pcl::visualization::PCLVisualizer& pcl_visualizer) {
          std::string cluster_name = "cluster_"s + std::to_string(i);
          pcl_visualizer.addPointCloud(segmented_object->cluster_cloud, cluster_name);
          pcl_visualizer.addSphere(pcl::PointXYZ(segmented_object->centroid.x(), segmented_object->centroid.y(), segmented_object->centroid.z()), 0.002, 255, 0, 255, cluster_name + "_centroid");
          pcl_visualizer.addText3D(cluster_name + "_name", pcl::PointXYZ(segmented_object->centroid.x(), segmented_object->centroid.y(), segmented_object->centroid.z()), 0.025, 1.0, 1.0, 1.0, "", 0);
        });

    }

    return true;
 }


}

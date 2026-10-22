// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#include "quasi_r2_perception/objrec_ransac.hpp"
#include <pcl/visualization/cloud_viewer.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl_ros/transforms.hpp>
#include <pcl/recognition/ransac_based/obj_rec_ransac.h>

namespace quasi {
  bool RANSACRecognition::init(const std::string& model_file) {
    pcl::PointCloud<pcl::PointXYZRGBNormal> model_cloud;
    int result = pcl::io::loadPCDFile(model_file, model_cloud);
    if(result) {
      RCLCPP_ERROR(node_logging_->get_logger(), "error loading model file: %d", result);
      return false;
    }
    RCLCPP_INFO(node_logging_->get_logger(), "loaded model file: %d", result);

    model_cloud_.reset(new pcl::PointCloud<pcl::PointXYZ>());
    model_normals_.reset(new pcl::PointCloud<pcl::Normal>);
    pcl::copyPointCloud(model_cloud, *model_cloud_);
    pcl::copyPointCloud(model_cloud, *model_normals_);

//    // Downsample and Extract Keypoints
//    model_keypoints_.reset(new pcl::PointCloud<pcl::PointXYZRGBNormal>());
//    pcl::UniformSampling<pcl::PointXYZRGBNormal> uniform_sampling;
//    uniform_sampling.setInputCloud(model_cloud_);
//    uniform_sampling.setRadiusSearch(model_ss_);
//    uniform_sampling.filter (*model_keypoints_);

    std::cout << "Model total points: " << model_cloud_->size () << std::endl;

    return true;
  }



  bool RANSACRecognition::recognize(const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr& cloud_cropped, const pcl::PointCloud<pcl::PointNormal>::ConstPtr& cloud_normals,
                                    geometry_msgs::msg::TransformStamped /*transform*/,
                                    const pcl::visualization::CloudViewer::Ptr& cloud_viewer) {
//    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_voxelized(new pcl::PointCloud<pcl::PointXYZRGB>);
//    pcl::UniformSampling<pcl::PointXYZRGB> sampling;
//    float leaf_size = scene_ss_;
//    //sampling.setLeafSize(leaf_size, leaf_size, leaf_size);
//    sampling.setRadiusSearch(leaf_size);
//    sampling.setInputCloud(cloud_cropped);
//    sampling.filter(*cloud_voxelized);

//    // Segment the largest planar component from the remaining cloud
//    pcl::PointIndices::Ptr plane_indices(new pcl::PointIndices);
//    pcl::ModelCoefficients::Ptr plane_coefficients(new pcl::ModelCoefficients);
//    pcl::SACSegmentation<pcl::PointXYZRGB> segment;
//    segment.setOptimizeCoefficients(true);
//    segment.setMethodType(pcl::SAC_RANSAC);
//    segment.setModelType(pcl::SACMODEL_PLANE);
//    segment.setMaxIterations(100);
//    segment.setDistanceThreshold(0.025);
//    //segment.setNormalDistanceWeight(0.01);
//    //segment.setInputNormals(cloud_normals);
//    segment.setInputCloud(cloud_voxelized);
//    segment.segment(*plane_indices, *plane_coefficients);
//    RCLCPP_DEBUG_STREAM(node_->get_logger(), "Extracted plane: " << *plane_coefficients);

//    // Remove plane from cropped and normals clouds
//    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZRGB>);
//    pcl::ExtractIndices<pcl::PointXYZRGB> extract;
//    extract.setInputCloud(cloud_voxelized);
//    extract.setIndices(plane_indices);
//    extract.setNegative(true);
//    extract.filter(*cloud_filtered);

//  pcl::ExtractIndices<pcl::PointNormal> extract_normals;
//  extract_normals.setInputCloud(cloud_normals);
//  extract_normals.setIndices(plane_indices);
//  extract_normals.setNegative(true);
//  extract_normals.filter(*cloud_normals);

    pcl::recognition::ObjRecRANSAC objrec_ransac(pair_width_, voxel_size_);
    objrec_ransac.ignoreCoplanarPointPairsOn();
    objrec_ransac.setMaxCoplanarityAngleDegrees(max_coplanary_angle_);
    objrec_ransac.addModel(*model_cloud_, *model_normals_, "model");

    pcl::PointCloud<pcl::PointXYZ> cloud_xyz;
    pcl::PointCloud<pcl::Normal> cloud_normal;
    pcl::copyPointCloud(*cloud_cropped, cloud_xyz);
    pcl::copyPointCloud(*cloud_normals, cloud_normal);

    std::list<pcl::recognition::ObjRecRANSAC::Output> results;
    objrec_ransac.recognize(cloud_xyz, cloud_normal, results, success_probability_);

    std::cout << "Model instances found: " << results.size () << std::endl;
    for(auto result : results)
    {
      std::cout << "\n    Object " << result.object_name_ << ": " << result.match_confidence_ << std::endl;
    }

    pcl::PointCloud<pcl::PointXYZ>::Ptr octree_points(new pcl::PointCloud<pcl::PointXYZ> ());
    objrec_ransac.getSceneOctree().getFullLeavesPoints(*octree_points);

    if(cloud_viewer)
      cloud_viewer->runOnVisualizationThreadOnce([=] (pcl::visualization::PCLVisualizer& viewer) {
        viewer.removeAllPointClouds();
        viewer.removeAllShapes();

        viewer.addPointCloud(cloud_cropped);
        viewer.addPointCloudNormals<pcl::PointXYZRGB, pcl::PointNormal>(cloud_cropped, cloud_normals, 100, 0.02f, "cloud_normals");

        viewer.addPointCloud(octree_points, "octree points");
    //  viewer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "octree points");
        viewer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_COLOR, 1.0, 0.0, 0.0, "octree points");

        //viewer.addPointCloud (plane_points, "plane points");
        //viewer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_COLOR, 0.9, 0.9, 0.9, "plane points");

      });

    return true;
 }


}
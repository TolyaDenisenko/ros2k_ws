// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#include "quasi_r2_perception/correspondence_grouping.hpp"
#include <pcl/visualization/cloud_viewer.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl_ros/transforms.hpp>
#include <pcl/registration/icp.h>

namespace quasi {

  bool CorrespondenceGroupingRecognition::init(const std::string& model_file) {
    pcl::PointCloud<pcl::PointXYZRGBNormal> model_cloud;
    int result = pcl::io::loadPCDFile(model_file, model_cloud);
    if(result) {
      RCLCPP_ERROR(node_logging_->get_logger(), "error loading model file: %d", result);
      return false;
    }
    RCLCPP_INFO(node_logging_->get_logger(), "loaded model file: %d", result);

    model_cloud_.reset(new pcl::PointCloud<pcl::PointXYZRGB>());
    model_normals_.reset(new pcl::PointCloud<pcl::PointNormal>);
    pcl::copyPointCloud(model_cloud, *model_cloud_);
    pcl::copyPointCloud(model_cloud, *model_normals_);

    // Downsample and Extract Keypoints
    model_keypoints_.reset(new pcl::PointCloud<pcl::PointXYZRGB>());
    pcl::UniformSampling<pcl::PointXYZRGB> uniform_sampling;
    uniform_sampling.setInputCloud(model_cloud_);
    uniform_sampling.setRadiusSearch(model_ss_);
    uniform_sampling.filter (*model_keypoints_);

    model_descriptors_.reset(new pcl::PointCloud<pcl::SHOT352>);
    pcl::SHOTEstimationOMP<pcl::PointXYZRGB, pcl::PointNormal, pcl::SHOT352> descr_est;
    descr_est.setRadiusSearch (descr_rad_);
    descr_est.setInputCloud (model_keypoints_);
    descr_est.setInputNormals (model_normals_);
    descr_est.setSearchSurface (model_cloud_);
    descr_est.compute (*model_descriptors_);

    model_rf_.reset(new pcl::PointCloud<pcl::ReferenceFrame> ());

    pcl::BOARDLocalReferenceFrameEstimation<pcl::PointXYZRGB, pcl::PointNormal, pcl::ReferenceFrame> rf_est_model;
    rf_est_model.setFindHoles(true);
    rf_est_model.setRadiusSearch(rf_rad_);
    rf_est_model.setInputCloud(model_keypoints_);
    rf_est_model.setInputNormals(model_normals_);
    rf_est_model.setSearchSurface(model_cloud_);
    rf_est_model.compute(*model_rf_);

    std::cout << "Model total points: " << model_cloud_->size () << "; Selected Keypoints: " << model_keypoints_->size () << "; Descriptors: " << model_descriptors_->size() << std::endl;

    return true;
  }



  bool CorrespondenceGroupingRecognition::recognize(const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr& cloud_cropped, const pcl::PointCloud<pcl::PointNormal>::ConstPtr& cloud_normals,
                                                    geometry_msgs::msg::TransformStamped transform,
                                                    const pcl::visualization::CloudViewer::Ptr& cloud_viewer) {
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_voxelized(new pcl::PointCloud<pcl::PointXYZRGB>);
    pcl::UniformSampling<pcl::PointXYZRGB> sampling;
    float leaf_size = scene_ss_;
    //sampling.setLeafSize(leaf_size, leaf_size, leaf_size);
    sampling.setRadiusSearch(leaf_size);
    sampling.setInputCloud(cloud_cropped);
    sampling.filter(*cloud_voxelized);

    // Segment the largest planar component from the remaining cloud
    pcl::PointIndices::Ptr plane_indices(new pcl::PointIndices);
    pcl::ModelCoefficients::Ptr plane_coefficients(new pcl::ModelCoefficients);
    pcl::SACSegmentation<pcl::PointXYZRGB> segment;
    segment.setOptimizeCoefficients(true);
    segment.setMethodType(pcl::SAC_RANSAC);
    segment.setModelType(pcl::SACMODEL_PLANE);
    segment.setMaxIterations(100);
    segment.setDistanceThreshold(0.025);
    //segment.setNormalDistanceWeight(0.01);
    //segment.setInputNormals(cloud_normals);
    segment.setInputCloud(cloud_voxelized);
    segment.segment(*plane_indices, *plane_coefficients);
    RCLCPP_DEBUG_STREAM(node_logging_->get_logger(), "Extracted plane: " << *plane_coefficients);

    // Remove plane from cropped and normals clouds
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZRGB>);
    pcl::ExtractIndices<pcl::PointXYZRGB> extract;
    extract.setInputCloud(cloud_voxelized);
    extract.setIndices(plane_indices);
    extract.setNegative(true);
    extract.filter(*cloud_filtered);

//  pcl::ExtractIndices<pcl::PointNormal> extract_normals;
//  extract_normals.setInputCloud(cloud_normals);
//  extract_normals.setIndices(plane_indices);
//  extract_normals.setNegative(true);
//  extract_normals.filter(*cloud_normals);

    pcl::PointCloud<pcl::SHOT352>::Ptr scene_descriptors(new pcl::PointCloud<pcl::SHOT352>);
    pcl::SHOTEstimationOMP<pcl::PointXYZRGB, pcl::PointNormal, pcl::SHOT352> descr_est;
    descr_est.setRadiusSearch (descr_rad_);
    descr_est.setInputCloud (cloud_filtered);
    descr_est.setInputNormals (cloud_normals);
    descr_est.setSearchSurface (cloud_cropped);
    descr_est.compute (*scene_descriptors);
    std::cout << "Scene total points after filtering: " << cloud_filtered->size() << "; Scene descriptors: " << scene_descriptors->size () << std::endl;

    //  Find Model-Scene Correspondences with KdTree
    pcl::CorrespondencesPtr model_scene_corrs (new pcl::Correspondences ());
    pcl::KdTreeFLANN<pcl::SHOT352> match_search;
    match_search.setInputCloud (model_descriptors_);

    //  For each scene keypoint descriptor, find nearest neighbor into the model keypoints descriptor cloud and add it to the correspondences vector.
    for (std::size_t i = 0; i < scene_descriptors->size() && rclcpp::ok(); ++i)
    {
      std::vector<int> neigh_indices (1);
      std::vector<float> neigh_sqr_dists (1);
      if (!std::isfinite (scene_descriptors->at (i).descriptor[0])) //skipping NaNs
      {
        continue;
      }
      int found_neighs = match_search.nearestKSearch (scene_descriptors->at (i), 1, neigh_indices, neigh_sqr_dists);
      if(found_neighs == 1 && neigh_sqr_dists[0] < 0.25f) //  add match only if the squared descriptor distance is less than 0.25 (SHOT descriptor distances are between 0 and 1 by design)
      {
        pcl::Correspondence corr (neigh_indices[0], static_cast<int> (i), neigh_sqr_dists[0]);
        model_scene_corrs->push_back (corr);
      }
    }
    std::cout << "Correspondences found: " << model_scene_corrs->size () << std::endl;

    // Clustering
    std::vector<Eigen::Matrix4f, Eigen::aligned_allocator<Eigen::Matrix4f> > rototranslations;
    std::vector<pcl::Correspondences> clustered_corrs;

    if(algorithm_ == "hough")  //  Using Hough3D
    {
      //  Compute (Keypoints) Reference Frames only for Hough
      pcl::PointCloud<pcl::ReferenceFrame>::Ptr scene_rf (new pcl::PointCloud<pcl::ReferenceFrame> ());
      pcl::BOARDLocalReferenceFrameEstimation<pcl::PointXYZRGB, pcl::PointNormal, pcl::ReferenceFrame> rf_est_scene;
      rf_est_scene.setFindHoles(true);
      rf_est_scene.setRadiusSearch(rf_rad_);
      rf_est_scene.setInputCloud(cloud_filtered);
      rf_est_scene.setInputNormals(cloud_normals);
      rf_est_scene.setSearchSurface (cloud_cropped);
      rf_est_scene.compute(*scene_rf);

      //  Clustering
      pcl::PointCloud<pcl::PointXYZ>::Ptr model_keypoints_xyz(new pcl::PointCloud<pcl::PointXYZ>());
      pcl::copyPointCloud(*model_keypoints_, *model_keypoints_xyz);
      pcl::Hough3DGrouping<pcl::PointXYZ, pcl::PointXYZRGB, pcl::ReferenceFrame, pcl::ReferenceFrame> clusterer;
      clusterer.setHoughBinSize(cg_size_);
      clusterer.setHoughThreshold(cg_thresh_);
      clusterer.setUseInterpolation(true);
      clusterer.setUseDistanceWeight(true);

      clusterer.setInputCloud(model_keypoints_xyz);
      clusterer.setInputRf(model_rf_);
      clusterer.setSceneCloud(cloud_filtered);
      clusterer.setSceneRf(scene_rf);
      clusterer.setModelSceneCorrespondences(model_scene_corrs);

      //clusterer.cluster (clustered_corrs);
      clusterer.recognize (rototranslations, clustered_corrs);
    }
    else // Using GeometricConsistency
    {
      pcl::GeometricConsistencyGrouping<pcl::PointXYZRGB, pcl::PointXYZRGB> gc_clusterer;
      gc_clusterer.setGCSize(cg_size_);
      gc_clusterer.setGCThreshold(cg_thresh_);

      gc_clusterer.setInputCloud(model_keypoints_);
      gc_clusterer.setSceneCloud(cloud_filtered);
      gc_clusterer.setModelSceneCorrespondences(model_scene_corrs);

      //gc_clusterer.cluster (clustered_corrs);
      gc_clusterer.recognize(rototranslations, clustered_corrs);
    }

    std::vector<pcl::PointCloud<pcl::PointXYZRGB>::Ptr> instances;
    std::cout << "Model instances found: " << rototranslations.size () << std::endl;
    for (std::size_t i = 0; i < rototranslations.size (); ++i)
    {
      std::cout << "\n    Instance " << i + 1 << ":" << std::endl;
      std::cout << "        Correspondences belonging to this instance: " << clustered_corrs[i].size () << std::endl;

      // Print the rotation matrix and translation vector
      Eigen::Matrix3f rotation = rototranslations[i].block<3,3>(0, 0);
      Eigen::Vector3f translation = rototranslations[i].block<3,1>(0, 3);

      printf ("\n");
      printf ("            | %6.3f %6.3f %6.3f | \n", rotation (0,0), rotation (0,1), rotation (0,2));
      printf ("        R = | %6.3f %6.3f %6.3f | \n", rotation (1,0), rotation (1,1), rotation (1,2));
      printf ("            | %6.3f %6.3f %6.3f | \n", rotation (2,0), rotation (2,1), rotation (2,2));
      printf ("\n");
      printf ("        t = < %0.3f, %0.3f, %0.3f >\n", translation (0), translation (1), translation (2));

      pcl::PointCloud<pcl::PointXYZRGB>::Ptr rotated_model(new pcl::PointCloud<pcl::PointXYZRGB> ());
      pcl::transformPointCloud(*model_keypoints_, *rotated_model, rototranslations[i]);
      instances.push_back(rotated_model);
    }

    // ICP Alignment
    std::vector<pcl::PointCloud<pcl::PointXYZRGB>::Ptr> registered_instances;
    for (std::size_t i = 0; i < rototranslations.size(); ++i)
    {
      pcl::IterativeClosestPoint<pcl::PointXYZRGB, pcl::PointXYZRGB> icp;
      icp.setMaximumIterations(icp_max_iter_);
      icp.setMaxCorrespondenceDistance(icp_corr_distance_);
      icp.setTransformationEpsilon(1e-9);
      icp.setTransformationRotationEpsilon(1e-9);
      icp.setInputTarget(cloud_filtered);
      icp.setInputSource(instances[i]);
      pcl::PointCloud<pcl::PointXYZRGB>::Ptr registered(new pcl::PointCloud<pcl::PointXYZRGB>);
      icp.align(*registered);
      registered_instances.push_back(registered);
      std::cout << "Instance " << i << " ";
      if (icp.hasConverged ())
        std::cout << "Aligned!" << std::endl;
      else
        std::cout << "Not Aligned!" << std::endl;
    }

    if(cloud_viewer) {
      pcl::PointCloud<pcl::PointXYZRGB>::Ptr off_scene_model(new pcl::PointCloud<pcl::PointXYZRGB>());
      pcl::PointCloud<pcl::PointXYZRGB>::Ptr off_scene_model_keypoints(new pcl::PointCloud<pcl::PointXYZRGB>());
      pcl::PointCloud<pcl::PointNormal>::Ptr off_scene_model_normals(new pcl::PointCloud<pcl::PointNormal>());

      //  Translate the model so that it doesn't end in the middle of the scene representation
      pcl::transformPointCloud(*model_cloud_, *off_scene_model, Eigen::Vector3d(transform.transform.translation.x,transform.transform.translation.y,0.8), Eigen::Quaterniond(1, 0, 0, 0));
      pcl::transformPointCloud(*model_keypoints_, *off_scene_model_keypoints, Eigen::Vector3d(transform.transform.translation.x,transform.transform.translation.y,0.8), Eigen::Quaterniond(1, 0, 0, 0));
      pcl::transformPointCloud(*model_normals_, *off_scene_model_normals, Eigen::Vector3d(transform.transform.translation.x,transform.transform.translation.y,0.8), Eigen::Quaterniond(1, 0, 0, 0));

      cloud_viewer->runOnVisualizationThreadOnce([=](pcl::visualization::PCLVisualizer &viewer) {
        viewer.removeAllPointClouds();
        viewer.removeAllShapes();

        viewer.addPointCloud(cloud_cropped);
        viewer.addPointCloudNormals<pcl::PointXYZRGB, pcl::PointNormal>(cloud_cropped, cloud_normals, 100, 0.02f, "cloud_normals");

        //pcl_visualizer.addPlane(*plane_coefficients, 6.55, 0.95, 0.8);

        //pcl_visualizer.addPointCloud<pcl::PointNormal>(model_keypoints_, "model_cloud");
        //pcl_visualizer.addPointCloudNormals<pcl::PointNormal>(model_keypoints_, 100, 0.02f, "model_normals");
        pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZRGB> off_scene_model_color_handler(off_scene_model, 255, 255, 128);
        pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZRGB> off_scene_model_keypoints_color_handler(off_scene_model_keypoints, 0, 0, 255);
        viewer.addPointCloud(off_scene_model, off_scene_model_color_handler, "off_scene_model");
        viewer.addPointCloudNormals<pcl::PointXYZRGB, pcl::PointNormal>(off_scene_model, off_scene_model_normals, 100, 0.02f, "off_scene_model_normals");
        viewer.addPointCloud(off_scene_model_keypoints, off_scene_model_keypoints_color_handler, "off_scene_model_keypoints");
        viewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, "off_scene_model_keypoints");

        pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZRGB> scene_keypoints_color_handler(cloud_filtered, 0, 0, 255);
        viewer.addPointCloud(cloud_filtered, scene_keypoints_color_handler, "scene_keypoints");
        viewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, "scene_keypoints");

        for(std::size_t i = 0; i < rototranslations.size(); ++i) {
          std::stringstream ss_instance;
          ss_instance << "instance" << i;

          pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZRGB> instance_color_handler(instances[i], 255, 0, 92);
          viewer.addPointCloud(instances[i], instance_color_handler, ss_instance.str());

          ss_instance << "_registered";
          pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZRGB> registered_model_color_handler(registered_instances[i], 31, 255, 127);
          viewer.addPointCloud(registered_instances[i], registered_model_color_handler, ss_instance.str());

          for(std::size_t j = 0; j < clustered_corrs[i].size(); ++j) {
            std::stringstream ss_line;
            ss_line << "correspondence_line" << i << "_" << j;
            pcl::PointXYZRGB &model_point = off_scene_model_keypoints->at(clustered_corrs[i][j].index_query);
            pcl::PointXYZRGB &scene_point = cloud_filtered->at(clustered_corrs[i][j].index_match);

            //  We are drawing a line for each pair of clustered correspondences found between the model and the scene
            viewer.addLine<pcl::PointXYZRGB, pcl::PointXYZRGB>(model_point, scene_point, 0, 255, 0, ss_line.str());
          }
        }
      });
    }

    return true;
 }


}
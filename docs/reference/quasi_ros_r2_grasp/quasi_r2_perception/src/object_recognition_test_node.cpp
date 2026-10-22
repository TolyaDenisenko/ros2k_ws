// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#include "quasi_r2_perception/object_recognition_test_node.hpp"

#include <pcl/visualization/cloud_viewer.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl_ros/transforms.hpp>
#include <pcl/filters/crop_box.h>
#include <pcl/features/normal_3d_omp.h>

#include <object_msgs/msg/object_in_box.hpp>
#include <geometric_shapes/bodies.h>

#include <tf2_eigen/tf2_eigen.hpp>

#include "quasi_r2_perception/correspondence_grouping.hpp"
#include "quasi_r2_perception/objrec_ransac.hpp"
#include "quasi_r2_perception/object_segmentation.hpp"

using namespace quasi;
using namespace std::string_literals;

int main(int argc, char **argv) {
  // Set up ROS.
  rclcpp::init(argc, argv);

  auto node = std::make_shared<ObjectRecognitionTestNode>();
  if(!node->init())
    return 1;

  rclcpp::spin(node);
  return 0;
}

bool ObjectRecognitionTestNode::init() {
  if(algorithm_ == "ransac")
    object_recognition_ = std::make_shared<RANSACRecognition>(shared_from_this());
  else if(algorithm_ == "hough" || algorithm_ == "GC")
    object_recognition_ = std::make_shared<CorrespondenceGroupingRecognition>(shared_from_this(), algorithm_);
  else if(algorithm_ == "objseg" )
    object_recognition_ = std::make_shared<ObjectSegmentation>(shared_from_this());
  else
    throw std::runtime_error("Unknown algorithm "s + algorithm_);

  tf_buffer_ = std::make_shared<tf2_ros::Buffer>(get_clock());
  tf_buffer_->setUsingDedicatedThread(true);
  tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
  pcl_subscription_ = create_subscription<sensor_msgs::msg::PointCloud2>("/realsense/depth/color/points", rclcpp::SensorDataQoS(),
                                                                         std::bind(&ObjectRecognitionTestNode::on_pointcloud_message, this, std::placeholders::_1));
  objects_subscription_ = create_subscription<object_msgs::msg::ObjectsInBoxes>("/openvino_toolkit/object/detected_objects", rclcpp::SystemDefaultsQoS(),
                                                                                std::bind(&ObjectRecognitionTestNode::on_object_detected_message, this, std::placeholders::_1));

  return object_recognition_->init("/home/masf/tmp/coke.pcd");
}

void ObjectRecognitionTestNode::on_object_detected_message(object_msgs::msg::ObjectsInBoxes::SharedPtr msg) {
  objects_ = msg->objects_vector;
}

void ObjectRecognitionTestNode::on_pointcloud_message(sensor_msgs::msg::PointCloud2::SharedPtr msg) {
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud = pcl::make_shared<pcl::PointCloud<pcl::PointXYZRGB>>();
  pcl::fromROSMsg(*msg, *cloud);

  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_transformed(new pcl::PointCloud<pcl::PointXYZRGB>);
  if( !pcl_ros::transformPointCloud("map"s, *cloud, *cloud_transformed, *tf_buffer_) )
    return;

//  double res = computeCloudResolution<pcl::PointXYZRGB>(cloud);
//  RCLCPP_INFO(get_logger(), "Model cloud resolution: %f", res);

  std::vector<std::shared_ptr<bodies::Body>> boxes;
  for( const auto& object : objects_) {
    auto roi = object.roi;
    rclcpp::Parameter reduce_roi_parameter;
    if(get_parameter("reduce_roi", reduce_roi_parameter)) {
      auto reduce_roi = reduce_roi_parameter.get_value<int32_t>();
      roi.x_offset += (roi.width - roi.width / reduce_roi) / 2;
      roi.y_offset += (roi.height - roi.height / reduce_roi) / 2;
      roi.width = roi.width / reduce_roi;
      roi.height = roi.height / reduce_roi;
    }
    double min_x = std::numeric_limits<double>::max(), max_x = -std::numeric_limits<double>::max(),
           min_y = std::numeric_limits<double>::max(), max_y = -std::numeric_limits<double>::max(),
           min_z = std::numeric_limits<double>::max(), max_z = -std::numeric_limits<double>::max();
    for(size_t y = 0; y < roi.height; y++) {  // rows
      int idx = (y + roi.y_offset) * 848 + roi.x_offset;
      for(size_t x = 0; x < roi.width; x++) {  // columns
        const auto& point = cloud->points[idx + x];
        if(!isnan(point.x) && !isnan(point.y) && !isnan(point.z)) {
          if(point.x < min_x)
            min_x = point.x;
          if(point.x > max_x)
            max_x = point.x;
          if(point.y < min_y)
            min_y = point.y;
          if(point.y > max_y)
            max_y = point.y;
          if(point.z < min_z)
            min_z = point.z;
          if(point.z > max_z)
            max_z = point.z;
        }
      }
    }

    RCLCPP_DEBUG(get_logger(), "min/max: %f %f   %f %f   %f %f", min_x, max_x, min_y, max_y, min_z, max_z);
    if(min_x == std::numeric_limits<double>::max())
      continue;

    shapes::Box shape(max_x-min_x, max_y-min_y, max_z-min_z);
    std::shared_ptr<bodies::Body>& box = boxes.emplace_back(new bodies::Box(&shape));

    Eigen::Isometry3d pose = Eigen::Translation3d((min_x+max_x)/2.0, (min_y+max_y)/2.0, (min_z+max_z)/2.0) * Eigen::Quaterniond(1.0, 0.0, 0.0, 0.0);
    RCLCPP_DEBUG(get_logger(), "pose in map frame: %f %f %f  %f %f %f", pose.translation().x(), pose.translation().y(), pose.translation().z(),
                 box->getDimensions()[0], box->getDimensions()[1], box->getDimensions()[2]);
    tf2::doTransform(pose, pose, tf_buffer_->lookupTransform("map", cloud->header.frame_id, tf2::TimePointZero));
    box->setPose(pose);
  }


  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_cropped(new pcl::PointCloud<pcl::PointXYZRGB>);
  pcl::CropBox<pcl::PointXYZRGB> boxFilter;
  boxFilter.setMin(Eigen::Vector4f(-0.91, -0.4, -0.005, 1.0));
  boxFilter.setMax(Eigen::Vector4f(+0.91, +0.4, 1.0, 1.0));
  boxFilter.setTranslation(Eigen::Vector3f(6.55, 0.95, 0.8));
  boxFilter.setInputCloud(cloud_transformed);
  boxFilter.filter(*cloud_cropped);
  if(cloud_cropped->empty()) {
    RCLCPP_INFO(get_logger(), "Empty cloud after workspace cropping");
    return;
  }
  cloud_cropped = cloud_transformed;


  // Estimate point normals
  pcl::PointCloud<pcl::PointNormal>::Ptr cloud_normals(new pcl::PointCloud<pcl::PointNormal>);
  pcl::NormalEstimationOMP<pcl::PointXYZRGB, pcl::PointNormal> ne;
  ne.setInputCloud(cloud_cropped);
  ne.setKSearch(30);
  //ne.setRadiusSearch(0.05);
  auto map_transform = tf_buffer_->lookupTransform("map", "realsense_color_frame", tf2::TimePointZero);
  ne.setViewPoint(map_transform.transform.translation.x, map_transform.transform.translation.y, map_transform.transform.translation.z);
  ne.compute(*cloud_normals);

  object_recognition_->recognize(cloud_cropped, cloud_normals, map_transform, cloud_viewer_);

  // Visualize boxes
  cloud_viewer_->runOnVisualizationThreadOnce([=] (pcl::visualization::PCLVisualizer& pcl_visualizer) {
    if(get_parameter("show_box").as_bool()) {
      for(size_t i = 0; i < boxes.size(); ++i) {
        auto &box = boxes[i];
        std::string box_name = "cube_"s + std::to_string(i);
        Eigen::Isometry3f center = box->getPose().cast<float>();
        pcl_visualizer.addCube((Eigen::Vector3f) center.translation(), (Eigen::Quaternionf) center.rotation(),
                               box->getDimensions()[0], box->getDimensions()[1], box->getDimensions()[2], box_name);
      }
    }
  });

}

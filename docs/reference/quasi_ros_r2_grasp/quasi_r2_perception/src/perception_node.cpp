// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

#include "quasi_r2_perception/perception_node.hpp"
#include <quasi_utils/service_client.hpp>
#include <quasi_utils/quasi_utils.hpp>

#include <pcl_ros/transforms.hpp>
#include <pcl/filters/crop_box.h>

#include <tf2_eigen/tf2_eigen.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <lifecycle_msgs/msg/state.hpp>

using namespace quasi;
using namespace std::string_literals;

int main(int argc, char **argv) {
  // Set up ROS.
  rclcpp::init(argc, argv);

  auto node = std::make_shared<PerceptionNode>();
  rclcpp::contexts::get_global_default_context()->add_pre_shutdown_callback([node](){
    if(node->get_current_state().id() > lifecycle_msgs::msg::State::PRIMARY_STATE_UNKNOWN && node->get_current_state().id() < lifecycle_msgs::msg::State::PRIMARY_STATE_FINALIZED)
      node->shutdown();
  });
  rclcpp::spin(node->get_node_base_interface());
  return 0;
}

PerceptionNode::CallbackReturn PerceptionNode::on_configure(const rclcpp_lifecycle::State &/*previous_state*/) {
  RCLCPP_INFO(get_logger(), "Configuring node...");

  try {
    // Create the transform-related objects. Do it here instead of on_activate because it takes time to get TF buffer populated with data.
    tf_buffer_ = std::make_shared<tf2_ros::Buffer>(get_clock());
    tf_buffer_->setUsingDedicatedThread(true);
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

    object_segmentation_ = std::make_shared<ObjectSegmentation>(shared_from_this());

    // Load Object Types config
    rclcpp::Parameter object_types_file_path_param;
    if(get_parameter("object_types_file_path", object_types_file_path_param)) {
      std::ifstream object_types_config_file(quasi::utils::expand_user_home_dir(object_types_file_path_param.as_string().c_str()));
      if(object_types_config_file.good())
        object_types_config_ = nlohmann::json::parse(object_types_config_file);
    }

  }
  catch(std::exception &ex) {
    RCLCPP_ERROR(get_logger(), "exception while configuring: %s", ex.what());
    return CallbackReturn::FAILURE;
  }

  return CallbackReturn::SUCCESS;
}

PerceptionNode::CallbackReturn PerceptionNode::on_activate(const rclcpp_lifecycle::State &/*previous_state*/) {
  RCLCPP_INFO(get_logger(), "Activating node...");

  detect_object_action_server_ = std::make_shared<BasicActionServer<quasi_r2_msgs::action::DetectObject>>(this, "detect_object",
                                            std::bind(&PerceptionNode::execute_detect_object_goal, this, std::placeholders::_1),
                                            [&] (const std::shared_ptr<const quasi_r2_msgs::action::DetectObject::Goal>& ) { return !(find_place_pose_action_server_ && find_place_pose_action_server_->is_goal_active());});
  find_place_pose_action_server_ = std::make_shared<BasicActionServer<quasi_r2_msgs::action::FindPlacePose>>(this, "find_place_pose",
                                            std::bind(&PerceptionNode::execute_find_place_pose_goal, this, std::placeholders::_1),
                                            [&] (const std::shared_ptr<const quasi_r2_msgs::action::FindPlacePose::Goal>& ) { return !(detect_object_action_server_ && detect_object_action_server_->is_goal_active());});

  if(get_parameter("use_bond").as_bool()) {
    // create bond connection
    RCLCPP_INFO(get_logger(), "Creating bond (%s) to lifecycle manager.", this->get_name());
    bond_ = std::make_unique<bond::Bond>(std::string("bond"), this->get_name(), shared_from_this());
    bond_->start();
  }

  return CallbackReturn::SUCCESS;
}

PerceptionNode::CallbackReturn PerceptionNode::on_deactivate(const rclcpp_lifecycle::State &/*previous_state*/) {
  RCLCPP_INFO(get_logger(), "Deactivating node...");

  // TODO: cancel current goal if such exists. For now just refuse to deactivate.
  if((detect_object_action_server_ && detect_object_action_server_->is_goal_active()) ||
     (find_place_pose_action_server_ && find_place_pose_action_server_->is_goal_active())) {
    RCLCPP_ERROR(get_logger(), "Deactivation cannot be done because action goal is currently being executed");
    return CallbackReturn::FAILURE;
  }
  detect_object_action_server_.reset();
  find_place_pose_action_server_.reset();

  if(bond_) {
    RCLCPP_INFO(get_logger(), "Destroying bond (%s) to lifecycle manager.", this->get_name());
    bond_.reset();
  }

  return CallbackReturn::SUCCESS;
}

PerceptionNode::CallbackReturn PerceptionNode::on_cleanup(const rclcpp_lifecycle::State &/*previous_state*/) {
  object_segmentation_.reset();

  tf_listener_.reset();
  tf_buffer_.reset();

  RCLCPP_INFO(get_logger(), "Cleaning up node...");
  return CallbackReturn::SUCCESS;
}

PerceptionNode::CallbackReturn PerceptionNode::on_shutdown(const rclcpp_lifecycle::State &previous_state) {
  if(previous_state.id() == lifecycle_msgs::msg::State::PRIMARY_STATE_ACTIVE)
    on_deactivate(previous_state);
  if(previous_state.id() == lifecycle_msgs::msg::State::PRIMARY_STATE_ACTIVE ||
     previous_state.id() == lifecycle_msgs::msg::State::PRIMARY_STATE_INACTIVE)
    on_cleanup(previous_state);

  return CallbackReturn::SUCCESS;
}



pcl::PointCloud<pcl::PointXYZRGB>::Ptr PerceptionNode::filter_cloud_by_workspace(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_transformed, const vision_msgs::msg::BoundingBox3DArray& workspace) {
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_cropped(new pcl::PointCloud<pcl::PointXYZRGB>());
  auto frame_id = get_parameter("frame_id").as_string();
  if(!workspace.boxes.empty()) {
    for(const auto& workspace_box : workspace.boxes) {
      Eigen::Isometry3d box_pose;
      tf2::fromMsg(workspace_box.center, box_pose);
      if(!workspace.header.frame_id.empty() && workspace.header.frame_id != frame_id)
        tf2::doTransform(box_pose, box_pose, tf_buffer_->lookupTransform(frame_id, workspace.header.frame_id, tf2::TimePointZero));

      pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_workspace_box(new pcl::PointCloud<pcl::PointXYZRGB>);
      pcl::CropBox<pcl::PointXYZRGB> boxFilter;
      boxFilter.setMin(Eigen::Vector4f(-(float)workspace_box.size.x/2.0f, -(float)workspace_box.size.y, -(float)workspace_box.size.z/2.0f, 1.0f));
      boxFilter.setMax(Eigen::Vector4f((float)workspace_box.size.x/2.0f, (float)workspace_box.size.y, (float)workspace_box.size.z/2.0f, 1.0f));
      boxFilter.setTranslation(box_pose.translation().cast<float>());
      boxFilter.setRotation(box_pose.rotation().eulerAngles(0,1,2).cast<float>());
      boxFilter.setInputCloud(cloud_transformed);
      boxFilter.filter(*cloud_workspace_box);
      *cloud_cropped += *cloud_workspace_box;
    }
    if(cloud_cropped->empty()) {
      RCLCPP_INFO(get_logger(), "Empty cloud after workspace cropping");
      return nullptr;
    }
  }
  else {
    auto transform = tf2::transformToEigen(tf_buffer_->lookupTransform(frame_id, "base_footprint", tf2::TimePointZero));

    float default_workspace_width = (float)get_parameter("default_workspace.width").as_double();
    float default_workspace_length = (float)get_parameter("default_workspace.length").as_double();
    float default_workspace_height = (float)get_parameter("default_workspace.height").as_double();
    float origin_z = (float)get_parameter("default_workspace.origin_z").as_double();

    pcl::CropBox<pcl::PointXYZRGB> boxFilter;
    boxFilter.setMin(Eigen::Vector4f(0.0f, -default_workspace_width/2.0f, origin_z, 1.0f));
    boxFilter.setMax(Eigen::Vector4f((float)default_workspace_length, default_workspace_width/2.0f, origin_z + default_workspace_height, 1.0f));
    boxFilter.setTranslation(transform.translation().cast<float>());
    boxFilter.setRotation(transform.rotation().eulerAngles(0,1,2).cast<float>());
    boxFilter.setInputCloud(cloud_transformed);
    boxFilter.filter(*cloud_cropped);
  }
  return cloud_cropped;
}

uint8_t PerceptionNode::parse_object_shape(const std::string& shape_type) {
  if(shape_type.empty())
    return 0;

  uint8_t object_shape_type;
  if(!strcasecmp(shape_type.c_str(), shapes::Box::STRING_NAME.c_str()) ||
     !strcasecmp(shape_type.c_str(), "cuboid"))
    object_shape_type = shape_msgs::msg::SolidPrimitive::BOX;
  else if(!strcasecmp(shape_type.c_str(), shapes::Cylinder::STRING_NAME.c_str()) )
    object_shape_type = shape_msgs::msg::SolidPrimitive::CYLINDER;
  else if(!strcasecmp(shape_type.c_str(), shapes::Sphere::STRING_NAME.c_str()) )
    object_shape_type = shape_msgs::msg::SolidPrimitive::SPHERE;
  else if(!strcasecmp(shape_type.c_str(), shapes::Mesh::STRING_NAME.c_str()) )
    object_shape_type = 0;
  else if(!strcasecmp(shape_type.c_str(), "auto") )
    object_shape_type = 255;
  else
    throw std::runtime_error("Unsupported shape type: "s + shape_type);

  return object_shape_type;
}

std::string PerceptionNode::object_shape_name(uint8_t shape_type) {
  switch(shape_type) {
    case 0:
      return "mesh"s;
    case shape_msgs::msg::SolidPrimitive::BOX:
      return "box"s;
    case shape_msgs::msg::SolidPrimitive::SPHERE:
      return "sphere"s;
    case shape_msgs::msg::SolidPrimitive::CYLINDER:
      return "cylinder"s;
    case shape_msgs::msg::SolidPrimitive::CONE:
      return "cone"s;
    case shape_msgs::msg::SolidPrimitive::PRISM:
      return "prism"s;
    case 255:
      return "auto"s;
    default:
      return ""s;
      //throw std::runtime_error("Unsupported shape type: "s + std::to_string(shape_type));
  }
}
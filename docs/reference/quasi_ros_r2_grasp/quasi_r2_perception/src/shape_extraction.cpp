// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.
/*
 * Copyright 2021, Quasi Robotics Inc.
 * Copyright 2020, Michael Ferguson
 * Copyright 2013-2014, Unbounded Robotics Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Unbounded Robotics, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

// Author: Michael Ferguson
// Author: Fedor Bokov

#include <cmath>
#include <Eigen/Eigen>
#include "pcl/ModelCoefficients.h"
#include "pcl/filters/project_inliers.h"
#include "pcl/surface/convex_hull.h"
#include "quasi_r2_perception/perception_node.hpp"

namespace quasi
{

pcl::PointCloud<pcl::PointXYZRGB>::Ptr PerceptionNode::project_onto_plane(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& input, const pcl::ModelCoefficients::Ptr& plane, Eigen::Matrix3f& inv_plane_rotation) {
  // Project object into 2d, using plane model coefficients
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr flat(new pcl::PointCloud<pcl::PointXYZRGB>);
  pcl::ProjectInliers<pcl::PointXYZRGB> projection;
  projection.setModelType(pcl::SACMODEL_PLANE);
  projection.setInputCloud(input);
  projection.setModelCoefficients(plane);
  projection.filter(*flat);

  // Rotate plane so that Z=0
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr flat_projected(new pcl::PointCloud<pcl::PointXYZRGB>);
  Eigen::Vector3f normal(plane->values[0], plane->values[1], plane->values[2]);
  Eigen::Quaternionf qz; qz.setFromTwoVectors(normal, Eigen::Vector3f::UnitZ());
  Eigen::Matrix3f plane_rotation = qz.toRotationMatrix();
  inv_plane_rotation = plane_rotation.inverse();

  for (size_t i = 0; i < flat->size(); ++i)
  {
    pcl::PointXYZRGB p;
    p.getVector3fMap() = plane_rotation * (*flat)[i].getVector3fMap();
    flat_projected->push_back(p);
  }

  return flat_projected;
}

bool PerceptionNode::extract_primitive_shape_bc(uint8_t shape_type,
                                                const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& input,
                                                const pcl::ModelCoefficients::Ptr& plane,
                                                shape_msgs::msg::SolidPrimitive& shape,
                                                geometry_msgs::msg::Pose& pose)
{
  // Used to decide between various shapes
  double min_volume = std::numeric_limits<double>::max();  // the minimum volume shape found thus far.
  Eigen::Matrix3f transformation;  // the transformation for the best-fit shape

  // Compute min/max z and height as minimum and maximum distances from plane
  double max_z = -std::numeric_limits<double>::max();
  double min_z = std::numeric_limits<double>::max();
  Eigen::Vector4f m(plane->values[0], plane->values[1], plane->values[2], plane->values[3]);
  for (const auto & pt : *input)
  {
    auto pp = pt.getVector4fMap();
    double distance_to_plane = fabs(pp.dot(m));
    if(distance_to_plane > max_z)
      max_z = distance_to_plane;
    if(distance_to_plane < min_z)
      min_z = distance_to_plane;
  }
  double height = max_z - min_z;

  Eigen::Matrix3f inv_plane_rotation;
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr flat_projected = project_onto_plane(input, plane, inv_plane_rotation);

  // Find the convex hull
  pcl::PointCloud<pcl::PointXYZRGB> hull;
  pcl::ConvexHull<pcl::PointXYZRGB> convex_hull;
  convex_hull.setInputCloud(flat_projected);
  convex_hull.setDimension(2);
  convex_hull.reconstruct(hull);

  if(shape_type == shape_msgs::msg::SolidPrimitive::BOX || shape_type == 255) {
    // Try fitting a rectangle
    shape_msgs::msg::SolidPrimitive rect;  // the best-fit rectangle
    rect.type = shape_msgs::msg::SolidPrimitive::BOX;
    rect.dimensions.resize(3);
    for (size_t i = 0; i < hull.size() - 1; ++i)
    {
      // For each pair of hull points, determine the angle
      float rise = hull[i+1].y - hull[i].y;
      float run = hull[i+1].x - hull[i].x;
      // and normalize..
      {
        float l = sqrt((rise * rise) + (run * run));
        rise = rise/l;
        run = run/l;
      }

      // Build rotation matrix from change of basis
      Eigen::Matrix3f rotation;
      rotation(0, 0) = run;
      rotation(0, 1) = rise;
      rotation(0, 2) = 0.0;
      rotation(1, 0) = -rise;
      rotation(1, 1) = run;
      rotation(1, 2) = 0.0;
      rotation(2, 0) = 0.0;
      rotation(2, 1) = 0.0;
      rotation(2, 2) = 1.0;
      Eigen::Matrix3f inv_rotation = rotation.inverse();

      // Project hull to new coordinate system
      pcl::PointCloud<pcl::PointXYZRGB> projected_cloud;
      for (size_t j = 0; j < hull.size(); ++j)
      {
        pcl::PointXYZRGB p;
        p.getVector3fMap() = rotation * hull[j].getVector3fMap();
        projected_cloud.push_back(p);
      }

      // Compute min/max
      double x_min = std::numeric_limits<double>::max();
      double x_max = -std::numeric_limits<double>::max();
      double y_min = std::numeric_limits<double>::max();
      double y_max = -std::numeric_limits<double>::max();
      for (size_t j = 0; j < projected_cloud.size(); ++j)
      {
        if (projected_cloud[j].x < x_min)
          x_min = projected_cloud[j].x;
        if (projected_cloud[j].x > x_max)
          x_max = projected_cloud[j].x;

        if (projected_cloud[j].y < y_min)
          y_min = projected_cloud[j].y;
        if (projected_cloud[j].y > y_max)
          y_max = projected_cloud[j].y;
      }

      // Is this the best estimate?
      double area = (x_max - x_min) * (y_max - y_min);
      if (area*height < min_volume)
      {
        transformation = inv_plane_rotation * inv_rotation;

        rect.dimensions[0] = (x_max - x_min);
        rect.dimensions[1] = (y_max - y_min);
        rect.dimensions[2] = height;

        Eigen::Vector3f pose3f((x_max + x_min)/2.0, (y_max + y_min)/2.0, projected_cloud[0].z + min_z + height/2.0);
        pose3f = transformation * pose3f;
        pose.position.x = pose3f(0);
        pose.position.y = pose3f(1);
        pose.position.z = pose3f(2);

        Eigen::Quaternionf q(transformation);
        pose.orientation.x = q.x();
        pose.orientation.y = q.y();
        pose.orientation.z = q.z();
        pose.orientation.w = q.w();

        min_volume = area * height;
        shape = rect;
      }
    }
  }

  if(shape_type == shape_msgs::msg::SolidPrimitive::CYLINDER || shape_type == 255) {
    // Try fitting a cylinder
    shape_msgs::msg::SolidPrimitive cylinder;  // the best-fit cylinder
    cylinder.type = shape_msgs::msg::SolidPrimitive::CYLINDER;
    cylinder.dimensions.resize(2);
    for (size_t i = 0; i < hull.size(); ++i)
    {
      for (size_t j = i + 1; j < hull.size(); ++j)
      {
        // For each pair of hull points determine the center point
        //  between them as a possible cylinder
        pcl::PointXYZRGB p;
        p.x = (hull[i].x + hull[j].x) / 2.0;
        p.y = (hull[i].y + hull[j].y) / 2.0;
        double radius = 0.0;
        // Find radius from this point
        for (size_t k = 0; k < hull.size(); ++k)
        {
          double dx = hull[k].x - p.x;
          double dy = hull[k].y - p.y;
          double r = sqrt((dx * dx) + (dy * dy));
          if (r > radius)
            radius = r;
        }
        // Is this cylinder the best match?
        double volume = M_PI * radius * radius * height;
        if (volume < min_volume)
        {
          transformation = inv_plane_rotation;

          cylinder.dimensions[0] = height;
          cylinder.dimensions[1] = radius;

          Eigen::Vector3f pose3f(p.x, p.y, hull[0].z + min_z + height/2.0);
          pose3f = transformation * pose3f;
          pose.position.x = pose3f(0);
          pose.position.y = pose3f(1);
          pose.position.z = pose3f(2);

          min_volume = volume;
          shape = cylinder;
        }
      }
    }
  }

  return true;
}

}  // namespace quasi

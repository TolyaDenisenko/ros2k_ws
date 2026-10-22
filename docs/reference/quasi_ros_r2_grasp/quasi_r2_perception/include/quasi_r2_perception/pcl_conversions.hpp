// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.
#ifndef QUASI_PCL_CONVERSIONS_HPP
#define QUASI_PCL_CONVERSIONS_HPP

#include <pcl/PolygonMesh.h>
#include <pcl_msgs/msg/polygon_mesh.hpp>
#include <pcl_conversions/pcl_conversions.h>
#include <shape_msgs/msg/mesh.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>

namespace quasi {

  static shape_msgs::msg::Mesh& pcl_mesh_to_shape_mesh(const pcl::PolygonMesh& polygon_mesh, shape_msgs::msg::Mesh& mesh) {
    pcl_msgs::msg::PolygonMesh pcl_msg_mesh;
    pcl_conversions::fromPCL(polygon_mesh, pcl_msg_mesh);

    size_t size = sensor_msgs::PointCloud2Modifier(pcl_msg_mesh.cloud).size();

    // Convert points represented as point cloud in CPL Mesh to vertices in destination ROS Mesh
    mesh.vertices.resize(size);
    sensor_msgs::PointCloud2ConstIterator<float> pt_iter(pcl_msg_mesh.cloud, "x");
    for(size_t i = 0u; i < size; i++, ++pt_iter) {
      mesh.vertices[i].x = pt_iter[0];
      mesh.vertices[i].y = pt_iter[1];
      mesh.vertices[i].z = pt_iter[2];
    }

    // Convert polygon vertex indexes in PCL Mesh to triangle vertex indexes in ROS Mesh
    mesh.triangles.resize(polygon_mesh.polygons.size());
    for(size_t i = 0u; i < polygon_mesh.polygons.size(); ++i) {
      if(polygon_mesh.polygons[i].vertices.size() < 3u)
        continue;

      for(size_t j = 0u; j < 3u; ++j)
        mesh.triangles[i].vertex_indices[j] = polygon_mesh.polygons[i].vertices[j];
    }

    return mesh;
  }

}

#endif // QUASI_PCL_CONVERSIONS_HPP
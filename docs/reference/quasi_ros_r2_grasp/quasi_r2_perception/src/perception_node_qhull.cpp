// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.
#include <pcl/pcl_config.h>
#if PCL_VERSION_COMPARE(<, 1, 12, 0)
extern "C" {
  #include <libqhull/libqhull.h>
}
#endif

#include <geometric_shapes/bodies.h>
#include <geometric_shapes/body_operations.h>
#include <geometric_shapes/shape_operations.h>
#include <geometry_msgs/msg/pose.hpp>
#include <tf2/convert.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2_eigen/tf2_eigen.hpp>
#include "quasi_r2_perception/perception_node.hpp"

#if PCL_VERSION_COMPARE(<, 1, 12, 0)
namespace bodies
{
  struct ConvexMesh::MeshData
  {
    EigenSTL::vector_Vector4d planes_;
    EigenSTL::vector_Vector3d vertices_;
    std::vector<unsigned int> triangles_;
    std::map<unsigned int, unsigned int> plane_for_triangle_;
    std::map<unsigned int, unsigned int> triangle_for_plane_;
    Eigen::Vector3d mesh_center_;
    double mesh_radiusB_;
    Eigen::Vector3d box_offset_;
    Eigen::Vector3d box_size_;
    BoundingCylinder bounding_cylinder_;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  };
}  // namespace bodies

void bodies::ConvexMesh::useDimensions(const shapes::Shape* shape)
{
  mesh_data_ = std::allocate_shared<MeshData>(Eigen::aligned_allocator<MeshData>());
  const shapes::Mesh* mesh = static_cast<const shapes::Mesh*>(shape);

  double maxX = -std::numeric_limits<double>::infinity(), maxY = -std::numeric_limits<double>::infinity(),
      maxZ = -std::numeric_limits<double>::infinity();
  double minX = std::numeric_limits<double>::infinity(), minY = std::numeric_limits<double>::infinity(),
      minZ = std::numeric_limits<double>::infinity();

  for (unsigned int i = 0; i < mesh->vertex_count; ++i)
  {
    double vx = mesh->vertices[3 * i];
    double vy = mesh->vertices[3 * i + 1];
    double vz = mesh->vertices[3 * i + 2];

    if (maxX < vx)
      maxX = vx;
    if (maxY < vy)
      maxY = vy;
    if (maxZ < vz)
      maxZ = vz;

    if (minX > vx)
      minX = vx;
    if (minY > vy)
      minY = vy;
    if (minZ > vz)
      minZ = vz;
  }

  if (maxX < minX)
    maxX = minX = 0.0;
  if (maxY < minY)
    maxY = minY = 0.0;
  if (maxZ < minZ)
    maxZ = minZ = 0.0;

  mesh_data_->box_size_ = Eigen::Vector3d(maxX - minX, maxY - minY, maxZ - minZ);

  mesh_data_->box_offset_ = Eigen::Vector3d((minX + maxX) / 2.0, (minY + maxY) / 2.0, (minZ + maxZ) / 2.0);

  mesh_data_->planes_.clear();
  mesh_data_->triangles_.clear();
  mesh_data_->vertices_.clear();
  mesh_data_->mesh_radiusB_ = 0.0;
  mesh_data_->mesh_center_ = Eigen::Vector3d();

  double xdim = maxX - minX;
  double ydim = maxY - minY;
  double zdim = maxZ - minZ;

  double pose1;
  double pose2;

  unsigned int off1;
  unsigned int off2;

  /* compute bounding cylinder */
  double cyl_length;
  double maxdist = -std::numeric_limits<double>::infinity();
  if (xdim > ydim && xdim > zdim)
  {
    off1 = 1;
    off2 = 2;
    pose1 = mesh_data_->box_offset_.y();
    pose2 = mesh_data_->box_offset_.z();
    cyl_length = xdim;
  }
  else if (ydim > zdim)
  {
    off1 = 0;
    off2 = 2;
    pose1 = mesh_data_->box_offset_.x();
    pose2 = mesh_data_->box_offset_.z();
    cyl_length = ydim;
  }
  else
  {
    off1 = 0;
    off2 = 1;
    pose1 = mesh_data_->box_offset_.x();
    pose2 = mesh_data_->box_offset_.y();
    cyl_length = zdim;
  }

  /* compute convex hull */
  coordT* points = (coordT*)calloc(mesh->vertex_count * 3, sizeof(coordT));
  for (unsigned int i = 0; i < mesh->vertex_count; ++i)
  {
    points[3 * i + 0] = (coordT)mesh->vertices[3 * i + 0];
    points[3 * i + 1] = (coordT)mesh->vertices[3 * i + 1];
    points[3 * i + 2] = (coordT)mesh->vertices[3 * i + 2];

    double dista = mesh->vertices[3 * i + off1] - pose1;
    double distb = mesh->vertices[3 * i + off2] - pose2;
    double dist = sqrt(((dista * dista) + (distb * distb)));
    if (dist > maxdist)
      maxdist = dist;
  }
  mesh_data_->bounding_cylinder_.radius = maxdist;
  mesh_data_->bounding_cylinder_.length = cyl_length;

  static FILE* null = fopen("/dev/null", "w");

  char flags[] = "qhull Tv Qt";
  int exitcode = qh_new_qhull(3, mesh->vertex_count, points, true, flags, null, null);

  if (exitcode != 0)
  {
    qh_freeqhull(!qh_ALL);
    int curlong, totlong;
    qh_memfreeshort(&curlong, &totlong);
    return;
  }

  int num_facets = qh num_facets;

  int num_vertices = qh num_vertices;
  mesh_data_->vertices_.reserve(num_vertices);
  Eigen::Vector3d sum(0, 0, 0);

  // necessary for FORALLvertices
  std::map<unsigned int, unsigned int> qhull_vertex_table;
  vertexT* vertex;
  FORALLvertices
  {
    Eigen::Vector3d vert(vertex->point[0], vertex->point[1], vertex->point[2]);
    qhull_vertex_table[vertex->id] = mesh_data_->vertices_.size();
    sum += vert;
    mesh_data_->vertices_.push_back(vert);
  }

  mesh_data_->mesh_center_ = sum / (double)(num_vertices);
  for (unsigned int j = 0; j < mesh_data_->vertices_.size(); ++j)
  {
    double dist = (mesh_data_->vertices_[j] - mesh_data_->mesh_center_).squaredNorm();
    if (dist > mesh_data_->mesh_radiusB_)
      mesh_data_->mesh_radiusB_ = dist;
  }

  mesh_data_->mesh_radiusB_ = sqrt(mesh_data_->mesh_radiusB_);
  mesh_data_->triangles_.reserve(num_facets);

  // neccessary for qhull macro
  facetT* facet;
  FORALLfacets
  {
    Eigen::Vector4d planeEquation(facet->normal[0], facet->normal[1], facet->normal[2], facet->offset);
    if (!mesh_data_->planes_.empty())
    {
      // filter equal planes - assuming same ones follow each other
      if ((planeEquation - mesh_data_->planes_.back()).cwiseAbs().maxCoeff() > 1e-6)  // max diff to last
        mesh_data_->planes_.push_back(planeEquation);
    }
    else
    {
      mesh_data_->planes_.push_back(planeEquation);
    }

    // Needed by FOREACHvertex_i_
    int vertex_n, vertex_i;
    FOREACHvertex_i_((*facet).vertices)
    {
      mesh_data_->triangles_.push_back(qhull_vertex_table[vertex->id]);
    }

    mesh_data_->plane_for_triangle_[(mesh_data_->triangles_.size() - 1) / 3] = mesh_data_->planes_.size() - 1;
    mesh_data_->triangle_for_plane_[mesh_data_->planes_.size() - 1] = (mesh_data_->triangles_.size() - 1) / 3;
  }
  qh_freeqhull(!qh_ALL);
  int curlong, totlong;
  qh_memfreeshort(&curlong, &totlong);
}
#endif

namespace quasi {
  bool PerceptionNode::extract_primitive_shape(uint8_t shape_type,
                                               const geometry_msgs::msg::Pose& mesh_pose, const shape_msgs::msg::Mesh& mesh,
                                               geometry_msgs::msg::Pose& pose, shape_msgs::msg::SolidPrimitive& shape) {
    std::shared_ptr<bodies::Body> mesh_body = std::shared_ptr<bodies::Body>(bodies::constructBodyFromMsg(mesh, mesh_pose));
    std::shared_ptr<bodies::Body> primitive_body;
    if(shape_type == shape_msgs::msg::SolidPrimitive::CYLINDER) {
      bodies::BoundingCylinder bounding_cylinder;
      mesh_body->computeBoundingCylinder(bounding_cylinder);
      primitive_body = std::make_shared<bodies::Cylinder>(bounding_cylinder);
    }
    else if(shape_type == shape_msgs::msg::SolidPrimitive::BOX) {
      bodies::AABB bounding_box;
      mesh_body->computeBoundingBox(bounding_box);
      primitive_body = std::make_shared<bodies::Box>(bounding_box);
    }
    else if(shape_type == shape_msgs::msg::SolidPrimitive::SPHERE) {
      bodies::BoundingSphere bounding_sphere;
      mesh_body->computeBoundingSphere(bounding_sphere);
      primitive_body = std::make_shared<bodies::Sphere>(bounding_sphere);
    }

    if(primitive_body) {
      std::shared_ptr<const shapes::Shape> primitive_shape = bodies::constructShapeFromBody(primitive_body.get());
      shapes::ShapeMsg sm;
      if(shapes::constructMsgFromShape(primitive_shape.get(), sm)) {
        pose = tf2::toMsg(primitive_body->getPose());
        shape = boost::get<shape_msgs::msg::SolidPrimitive>(sm);
        return true;
      }
    }

    return false;
  }
}
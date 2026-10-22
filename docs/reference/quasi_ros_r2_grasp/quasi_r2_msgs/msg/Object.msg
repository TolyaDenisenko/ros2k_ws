###########################################################
# This message describes an object.

# Many of the geometric items below lack a stamp/frame_id,
# header stamp/frame_id should be used there
std_msgs/Header header

# An object might have a name
string name

###########################################################
# Objects have many possible descriptions
#  The following are the possible description formats

# Perception modules often represent an object as a cluster of points
#  Is considered valid if number of points > 0
sensor_msgs/PointCloud2 point_cluster

# MoveIt prefers solid primitives or meshes as a description of objects
shape_msgs/SolidPrimitive[<=1] primitives
geometry_msgs/Pose[<=1] primitive_poses

shape_msgs/Mesh[<=1] meshes
geometry_msgs/Pose[<=1] mesh_poses

# An object representing a support surface might be described by a plane
# Is considered valid if coefficients are not all 0s.
shape_msgs/Plane surface

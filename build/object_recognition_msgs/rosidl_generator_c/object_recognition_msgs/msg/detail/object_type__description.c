// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from object_recognition_msgs:msg/ObjectType.idl
// generated code does not contain a copyright notice

#include "object_recognition_msgs/msg/detail/object_type__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_object_recognition_msgs
const rosidl_type_hash_t *
object_recognition_msgs__msg__ObjectType__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x5e, 0xb3, 0xee, 0x39, 0x76, 0x40, 0x33, 0x66,
      0xd7, 0xc5, 0x0d, 0x7d, 0xbe, 0xe8, 0xbe, 0xc9,
      0x6b, 0x3b, 0xce, 0xd1, 0xe7, 0x52, 0x67, 0x4a,
      0x71, 0x94, 0xa3, 0x58, 0x41, 0x63, 0x21, 0x34,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char object_recognition_msgs__msg__ObjectType__TYPE_NAME[] = "object_recognition_msgs/msg/ObjectType";

// Define type names, field names, and default values
static char object_recognition_msgs__msg__ObjectType__FIELD_NAME__key[] = "key";
static char object_recognition_msgs__msg__ObjectType__FIELD_NAME__db[] = "db";

static rosidl_runtime_c__type_description__Field object_recognition_msgs__msg__ObjectType__FIELDS[] = {
  {
    {object_recognition_msgs__msg__ObjectType__FIELD_NAME__key, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {object_recognition_msgs__msg__ObjectType__FIELD_NAME__db, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
object_recognition_msgs__msg__ObjectType__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {object_recognition_msgs__msg__ObjectType__TYPE_NAME, 38, 38},
      {object_recognition_msgs__msg__ObjectType__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "################################################## OBJECT ID #########################################################\n"
  "\n"
  "# Contains information about the type of a found object. Those two sets of parameters together uniquely define an\n"
  "# object\n"
  "\n"
  "# The key of the found object: the unique identifier in the given db\n"
  "string key\n"
  "\n"
  "# The db parameters stored as a JSON/compressed YAML string. An object id does not make sense without the corresponding\n"
  "# database. E.g., in object_recognition, it can look like: \"{\\'type\\':\\'CouchDB\\', \\'root\\':\\'http://localhost\\'}\"\n"
  "# There is no conventional format for those parameters and it's nice to keep that flexibility.\n"
  "# The object_recognition_core as a generic DB type that can read those fields\n"
  "# Current examples:\n"
  "# For CouchDB:\n"
  "#   type: 'CouchDB'\n"
  "#   root: 'http://localhost:5984'\n"
  "#   collection: 'object_recognition'\n"
  "# For SQL household database:\n"
  "#   type: 'SqlHousehold'\n"
  "#   host: 'wgs36'\n"
  "#   port: 5432\n"
  "#   user: 'willow'\n"
  "#   password: 'willow'\n"
  "#   name: 'household_objects'\n"
  "#   module: 'tabletop'\n"
  "string db";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
object_recognition_msgs__msg__ObjectType__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {object_recognition_msgs__msg__ObjectType__TYPE_NAME, 38, 38},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1044, 1044},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
object_recognition_msgs__msg__ObjectType__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *object_recognition_msgs__msg__ObjectType__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

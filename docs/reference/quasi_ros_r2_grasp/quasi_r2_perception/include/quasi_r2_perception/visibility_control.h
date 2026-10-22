// Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.
#ifndef QUASI_R2_PERCEPTION__VISIBILITY_CONTROL_H_
#define QUASI_R2_PERCEPTION__VISIBILITY_CONTROL_H_

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define QUASI_R2_PERCEPTION_EXPORT __attribute__ ((dllexport))
    #define QUASI_R2_PERCEPTION_IMPORT __attribute__ ((dllimport))
  #else
    #define QUASI_R2_PERCEPTION_EXPORT __declspec(dllexport)
    #define QUASI_R2_PERCEPTION_IMPORT __declspec(dllimport)
  #endif
  #ifdef QUASI_R2_PERCEPTION_BUILDING_LIBRARY
    #define QUASI_R2_PERCEPTION_PUBLIC QUASI_R2_PERCEPTION_EXPORT
  #else
    #define QUASI_R2_PERCEPTION_PUBLIC QUASI_R2_PERCEPTION_IMPORT
  #endif
  #define QUASI_R2_PERCEPTION_PUBLIC_TYPE QUASI_R2_PERCEPTION_PUBLIC
  #define QUASI_R2_PERCEPTION_LOCAL
#else
  #define QUASI_R2_PERCEPTION_EXPORT __attribute__ ((visibility("default")))
  #define QUASI_R2_PERCEPTION_IMPORT
  #if __GNUC__ >= 4
    #define QUASI_R2_PERCEPTION_PUBLIC __attribute__ ((visibility("default")))
    #define QUASI_R2_PERCEPTION_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define QUASI_R2_PERCEPTION_PUBLIC
    #define QUASI_R2_PERCEPTION_LOCAL
  #endif
  #define QUASI_R2_PERCEPTION_PUBLIC_TYPE
#endif

#endif  // QUASI_R2_PERCEPTION__VISIBILITY_CONTROL_H_

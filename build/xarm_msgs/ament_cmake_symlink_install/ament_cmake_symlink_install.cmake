# generated from
# ament_cmake_core/cmake/symlink_install/ament_cmake_symlink_install.cmake.in

# create empty symlink install manifest before starting install step
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/symlink_install_manifest.txt")

#
# Reimplement CMake install(DIRECTORY) command to use symlinks instead of
# copying resources.
#
# :param cmake_current_source_dir: The CMAKE_CURRENT_SOURCE_DIR when install
#   was invoked
# :type cmake_current_source_dir: string
# :param ARGN: the same arguments as the CMake install command.
# :type ARGN: various
#
function(ament_cmake_symlink_install_directory cmake_current_source_dir)
  cmake_parse_arguments(ARG "OPTIONAL" "DESTINATION" "DIRECTORY;PATTERN;PATTERN_EXCLUDE" ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_cmake_symlink_install_directory() called with "
      "unused/unsupported arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  # make destination absolute path and ensure that it exists
  if(NOT IS_ABSOLUTE "${ARG_DESTINATION}")
    set(ARG_DESTINATION "/home/anatoly/ros2k_ws/install/xarm_msgs/${ARG_DESTINATION}")
  endif()
  if(NOT EXISTS "${ARG_DESTINATION}")
    file(MAKE_DIRECTORY "${ARG_DESTINATION}")
  endif()

  # default pattern to include
  if(NOT ARG_PATTERN)
    set(ARG_PATTERN "*")
  endif()

  # iterate over directories
  foreach(dir ${ARG_DIRECTORY})
    # make dir an absolute path
    if(NOT IS_ABSOLUTE "${dir}")
      set(dir "${cmake_current_source_dir}/${dir}")
    endif()

    if(EXISTS "${dir}")
      # if directory has no trailing slash
      # append folder name to destination
      set(destination "${ARG_DESTINATION}")
      string(LENGTH "${dir}" length)
      math(EXPR offset "${length} - 1")
      string(SUBSTRING "${dir}" ${offset} 1 dir_last_char)
      if(NOT dir_last_char STREQUAL "/")
        get_filename_component(destination_name "${dir}" NAME)
        set(destination "${destination}/${destination_name}")
      else()
        # remove trailing slash
        string(SUBSTRING "${dir}" 0 ${offset} dir)
      endif()

      # Create destination directory.
      # This does *not* solve the problem of empty directories WITHIN the install tree,
      # but does make sure that the top-level directory specified by the caller gets created.
      file(MAKE_DIRECTORY "${destination}")

      # glob recursive files
      set(relative_files "")
      foreach(pattern ${ARG_PATTERN})
        file(
          GLOB_RECURSE
          include_files
          RELATIVE "${dir}"
          "${dir}/${pattern}"
        )
        if(NOT include_files STREQUAL "")
          list(APPEND relative_files ${include_files})
        endif()
      endforeach()
      foreach(pattern ${ARG_PATTERN_EXCLUDE})
        file(
          GLOB_RECURSE
          exclude_files
          RELATIVE "${dir}"
          "${dir}/${pattern}"
        )
        if(NOT exclude_files STREQUAL "")
          list(REMOVE_ITEM relative_files ${exclude_files})
        endif()
      endforeach()
      list(SORT relative_files)

      foreach(relative_file ${relative_files})
        set(absolute_file "${dir}/${relative_file}")
        # determine link name for file including destination path
        set(symlink "${destination}/${relative_file}")

        # ensure that destination exists
        get_filename_component(symlink_dir "${symlink}" PATH)
        if(NOT EXISTS "${symlink_dir}")
          file(MAKE_DIRECTORY "${symlink_dir}")
        endif()

        _ament_cmake_symlink_install_create_symlink("${absolute_file}" "${symlink}")
      endforeach()
    else()
      if(NOT ARG_OPTIONAL)
        message(FATAL_ERROR
          "ament_cmake_symlink_install_directory() can't find '${dir}'")
      endif()
    endif()
  endforeach()
endfunction()

#
# Reimplement CMake install(FILES) command to use symlinks instead of copying
# resources.
#
# :param cmake_current_source_dir: The CMAKE_CURRENT_SOURCE_DIR when install
#   was invoked
# :type cmake_current_source_dir: string
# :param ARGN: the same arguments as the CMake install command.
# :type ARGN: various
#
function(ament_cmake_symlink_install_files cmake_current_source_dir)
  cmake_parse_arguments(ARG "OPTIONAL" "DESTINATION;RENAME" "FILES" ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_cmake_symlink_install_files() called with "
      "unused/unsupported arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  # make destination an absolute path and ensure that it exists
  if(NOT IS_ABSOLUTE "${ARG_DESTINATION}")
    set(ARG_DESTINATION "/home/anatoly/ros2k_ws/install/xarm_msgs/${ARG_DESTINATION}")
  endif()
  if(NOT EXISTS "${ARG_DESTINATION}")
    file(MAKE_DIRECTORY "${ARG_DESTINATION}")
  endif()

  if(ARG_RENAME)
    list(LENGTH ARG_FILES file_count)
    if(NOT file_count EQUAL 1)
    message(FATAL_ERROR "ament_cmake_symlink_install_files() called with "
      "RENAME argument but not with a single file")
    endif()
  endif()

  # iterate over files
  foreach(file ${ARG_FILES})
    # make file an absolute path
    if(NOT IS_ABSOLUTE "${file}")
      set(file "${cmake_current_source_dir}/${file}")
    endif()

    if(EXISTS "${file}")
      # determine link name for file including destination path
      get_filename_component(filename "${file}" NAME)
      if(NOT ARG_RENAME)
        set(symlink "${ARG_DESTINATION}/${filename}")
      else()
        set(symlink "${ARG_DESTINATION}/${ARG_RENAME}")
      endif()
      _ament_cmake_symlink_install_create_symlink("${file}" "${symlink}")
    else()
      if(NOT ARG_OPTIONAL)
        message(FATAL_ERROR
          "ament_cmake_symlink_install_files() can't find '${file}'")
      endif()
    endif()
  endforeach()
endfunction()

#
# Reimplement CMake install(PROGRAMS) command to use symlinks instead of copying
# resources.
#
# :param cmake_current_source_dir: The CMAKE_CURRENT_SOURCE_DIR when install
#   was invoked
# :type cmake_current_source_dir: string
# :param ARGN: the same arguments as the CMake install command.
# :type ARGN: various
#
function(ament_cmake_symlink_install_programs cmake_current_source_dir)
  cmake_parse_arguments(ARG "OPTIONAL" "DESTINATION" "PROGRAMS" ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_cmake_symlink_install_programs() called with "
      "unused/unsupported arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  # make destination an absolute path and ensure that it exists
  if(NOT IS_ABSOLUTE "${ARG_DESTINATION}")
    set(ARG_DESTINATION "/home/anatoly/ros2k_ws/install/xarm_msgs/${ARG_DESTINATION}")
  endif()
  if(NOT EXISTS "${ARG_DESTINATION}")
    file(MAKE_DIRECTORY "${ARG_DESTINATION}")
  endif()

  # iterate over programs
  foreach(file ${ARG_PROGRAMS})
    # make file an absolute path
    if(NOT IS_ABSOLUTE "${file}")
      set(file "${cmake_current_source_dir}/${file}")
    endif()

    if(EXISTS "${file}")
      # determine link name for file including destination path
      get_filename_component(filename "${file}" NAME)
      set(symlink "${ARG_DESTINATION}/${filename}")
      _ament_cmake_symlink_install_create_symlink("${file}" "${symlink}")
    else()
      if(NOT ARG_OPTIONAL)
        message(FATAL_ERROR
          "ament_cmake_symlink_install_programs() can't find '${file}'")
      endif()
    endif()
  endforeach()
endfunction()

#
# Reimplement CMake install(TARGETS) command to use symlinks instead of copying
# resources.
#
# :param TARGET_FILES: the absolute files, replacing the name of targets passed
#   in as TARGETS
# :type TARGET_FILES: list of files
# :param ARGN: the same arguments as the CMake install command except that
#   keywords identifying the kind of type and the DESTINATION keyword must be
#   joined with an underscore, e.g. ARCHIVE_DESTINATION.
# :type ARGN: various
#
function(ament_cmake_symlink_install_targets)
  cmake_parse_arguments(ARG "OPTIONAL" "ARCHIVE_DESTINATION;DESTINATION;LIBRARY_DESTINATION;RUNTIME_DESTINATION"
    "TARGETS;TARGET_FILES" ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_cmake_symlink_install_targets() called with "
      "unused/unsupported arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  list(REVERSE ARG_TARGET_FILES)
  list(REMOVE_DUPLICATES ARG_TARGET_FILES)
  list(REVERSE ARG_TARGET_FILES)

  # iterate over target files
  foreach(file ${ARG_TARGET_FILES})
    if(NOT IS_ABSOLUTE "${file}")
      message(FATAL_ERROR "ament_cmake_symlink_install_targets() target file "
        "'${file}' must be an absolute path")
    endif()

    # determine destination of file based on extension
    set(destination "")
    get_filename_component(fileext "${file}" EXT)
    if(fileext STREQUAL ".a" OR fileext STREQUAL ".lib")
      set(destination "${ARG_ARCHIVE_DESTINATION}")
    elseif(fileext MATCHES "(\\.[0-9]+)?(\\.[0-9]+)?(\\.[0-9]+)?\\.dylib$" OR fileext MATCHES "\\.so(\\.[0-9]+)?(\\.[0-9]+)?(\\.[0-9]+)?$")
      set(destination "${ARG_LIBRARY_DESTINATION}")
    elseif(fileext STREQUAL "" OR fileext STREQUAL ".dll" OR fileext STREQUAL ".exe")
      set(destination "${ARG_RUNTIME_DESTINATION}")
    endif()
    if(destination STREQUAL "")
      set(destination "${ARG_DESTINATION}")
    endif()

    # make destination an absolute path and ensure that it exists
    if(NOT IS_ABSOLUTE "${destination}")
      set(destination "/home/anatoly/ros2k_ws/install/xarm_msgs/${destination}")
    endif()
    if(NOT EXISTS "${destination}")
      file(MAKE_DIRECTORY "${destination}")
    endif()

    if(EXISTS "${file}")
      # determine link name for file including destination path
      get_filename_component(filename "${file}" NAME)
      set(symlink "${destination}/${filename}")
      _ament_cmake_symlink_install_create_symlink("${file}" "${symlink}")
    else()
      if(NOT ARG_OPTIONAL)
        message(FATAL_ERROR
          "ament_cmake_symlink_install_targets() can't find '${file}'")
      endif()
    endif()
  endforeach()
endfunction()

function(_ament_cmake_symlink_install_create_symlink absolute_file symlink)
  # register symlink for being removed during install step
  file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/symlink_install_manifest.txt"
    "${symlink}\n")

  # avoid any work if correct symlink is already in place
  if(EXISTS "${symlink}" AND IS_SYMLINK "${symlink}")
    get_filename_component(destination "${symlink}" REALPATH)
    get_filename_component(real_absolute_file "${absolute_file}" REALPATH)
    if(destination STREQUAL real_absolute_file)
      message(STATUS "Up-to-date symlink: ${symlink}")
      return()
    endif()
  endif()

  message(STATUS "Symlinking: ${symlink}")
  if(EXISTS "${symlink}" OR IS_SYMLINK "${symlink}")
    file(REMOVE "${symlink}")
  endif()

  execute_process(
    COMMAND "/usr/bin/cmake" "-E" "create_symlink"
      "${absolute_file}"
      "${symlink}"
  )
  # the CMake command does not provide a return code so check manually
  if(NOT EXISTS "${symlink}" OR NOT IS_SYMLINK "${symlink}")
    get_filename_component(destination "${symlink}" REALPATH)
    message(FATAL_ERROR
      "Could not create symlink '${symlink}' pointing to '${absolute_file}'")
  endif()
endfunction()

# end of template

message(STATUS "Execute custom install script")

# begin of custom install code

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_index/share/ament_index/resource_index/rosidl_interfaces/xarm_msgs" "DESTINATION" "share/ament_index/resource_index/rosidl_interfaces")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_0_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/msg/RobotMsg.json" "DESTINATION" "share/xarm_msgs/msg")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_1_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/msg/IOState.json" "DESTINATION" "share/xarm_msgs/msg")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_2_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/msg/CIOState.json" "DESTINATION" "share/xarm_msgs/msg")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_3_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/msg/MoveVelocity.json" "DESTINATION" "share/xarm_msgs/msg")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_4_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/BioGripperCtrl.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_5_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/BioGripperEnable.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_6_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/Call.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_7_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/GetAnalogIO.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_8_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/GetDigitalIO.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_9_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/GetFloat32.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_10_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/GetFloat32List.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_11_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/GetInt16.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_12_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/GetInt16List.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_13_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/GetInt32.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_14_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/GetInt32ByType.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_15_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/GetSetModbusData.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_16_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/GripperMove.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_17_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/MoveCartesian.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_18_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/MoveCircle.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_19_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/MoveHome.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_20_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/MoveJoint.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_21_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/MoveVelocity.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_22_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/RobotiqActivate.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_23_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/RobotiqGetStatus.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_24_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/RobotiqMove.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_25_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/RobotiqReset.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_26_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/SetAnalogIO.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_27_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/SetDigitalIO.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_28_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/SetFloat32.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_29_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/SetFloat32List.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_30_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/SetInt16.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_31_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/SetInt16ById.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_32_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/SetInt16List.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_33_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/SetInt32.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_34_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/SetInt32ByType.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_35_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/TrajCtrl.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_36_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/TrajPlay.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_37_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/VacuumGripperCtrl.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_38_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/SetTcpLoad.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_39_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/SetModbusTimeout.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_40_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/IdenLoad.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_41_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/FtCaliLoad.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_42_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/FtForceParams.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_43_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/FtAdmittanceParams.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_44_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/LinearMotorBackOrigin.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_45_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/LinearMotorSetPos.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_46_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/PlanExec.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_47_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/PlanJoint.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_48_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/PlanPose.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_49_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/PlanSingleStraight.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_50_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/FtForceConfig.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_51_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/FtForcePid.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_52_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/FtImpedance.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_53_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/LinearTrackBackOrigin.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_54_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_type_description/xarm_msgs/srv/LinearTrackSetPos.json" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_55_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_c/xarm_msgs/" "DESTINATION" "include/xarm_msgs/xarm_msgs" "PATTERN" "*.h")
ament_cmake_symlink_install_directory("/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs" DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_c/xarm_msgs/" "DESTINATION" "include/xarm_msgs/xarm_msgs" "PATTERN" "*.h")

# install(FILES "/opt/ros/kilted/lib/python3.12/site-packages/ament_package/template/environment_hook/library_path.sh" "DESTINATION" "share/xarm_msgs/environment")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_56_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_environment_hooks/library_path.dsv" "DESTINATION" "share/xarm_msgs/environment")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_57_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_typesupport_fastrtps_c/xarm_msgs/" "DESTINATION" "include/xarm_msgs/xarm_msgs" "PATTERN_EXCLUDE" "*.cpp")
ament_cmake_symlink_install_directory("/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs" DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_typesupport_fastrtps_c/xarm_msgs/" "DESTINATION" "include/xarm_msgs/xarm_msgs" "PATTERN_EXCLUDE" "*.cpp")

# install(DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_cpp/xarm_msgs/" "DESTINATION" "include/xarm_msgs/xarm_msgs" "PATTERN" "*.hpp")
ament_cmake_symlink_install_directory("/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs" DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_cpp/xarm_msgs/" "DESTINATION" "include/xarm_msgs/xarm_msgs" "PATTERN" "*.hpp")

# install(DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_typesupport_fastrtps_cpp/xarm_msgs/" "DESTINATION" "include/xarm_msgs/xarm_msgs" "PATTERN_EXCLUDE" "*.cpp")
ament_cmake_symlink_install_directory("/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs" DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_typesupport_fastrtps_cpp/xarm_msgs/" "DESTINATION" "include/xarm_msgs/xarm_msgs" "PATTERN_EXCLUDE" "*.cpp")

# install(DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_typesupport_introspection_c/xarm_msgs/" "DESTINATION" "include/xarm_msgs/xarm_msgs" "PATTERN" "*.h")
ament_cmake_symlink_install_directory("/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs" DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_typesupport_introspection_c/xarm_msgs/" "DESTINATION" "include/xarm_msgs/xarm_msgs" "PATTERN" "*.h")

# install(DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_typesupport_introspection_cpp/xarm_msgs/" "DESTINATION" "include/xarm_msgs/xarm_msgs" "PATTERN" "*.hpp")
ament_cmake_symlink_install_directory("/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs" DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_typesupport_introspection_cpp/xarm_msgs/" "DESTINATION" "include/xarm_msgs/xarm_msgs" "PATTERN" "*.hpp")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_environment_hooks/pythonpath.sh" "DESTINATION" "share/xarm_msgs/environment")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_58_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_environment_hooks/pythonpath.dsv" "DESTINATION" "share/xarm_msgs/environment")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_59_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_python/xarm_msgs/xarm_msgs.egg-info/" "DESTINATION" "lib/python3.12/site-packages/xarm_msgs-0.0.0-py3.12.egg-info")
ament_cmake_symlink_install_directory("/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs" DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_python/xarm_msgs/xarm_msgs.egg-info/" "DESTINATION" "lib/python3.12/site-packages/xarm_msgs-0.0.0-py3.12.egg-info")

# install(DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_py/xarm_msgs/" "DESTINATION" "lib/python3.12/site-packages/xarm_msgs" "PATTERN_EXCLUDE" "*.pyc" "PATTERN_EXCLUDE" "__pycache__")
ament_cmake_symlink_install_directory("/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs" DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_py/xarm_msgs/" "DESTINATION" "lib/python3.12/site-packages/xarm_msgs" "PATTERN_EXCLUDE" "*.pyc" "PATTERN_EXCLUDE" "__pycache__")

# install("TARGETS" "xarm_msgs_s__rosidl_typesupport_fastrtps_c" "DESTINATION" "lib/python3.12/site-packages/xarm_msgs")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_targets_0_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install("TARGETS" "xarm_msgs_s__rosidl_typesupport_introspection_c" "DESTINATION" "lib/python3.12/site-packages/xarm_msgs")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_targets_1_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install("TARGETS" "xarm_msgs_s__rosidl_typesupport_c" "DESTINATION" "lib/python3.12/site-packages/xarm_msgs")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_targets_2_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_rs/xarm_msgs/rust" "DESTINATION" "share/xarm_msgs")
ament_cmake_symlink_install_directory("/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs" DIRECTORY "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_generator_rs/xarm_msgs/rust" "DESTINATION" "share/xarm_msgs")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/msg/RobotMsg.idl" "DESTINATION" "share/xarm_msgs/msg")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_60_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/msg/IOState.idl" "DESTINATION" "share/xarm_msgs/msg")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_61_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/msg/CIOState.idl" "DESTINATION" "share/xarm_msgs/msg")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_62_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/msg/MoveVelocity.idl" "DESTINATION" "share/xarm_msgs/msg")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_63_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/BioGripperCtrl.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_64_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/BioGripperEnable.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_65_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/Call.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_66_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/GetAnalogIO.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_67_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/GetDigitalIO.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_68_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/GetFloat32.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_69_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/GetFloat32List.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_70_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/GetInt16.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_71_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/GetInt16List.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_72_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/GetInt32.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_73_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/GetInt32ByType.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_74_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/GetSetModbusData.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_75_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/GripperMove.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_76_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/MoveCartesian.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_77_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/MoveCircle.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_78_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/MoveHome.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_79_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/MoveJoint.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_80_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/MoveVelocity.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_81_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/RobotiqActivate.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_82_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/RobotiqGetStatus.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_83_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/RobotiqMove.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_84_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/RobotiqReset.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_85_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/SetAnalogIO.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_86_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/SetDigitalIO.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_87_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/SetFloat32.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_88_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/SetFloat32List.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_89_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/SetInt16.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_90_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/SetInt16ById.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_91_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/SetInt16List.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_92_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/SetInt32.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_93_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/SetInt32ByType.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_94_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/TrajCtrl.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_95_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/TrajPlay.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_96_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/VacuumGripperCtrl.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_97_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/SetTcpLoad.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_98_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/SetModbusTimeout.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_99_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/IdenLoad.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_100_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/FtCaliLoad.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_101_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/FtForceParams.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_102_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/FtAdmittanceParams.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_103_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/LinearMotorBackOrigin.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_104_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/LinearMotorSetPos.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_105_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/PlanExec.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_106_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/PlanJoint.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_107_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/PlanPose.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_108_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/PlanSingleStraight.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_109_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/FtForceConfig.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_110_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/FtForcePid.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_111_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/FtImpedance.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_112_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/LinearTrackBackOrigin.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_113_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_adapter/xarm_msgs/srv/LinearTrackSetPos.idl" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_114_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/msg/RobotMsg.msg" "DESTINATION" "share/xarm_msgs/msg")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_115_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/msg/IOState.msg" "DESTINATION" "share/xarm_msgs/msg")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_116_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/msg/CIOState.msg" "DESTINATION" "share/xarm_msgs/msg")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_117_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/msg/MoveVelocity.msg" "DESTINATION" "share/xarm_msgs/msg")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_118_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/BioGripperCtrl.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_119_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/BioGripperEnable.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_120_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/Call.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_121_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/GetAnalogIO.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_122_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/GetDigitalIO.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_123_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/GetFloat32.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_124_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/GetFloat32List.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_125_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/GetInt16.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_126_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/GetInt16List.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_127_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/GetInt32.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_128_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/GetInt32ByType.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_129_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/GetSetModbusData.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_130_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/GripperMove.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_131_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/MoveCartesian.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_132_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/MoveCircle.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_133_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/MoveHome.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_134_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/MoveJoint.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_135_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/MoveVelocity.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_136_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/RobotiqActivate.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_137_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/RobotiqGetStatus.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_138_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/RobotiqMove.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_139_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/RobotiqReset.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_140_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/SetAnalogIO.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_141_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/SetDigitalIO.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_142_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/SetFloat32.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_143_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/SetFloat32List.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_144_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/SetInt16.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_145_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/SetInt16ById.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_146_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/SetInt16List.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_147_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/SetInt32.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_148_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/SetInt32ByType.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_149_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/TrajCtrl.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_150_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/TrajPlay.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_151_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/VacuumGripperCtrl.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_152_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/SetTcpLoad.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_153_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/SetModbusTimeout.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_154_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/IdenLoad.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_155_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/FtCaliLoad.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_156_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/FtForceParams.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_157_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/FtAdmittanceParams.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_158_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/LinearMotorBackOrigin.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_159_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/LinearMotorSetPos.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_160_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/PlanExec.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_161_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/PlanJoint.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_162_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/PlanPose.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_163_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/PlanSingleStraight.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_164_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/FtForceConfig.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_165_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/FtForcePid.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_166_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/FtImpedance.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_167_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/LinearTrackBackOrigin.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_168_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/srv/LinearTrackSetPos.srv" "DESTINATION" "share/xarm_msgs/srv")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_169_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_index/share/ament_index/resource_index/package_run_dependencies/xarm_msgs" "DESTINATION" "share/ament_index/resource_index/package_run_dependencies")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_170_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_index/share/ament_index/resource_index/parent_prefix_path/xarm_msgs" "DESTINATION" "share/ament_index/resource_index/parent_prefix_path")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_171_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/opt/ros/kilted/share/ament_cmake_core/cmake/environment_hooks/environment/ament_prefix_path.sh" "DESTINATION" "share/xarm_msgs/environment")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_172_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_environment_hooks/ament_prefix_path.dsv" "DESTINATION" "share/xarm_msgs/environment")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_173_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/opt/ros/kilted/share/ament_cmake_core/cmake/environment_hooks/environment/path.sh" "DESTINATION" "share/xarm_msgs/environment")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_174_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_environment_hooks/path.dsv" "DESTINATION" "share/xarm_msgs/environment")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_175_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_environment_hooks/local_setup.bash" "DESTINATION" "share/xarm_msgs")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_176_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_environment_hooks/local_setup.sh" "DESTINATION" "share/xarm_msgs")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_177_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_environment_hooks/local_setup.zsh" "DESTINATION" "share/xarm_msgs")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_178_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_environment_hooks/local_setup.dsv" "DESTINATION" "share/xarm_msgs")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_179_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_environment_hooks/package.dsv" "DESTINATION" "share/xarm_msgs")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_180_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_index/share/ament_index/resource_index/packages/xarm_msgs" "DESTINATION" "share/ament_index/resource_index/packages")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_181_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_cmake/rosidl_cmake-extras.cmake" "DESTINATION" "share/xarm_msgs/cmake")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_182_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_export_dependencies/ament_cmake_export_dependencies-extras.cmake" "DESTINATION" "share/xarm_msgs/cmake")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_183_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_export_include_directories/ament_cmake_export_include_directories-extras.cmake" "DESTINATION" "share/xarm_msgs/cmake")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_184_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_export_libraries/ament_cmake_export_libraries-extras.cmake" "DESTINATION" "share/xarm_msgs/cmake")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_185_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_export_targets/ament_cmake_export_targets-extras.cmake" "DESTINATION" "share/xarm_msgs/cmake")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_186_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_cmake/rosidl_cmake_export_typesupport_targets-extras.cmake" "DESTINATION" "share/xarm_msgs/cmake")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_187_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_cmake/rosidl_cmake_export_typesupport_libraries-extras.cmake" "DESTINATION" "share/xarm_msgs/cmake")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_188_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/rosidl_cmake/rosidl_cmake_aggregate_target-extras.cmake" "DESTINATION" "share/xarm_msgs/cmake")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_189_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_core/xarm_msgsConfig.cmake" "/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_core/xarm_msgsConfig-version.cmake" "DESTINATION" "share/xarm_msgs/cmake")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_190_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

# install(FILES "/home/anatoly/ros2k_ws/src/xarm_ros2/xarm_msgs/package.xml" "DESTINATION" "share/xarm_msgs")
include("/home/anatoly/ros2k_ws/build/xarm_msgs/ament_cmake_symlink_install_files_191_${CMAKE_INSTALL_CONFIG_NAME}.cmake")

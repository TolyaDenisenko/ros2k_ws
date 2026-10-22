# Copyright (C) 2017-2023 Quasi, Inc. All Rights Reserved.

import os
import launch.actions
import lifecycle_msgs
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, SetLaunchConfiguration, IncludeLaunchDescription, RegisterEventHandler, EmitEvent
from launch.events import matches_action
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution, ThisLaunchFileDir, EnvironmentVariable
from launch_ros.actions import Node, LifecycleNode
from launch_ros.event_handlers import OnStateTransition
from launch_ros.events.lifecycle import ChangeState


def generate_launch_description():
    
    use_sim_time = LaunchConfiguration('use_sim_time', default='true')
    package_share_dir = get_package_share_directory('quasi_r2_find_grasp_service')
    
    return LaunchDescription([
        DeclareLaunchArgument('use_sim_time', default_value='false', description='Use simulation (Gazebo) clock if true'),
        DeclareLaunchArgument("grasp_config_file", default_value="recognize_pick.yaml"),

        Node(
            package='quasi_r2_find_grasp_service', executable='quasi_r2_find_grasp_service',
            parameters=[
                PathJoinSubstitution([package_share_dir, 'cfg', LaunchConfiguration('grasp_config_file')]),
                {'use_sim_time': use_sim_time}
            ],
            arguments=['--ros-args', '--log-level', 'Info'],
            output='screen')
    ])
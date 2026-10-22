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
    use_sim_time = LaunchConfiguration('use_sim_time', default='false')

    package_share_dir = get_package_share_directory('quasi_r2_perception')
    params_file = PathJoinSubstitution([package_share_dir, 'param', 'perception.yaml'])

    perception_node = LifecycleNode(       # Perception server
        package='quasi_r2_perception',
        executable='perception',
        namespace='', name='perception',
        output='screen',
        arguments=['--ros-args', '--log-level', 'perception:=debug'],
        parameters=[params_file,
                    {'use_sim_time': use_sim_time}]
    )

    return LaunchDescription([
        DeclareLaunchArgument('use_sim_time', default_value='false', description='Use simulation (Gazebo) clock if true'),

        # Openvino detection
        Node(
            package='dynamic_vino_sample', executable='image_object_server',
            name='openvino_image_object_server',
            arguments=['-config', os.path.join(package_share_dir, 'param', 'openvino_object_server.yaml')],
            remappings=[
                ('/openvino_toolkit/image_raw', '/realsense/color/image_raw')
            ],
            output='screen'),

        RegisterEventHandler(
            OnStateTransition(
                target_lifecycle_node=perception_node,
                start_state='configuring', goal_state='unconfigured',
                entities=[launch.actions.Shutdown()]
            )
        ),
        RegisterEventHandler(
            OnStateTransition(
                target_lifecycle_node=perception_node,
                start_state='activating', goal_state='inactive',
                entities=[launch.actions.Shutdown()]
            )
        ),

        RegisterEventHandler(
            OnStateTransition(
                target_lifecycle_node=perception_node,
                start_state='configuring', goal_state='inactive',
                entities=[EmitEvent(event=ChangeState(
                    lifecycle_node_matcher=matches_action(perception_node),
                    transition_id=lifecycle_msgs.msg.Transition.TRANSITION_ACTIVATE
                ))]
            )
        ),

        perception_node,

        EmitEvent(
            event=ChangeState(
                lifecycle_node_matcher=matches_action(perception_node),
                transition_id=lifecycle_msgs.msg.Transition.TRANSITION_CONFIGURE
            ))

    ])

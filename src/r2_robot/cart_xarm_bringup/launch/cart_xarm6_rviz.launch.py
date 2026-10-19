#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
cart_xarm6_rviz.launch.py — только RViz, без Gazebo: модель тележки с рукой
и ползунки суставов (joint_state_publisher_gui). Удобно проверять URDF,
крепление руки и TF без запуска симуляции.

Запуск:
  ros2 launch cart_xarm_bringup cart_xarm6_rviz.launch.py
  ros2 launch cart_xarm_bringup cart_xarm6_rviz.launch.py use_gui:=false
  ros2 launch cart_xarm_bringup cart_xarm6_rviz.launch.py robot_model:=c2l-v3-scaled
"""
import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition, UnlessCondition
from launch.substitutions import Command, FindExecutable, LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue
from ament_index_python.packages import get_package_share_directory

# Та же начальная поза, что и в cart_xarm6_gz.launch.py (рука развёрнута вбок)
ARM_SIDE_POSE = {'joint1': 1.5708, 'joint2': -0.9, 'joint3': -1.3,
                 'joint4': 0.0, 'joint5': 1.0, 'joint6': 0.0}


def generate_launch_description():
    use_gui = LaunchConfiguration('use_gui')
    robot_model = LaunchConfiguration('robot_model')

    cart_xacro = os.path.join(
        get_package_share_directory('quasi_cart_description'),
        'urdf', 'cart_with_arm.urdf.xacro')
    robot_description = {'robot_description': ParameterValue(Command([
        FindExecutable(name='xacro'), ' ', cart_xacro,
        ' use_sim:=false',
        ' robot_model:=', robot_model,
        ' arm_offset_xyz:="', LaunchConfiguration('arm_offset_xyz'), '"',
        ' arm_offset_rpy:="', LaunchConfiguration('arm_offset_rpy'), '"',
    ]), value_type=str)}

    # 'zeros' — стартовые значения ползунков (несмотря на имя параметра)
    jsp_params = [{'zeros': ARM_SIDE_POSE}]

    return LaunchDescription([
        DeclareLaunchArgument('use_gui', default_value='true',
                              description='Окно с ползунками суставов'),
        DeclareLaunchArgument(
            'robot_model', default_value='c2',
            description='Тип тележки: c2 | c2l-v3-scaled'),
        DeclareLaunchArgument(
            'arm_offset_xyz', default_value='0 0 0',
            description='Доп. смещение базы руки от точки крепления тележки, м'),
        DeclareLaunchArgument(
            'arm_offset_rpy', default_value='0 0 0',
            description='Доп. поворот базы руки, рад'),
        Node(package='robot_state_publisher', executable='robot_state_publisher',
             parameters=[robot_description], output='screen'),
        Node(package='joint_state_publisher_gui',
             executable='joint_state_publisher_gui',
             parameters=jsp_params, condition=IfCondition(use_gui)),
        Node(package='joint_state_publisher', executable='joint_state_publisher',
             parameters=jsp_params, condition=UnlessCondition(use_gui)),
        Node(package='rviz2', executable='rviz2', output='screen',
             arguments=['-d', os.path.join(
                 get_package_share_directory('cart_xarm_bringup'),
                 'rviz', 'cart_xarm.rviz')]),
    ])

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
cart_xarm6_gz.launch.py — тележка quasi_cart (r2_robot) + xArm6 с вакуумным
гриппером и камерой D435i на ЭФ, симуляция в Gazebo Sim + RViz.

URDF: quasi_cart_description/urdf/cart_with_arm.urdf.xacro
  Рука закреплена ПО ЦЕНТРУ верхней плоской платформы тележки
  (arm_mount_joint: base_link -> arm_mount_link, xyz="-0.0075 0 0.849").

Запуск:
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py use_moveit:=true
"""
import os

from launch import LaunchDescription
from launch.actions import (DeclareLaunchArgument, ExecuteProcess,
                            IncludeLaunchDescription, TimerAction)
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import (Command, FindExecutable, LaunchConfiguration,
                                  PathJoinSubstitution)
from launch_ros.actions import Node, SetParameter
from launch_ros.parameter_descriptions import ParameterValue
from launch_ros.substitutions import FindPackageShare
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    # ---------- Аргументы ----------
    world_arg = DeclareLaunchArgument(
        'world',
        default_value=os.path.join(
            get_package_share_directory('cart_xarm_bringup'),
            'worlds', 'cart_table_offline.world'),
        description='SDF-мир Gazebo (стол + цилиндрический блок). '
                    'Можно заменить на cart_table.world (нужен Fuel).')
    use_moveit_arg = DeclareLaunchArgument(
        'use_moveit', default_value='false',
        description='Запустить MoveIt (move_group + RViz) для руки на тележке')
    spawn_x_arg = DeclareLaunchArgument('spawn_x', default_value='0.0')
    spawn_y_arg = DeclareLaunchArgument('spawn_y', default_value='0.0')
    spawn_z_arg = DeclareLaunchArgument('spawn_z', default_value='0.07')
    spawn_yaw_arg = DeclareLaunchArgument(
        'spawn_yaw', default_value='0.0',
        description='Ориентация тележки; стол находится при x=+1.7 в мире')

    robot_model = LaunchConfiguration('robot_model', default='c2')
    use_moveit = LaunchConfiguration('use_moveit')

    # ---------- 1. URDF тележки + руки (xacro) ----------
    cart_xacro = os.path.join(
        get_package_share_directory('quasi_cart_description'),
        'urdf', 'cart_with_arm.urdf.xacro')

    robot_description_content = Command([
        FindExecutable(name='xacro'), ' ', cart_xacro,
        ' use_sim:=true',
        ' robot_model:=', robot_model,
        ' simulator:=ignition',
    ])
    robot_description = {
        'robot_description': ParameterValue(
            robot_description_content, value_type=str)}

    # ---------- 2. Gazebo Sim с миром ----------
    gz_sim = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare('ros_gz_sim'), 'launch', 'gz_sim.launch.py'])
        ]),
        launch_arguments={
            'gz_args': ['-r -v 3 ', LaunchConfiguration('world')],
            'on_exit_shutdown': 'true',
        }.items()
    )

    # ---------- 3. robot_state_publisher (тележка + рука) ----------
    rsp = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        parameters=[robot_description, {'use_sim_time': True}],
        output='screen',
    )

    # ---------- 4. Спавн тележки с рукой в сцену ----------
    spawn_robot = Node(
        package='ros_gz_sim',
        executable='create',
        arguments=[
            '-name', 'cart_xarm6',
            '-topic', 'robot_description',
            '-x', LaunchConfiguration('spawn_x'),
            '-y', LaunchConfiguration('spawn_y'),
            '-z', LaunchConfiguration('spawn_z'),
            '-Y', LaunchConfiguration('spawn_yaw'),
        ],
        parameters=[{'use_sim_time': True}],
        output='screen',
    )

    # ---------- 5. Мост Gazebo -> ROS 2 (clock, камера гриппера) ----------
    gz_bridge = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        name='gz_bridge',
        parameters=[{
            'config_file': os.path.join(
                get_package_share_directory('cart_xarm_bringup'),
                'config', 'gz_bridge.yaml'),
            'use_sim_time': True,
        }],
        output='screen',
    )

    # ---------- 6. Контроллеры руки (CM внутри Gazebo, gz_ros2_control) ----------
    spawner_jsb = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['joint_state_broadcaster',
                   '--controller-manager', '/controller_manager'],
        parameters=[{'use_sim_time': True}],
        output='screen',
    )
    spawner_traj = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['xarm6_traj_controller',
                   '--controller-manager', '/controller_manager'],
        parameters=[{'use_sim_time': True}],
        output='screen',
    )

    # ---------- 7. grasp_analyzer (камера гриппера -> варианты захвата) ----------
    # Столешница в frame link_base руки: база руки на z=0.919 (карт),
    # стол в мире на z~0.95 => table_height ~ 0.03 относительно link_base?
    # Нет: grasp_analyzer работает в base_frame=link_base руки, но у нас
    # подвижная база (тележка) — используем map-независимый режим: блокируем
    # параметры позы блока из world-файла (block_x/block_y задаются в link_base).
    grasp_analyzer = Node(
        package='xarm_grasping',
        executable='grasp_analyzer.py',
        name='grasp_analyzer',
        parameters=[
            {'use_sim_time': True},
            {'color_topic': '/camera/color/image_raw'},
            {'points_topic': '/camera/depth/points'},
            {'camera_frame': 'camera_color_optical_frame'},
            {'base_frame': 'link_base'},
            {'analyze_period': 2.0},
        ],
        output='screen',
    )

    # ---------- 8. RViz (базовый, без MoveIt) ----------
    rviz_config = os.path.join(
        get_package_share_directory('cart_xarm_bringup'),
        'rviz', 'cart_xarm.rviz')

    use_rviz = LaunchConfiguration('use_rviz', default='true')
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', rviz_config],
        parameters=[{'use_sim_time': True}],
        output='screen',
        condition=IfCondition(use_rviz),
    )

    # ---------- 9. MoveIt (опционально) ----------
    moveit_common = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare('xarm_moveit_config'),
                'launch', '_robot_moveit_common.launch.py'])
        ]),
        launch_arguments={
            'robot_type': 'xarm',
            'dof': '6',
            'add_gripper': 'false',
            'add_vacuum_gripper': 'true',
            'prefix': '',
            'hw_ns': 'xarm',
            'ros2_control_plugin': 'gz_ros2_control/GazeboSimSystem',
            'controllers_name': 'fake_controllers',
            'moveit_controller_manager_key': 'moveit_simple_controller_manager',
            'moveit_controller_manager_value':
                'moveit_simple_controller_manager/MoveItSimpleControllerManager',
        }.items(),
        condition=IfCondition(use_moveit),
    )

    # ---------- Тайминги ----------
    spawn_delay = TimerAction(period=4.0, actions=[spawn_robot])
    controllers_delay = TimerAction(
        period=7.0, actions=[spawner_jsb, spawner_traj])
    analyzer_delay = TimerAction(period=10.0, actions=[grasp_analyzer])

    # Фиксер use_sim_time для нод внутри include-ов (MoveIt)
    fix_sim_time = TimerAction(
        period=12.0,
        actions=[ExecuteProcess(
            cmd=['bash', '-c', '''
                for attempt in 1 2 3 4 5 6 7 8; do
                    if ros2 node list --no-daemon 2>/dev/null | grep -q "/move_group"; then
                        sleep 2
                        for n in $(ros2 node list --no-daemon 2>/dev/null); do
                            ros2 param set "$n" use_sim_time true >/dev/null 2>&1 || true
                        done
                        echo "[fix_sim_time] use_sim_time=true applied"
                        exit 0
                    fi
                    sleep 2
                done
            '''],
            output='screen',
        )],
        condition=IfCondition(use_moveit),
    )

    return LaunchDescription([
        SetParameter('use_sim_time', True),
        world_arg, use_moveit_arg,
        spawn_x_arg, spawn_y_arg, spawn_z_arg, spawn_yaw_arg,
        DeclareLaunchArgument('use_rviz', default_value='true'),
        DeclareLaunchArgument('robot_model', default_value='c2'),
        gz_sim,
        rsp,
        gz_bridge,
        spawn_delay,
        controllers_delay,
        analyzer_delay,
        rviz_node,
        moveit_common,
        fix_sim_time,
    ])

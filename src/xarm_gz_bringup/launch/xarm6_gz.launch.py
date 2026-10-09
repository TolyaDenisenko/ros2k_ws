#!/usr/bin/env python3
import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node, SetParameter
from launch_ros.substitutions import FindPackageShare
from ament_index_python.packages import get_package_share_directory
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, TimerAction, ExecuteProcess



def generate_launch_description():
    # ---------- Аргументы ----------
    robot_type = LaunchConfiguration('robot_type', default='xarm')
    dof = LaunchConfiguration('dof', default=6)
    add_gripper = LaunchConfiguration('add_gripper', default=False)
    add_vacuum_gripper = LaunchConfiguration('add_vacuum_gripper', default=False)
    add_bio_gripper = LaunchConfiguration('add_bio_gripper', default=False)
    prefix = LaunchConfiguration('prefix', default='')
    hw_ns = LaunchConfiguration('hw_ns', default='xarm')

    # Мир лежит в НАШЕМ пакете (не зависим от мёртвого xarm_gazebo)
    world_file = os.path.join(
        get_package_share_directory('xarm_gz_bringup'), 'worlds', 'table_gz.world')

    # ---------- 1. Gazebo Sim с миром ----------
    gz_sim = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare('ros_gz_sim'),
                'launch',
                'gz_sim.launch.py'
            ])
        ]),
        launch_arguments={
            'gz_args': f'-r -v 4 {world_file}',
            'on_exit_shutdown': 'true'
        }.items()
    )

    # ---------- 2. URDF робота с плагином gz_ros2_control ----------
    robot_description_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare('xarm_description'),
                'launch',
                '_robot_description.launch.py'
            ])
        ]),
        launch_arguments={
            'robot_type': robot_type,
            'dof': dof,
            'add_gripper': add_gripper,
            'add_vacuum_gripper': add_vacuum_gripper,
            'add_bio_gripper': add_bio_gripper,
            'prefix': prefix,
            'hw_ns': hw_ns,
            'ros2_control_plugin': 'gz_ros2_control/GazeboSimSystem',
            'ros2_control_pausable': 'true'
        }.items()
    )

    # ---------- 3. Спавн робота в сцену (поза: на столе) ----------
    spawn_robot = Node(
        package='ros_gz_sim',
        executable='create',
        arguments=[
            '-name', 'xarm6',
            '-topic', 'robot_description',
            '-x', '0', '-y', '-0.84', '-z', '1.0',
        ],
        parameters=[{'use_sim_time': True}],
        output='screen'
    )

    # ---------- 4. Спавн контроллеров (CM живёт внутри Gazebo) ----------
    joint_state_broadcaster_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['joint_state_broadcaster', '--controller-manager', '/controller_manager'],
        parameters=[{'use_sim_time': True}],
        output='screen'
    )

    xarm6_traj_controller_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['xarm6_traj_controller', '--controller-manager', '/controller_manager'],
        parameters=[{'use_sim_time': True}],
        output='screen'
    )

    # ---------- 5. Мост /clock: Gazebo -> ROS 2 (источник сим-времени) ----------
    gz_bridge = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        parameters=[
            {'config_file': os.path.join(
                get_package_share_directory('xarm_gz_bringup'), 'config', 'gz_bridge.yaml')},
            {'use_sim_time': True},
        ],
        output='screen'
    )

    # ---------- 6. MoveIt: move_group + RViz ----------
    moveit_common = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare('xarm_moveit_config'),
                'launch',
                '_robot_moveit_common.launch.py'
            ])
        ]),
        launch_arguments={
            'robot_type': robot_type,
            'dof': dof,
            'add_gripper': add_gripper,
            'add_vacuum_gripper': add_vacuum_gripper,
            'prefix': prefix,
            'hw_ns': hw_ns,
            'ros2_control_plugin': 'gz_ros2_control/GazeboSimSystem',
            'controllers_name': 'fake_controllers',
            'moveit_controller_manager_key': 'moveit_simple_controller_manager',
            'moveit_controller_manager_value': 'moveit_simple_controller_manager/MoveItSimpleControllerManager',
        }.items()
    )

    # ---------- Тайминги ----------
    spawn_delay = TimerAction(period=3.0, actions=[spawn_robot])
    controllers_delay = TimerAction(
        period=5.0,
        actions=[
            joint_state_broadcaster_spawner,
            xarm6_traj_controller_spawner,
        ]
    )
    moveit_delay = TimerAction(period=7.0, actions=[moveit_common])
        # Ноды из чужих include-ов (move_group, rviz2, robot_state_publisher)
    # не получают use_sim_time при старте — принудительно выставляем его после запуска
    fix_sim_time = TimerAction(
        period=12.0,
        actions=[
            ExecuteProcess(
                cmd=['bash', '-c',
                     'for i in 1 2 3; do '
                     'for n in $(ros2 node list --no-daemon 2>/dev/null); do '
                     'ros2 param set "$n" use_sim_time true >/dev/null 2>&1 || true; done; '
                     'sleep 3; done'],
                output='screen',
            )
        ],
    )

    return LaunchDescription([
        # ГЛАВНОЕ: сим-время ВСЕМ нодам дерева (включая те, что внутри include-ов).
        # Без этого MoveIt считает состояния из Gazebo "протухшими" и не исполняет.
        SetParameter('use_sim_time', True),

        # Аргументы
        DeclareLaunchArgument('robot_type', default_value='xarm'),
        DeclareLaunchArgument('dof', default_value='6'),
        DeclareLaunchArgument('add_gripper', default_value='false'),
        DeclareLaunchArgument('add_vacuum_gripper', default_value='false'),
        DeclareLaunchArgument('add_bio_gripper', default_value='false'),
        DeclareLaunchArgument('prefix', default_value=''),
        DeclareLaunchArgument('hw_ns', default_value='xarm'),

        # Сущности
        gz_sim,
        robot_description_launch,
        gz_bridge,
        spawn_delay,
        controllers_delay,
        moveit_delay,
        fix_sim_time,
    ])
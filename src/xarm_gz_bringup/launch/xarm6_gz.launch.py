#!/usr/bin/env python3
import os
from launch import LaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node, SetParameter
from launch_ros.substitutions import FindPackageShare
from ament_index_python.packages import get_package_share_directory
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, TimerAction



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
            'ros2_control_pausable': 'true',
            'xarm_initial_positions': LaunchConfiguration('arm_initial_positions'),
        }.items()
    )

    # ---------- 3. Спавн робота в сцену (поза: на столе) ----------
    spawn_robot = Node(
        package='ros_gz_sim',
        executable='create',
        arguments=[
            '-name', 'xarm6',
            '-topic', 'robot_description',
            '-x', '0', '-y', '-0.84', '-z', '0',  # верх столешницы Fuel Table = 1.0 + 0.03/2; при 1.0 база была внутри стола и контакт расталкивал суставы
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
            # _robot_moveit_common задаёт нодам use_sim_time явно (default False),
            # перекрывая SetParameter — без этого нужен был ручной param set.
            'use_sim_time': 'true',
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
    moveit_delay = TimerAction(period=12.0, actions=[moveit_common])   # было 7.0
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
        # Нулевая поза: фланец в 0.112 м над основанием, присоска 0.126 м —
        # кончик гриппера уходил под столешницу, контакт сбивал суставы и
        # MoveIt-траектории прерывались с PATH_TOLERANCE_VIOLATED.
        # По умолчанию: инструмент вертикально вниз, кончик в ~0.29 м над столом
        # и ~0.31 м впереди основания (над цилиндрическим блоком).
        DeclareLaunchArgument(
            'arm_initial_positions', default_value='0 -0.6 -0.7 0 1.3 0',
            description='Начальные углы joint1..joint6 (рад) через пробел'),

        # Сущности
        gz_sim,
        robot_description_launch,
        gz_bridge,
        spawn_delay,
        controllers_delay,
        moveit_delay,
    ])
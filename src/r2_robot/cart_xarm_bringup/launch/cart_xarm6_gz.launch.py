#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
cart_xarm6_gz.launch.py — тележка quasi_cart (r2_robot) + xArm6 с вакуумным
гриппером и камерой D435i на ЭФ, симуляция в Gazebo Sim + RViz.

URDF: quasi_cart_description/urdf/cart_with_arm.urdf.xacro
  Рука закреплена на верхней плоской платформе тележки
  (arm_mount_joint: base_link -> arm_mount_link). Точка крепления зависит
  от robot_model: c2 — центр (-0.0075 0 0.849), c2l-v3* — (-0.25 0.1295 0.849).

Запуск:
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py use_moveit:=false   # обычный RViz без MoveIt
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py use_moveit:=false use_rviz:=false  # только Gazebo
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py arm_initial_positions:="0 0 0 0 0 0"
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py robot_model:=c2l-v3-scaled
"""
import os

from launch import LaunchDescription
from launch.actions import (DeclareLaunchArgument, IncludeLaunchDescription,
                            TimerAction)
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import (Command, FindExecutable, LaunchConfiguration,
                                  PathJoinSubstitution, PythonExpression)
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
        'use_moveit', default_value='true',
        description='MoveIt (move_group + RViz с панелью MotionPlanning, '
                    'тележкой, маркерами захвата и камерой в одном окне). '
                    'false — обычный RViz без MoveIt')
    spawn_x_arg = DeclareLaunchArgument('spawn_x', default_value='0.0')
    spawn_y_arg = DeclareLaunchArgument('spawn_y', default_value='0.0')
    spawn_z_arg = DeclareLaunchArgument('spawn_z', default_value='0.07')
    spawn_yaw_arg = DeclareLaunchArgument(
        'spawn_yaw', default_value='0.0',
        description='Ориентация тележки; стол находится при x=+1.7 в мире')
    # Начальная поза руки (рад, joint1..joint6) — задаётся через initial_value
    # gz_ros2_control, рука сразу спавнится в ней, а xarm6_traj_controller при
    # активации удерживает текущую позу. По умолчанию: рука развёрнута вбок
    # (+Y тележки, стол — по +X), сложена над платформой, камера смотрит в сторону
    # и ~20° вниз. Нулевая поза выносила гриппер вперёд к краю платформы.
    arm_initial_positions_arg = DeclareLaunchArgument(
        'arm_initial_positions', default_value='1.5708 -0.9 -1.3 0 1.0 0',
        description='Начальные углы joint1..joint6 (рад) через пробел')
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
        ' xarm_initial_positions:="', LaunchConfiguration('arm_initial_positions'), '"',
        ' arm_offset_xyz:="', LaunchConfiguration('arm_offset_xyz'), '"',
        ' arm_offset_rpy:="', LaunchConfiguration('arm_offset_rpy'), '"',
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
    # Суставы приходят из двух источников: рука — /joint_states
    # (joint_state_broadcaster), колёса — /cart/wheel_joint_states (Gazebo).
    # joint_state_publisher сливает их в /merger/joint_states для rsp;
    # суставы без источника публикуются нулями. MoveIt и grasp-ноды
    # по-прежнему читают /joint_states.
    # Нода в namespace merger: её выход joint_states -> /merger/joint_states.
    # Ремапить 'joint_states' нельзя — ремап задевает и подписку на источник
    # /joint_states, и нода слушала саму себя (рука в RViz стояла в нулях).
    jsp = Node(
        package='joint_state_publisher',
        executable='joint_state_publisher',
        name='joint_state_merger',
        namespace='merger',
        parameters=[{
            'source_list': ['/joint_states', '/cart/wheel_joint_states'],
            'rate': 50,
            'use_sim_time': True,
        }],
        # описание робота берём из корневого топика, а не /merger/robot_description
        remappings=[('robot_description', '/robot_description')],
        output='screen',
    )
    rsp = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        parameters=[robot_description, {'use_sim_time': True}],
        remappings=[('joint_states', '/merger/joint_states')],
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
        # С MoveIt открывается его RViz (cart_xarm_moveit.rviz: то же самое +
        # MotionPlanning), второе окно не нужно.
        condition=IfCondition(PythonExpression(
            ["'", use_rviz, "' == 'true' and '", use_moveit, "' != 'true'"])),
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
            # _robot_moveit_common передаёт нодам {'use_sim_time': <arg>} ЯВНО
            # (default False) — это перекрывает глобальный SetParameter, и
            # move_group жил по системному времени: состояния из Gazebo считались
            # устаревшими, траектории не исполнялись. Передаём true напрямую.
            'use_sim_time': 'true',
            # MoveIt строит модель только руки: world -> link_base. Его
            # static_transform_publisher (world -> link_base) отключаем: у нас
            # link_base уже ребёнок arm_mount_link, два родителя ломают TF.
            # Кадр world публикуем ниже как link_base -> world (тождественно).
            'publish_attach_tf': 'false',
            'rviz_config': os.path.join(
                get_package_share_directory('cart_xarm_bringup'),
                'rviz', 'cart_xarm_moveit.rviz'),
            'controllers_name': 'fake_controllers',
            'moveit_controller_manager_key': 'moveit_simple_controller_manager',
            'moveit_controller_manager_value':
                'moveit_simple_controller_manager/MoveItSimpleControllerManager',
        }.items(),
        condition=IfCondition(use_moveit),
    )

    moveit_world_tf = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='moveit_world_tf',
        arguments=['--frame-id', 'link_base', '--child-frame-id', 'world'],
        parameters=[{'use_sim_time': True}],
        condition=IfCondition(use_moveit),
    )

    # ---------- Тайминги ----------
    spawn_delay = TimerAction(period=4.0, actions=[spawn_robot])
    controllers_delay = TimerAction(
        period=7.0, actions=[spawner_jsb, spawner_traj])
    analyzer_delay = TimerAction(period=10.0, actions=[grasp_analyzer])
    # MoveIt — после контроллеров: move_group сразу запрашивает состояние руки,
    # а одновременный старт RViz MoveIt и GUI Gazebo (оба OpenGL) ронял GUI.
    moveit_delay = TimerAction(period=12.0, actions=[moveit_common])

    return LaunchDescription([
        SetParameter('use_sim_time', True),
        world_arg, use_moveit_arg, arm_initial_positions_arg,
        spawn_x_arg, spawn_y_arg, spawn_z_arg, spawn_yaw_arg,
        DeclareLaunchArgument('use_rviz', default_value='true'),
        DeclareLaunchArgument(
            'robot_model', default_value='c2',
            description='Тип тележки (файл quasi_cart_description/urdf/'
                        '<robot_model>.urdf.xacro): c2 | c2l-v3-scaled'),
        DeclareLaunchArgument(
            'arm_offset_xyz', default_value='0 0 0',
            description='Доп. смещение базы руки от точки крепления тележки, м'),
        DeclareLaunchArgument(
            'arm_offset_rpy', default_value='0 0 0',
            description='Доп. поворот базы руки, рад'),
        gz_sim,
        jsp,
        rsp,
        gz_bridge,
        spawn_delay,
        controllers_delay,
        analyzer_delay,
        rviz_node,
        moveit_world_tf,
        moveit_delay,
    ])

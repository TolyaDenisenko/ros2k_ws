#!/usr/bin/env python3
"""
cart_nav_gz.launch.py — навигация тележки c2l-v3 (с рукой) в Gazebo: Nav2 +
slam_toolbox (карта по лидару) + OctoMap по передней камере.

Простой вариант на штатных пакетах Kilted (apt: navigation2, nav2_bringup,
slam_toolbox, octomap_server). Стек тележки quasi_ros_c1 (fuse, STVL, свой BT)
подключим позже.

Состав:
  * cart_xarm6_gz.launch.py — тележка+рука, лидар /scan, ToF /range*_pcl,
    передняя камера /front_camera/... (мир worlds/cart_nav.world: комната 9x7 м);
  * мост config/gz_bridge_cart_nav.yaml — TF одометрии (odom -> base_footprint), /imu;
  * slam_toolbox (config/slam_cart.yaml) — карта /map и map -> odom;
  * Nav2 navigation_launch (config/nav2_cart.yaml) — лидар + облако передней
    камеры в costmap; /cmd_vel -> VelocityControl тележки; рука на время езды
    паркуется (arm_parking);
  * octomap_server — 3D-карта по /front_camera/points в кадре map
    (/occupied_cells_vis_array, /octomap_binary);
  * RViz rviz/cart_nav.rviz — цель кнопкой «Nav2 Goal».

Запуск:
  ros2 launch cart_xarm_bringup cart_nav_gz.launch.py
  ros2 launch cart_xarm_bringup cart_nav_gz.launch.py headless:=true use_octomap:=false
Цель без RViz:
  ros2 action send_goal /navigate_to_pose nav2_msgs/action/NavigateToPose \
      "{pose: {header: {frame_id: map}, pose: {position: {x: 2.0, y: 1.5}, orientation: {w: 1.0}}}}"
Сохранить карту:
  ros2 run nav2_map_server map_saver_cli -f ~/cart_map
"""
import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import (DeclareLaunchArgument, GroupAction, IncludeLaunchDescription,
                            TimerAction)
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node, SetParameter


def generate_launch_description():
    share = get_package_share_directory('cart_xarm_bringup')
    cfg = lambda name: os.path.join(share, 'config', name)  # noqa: E731
    use_rviz = LaunchConfiguration('use_rviz')

    # ---------- 1. Тележка с рукой в мире навигации ----------
    # RViz тележки/MoveIt не открываем — свой RViz навигации ниже.
    # GroupAction (scoped): аргументы include иначе перезаписывают наши
    # LaunchConfiguration — use_rviz:=false выключал и RViz навигации.
    cart = GroupAction([IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(share, 'launch', 'cart_xarm6_gz.launch.py')),
        launch_arguments={
            'world': os.path.join(share, 'worlds', 'cart_nav.world'),
            'use_rviz': 'false',
            'use_moveit': LaunchConfiguration('use_moveit'),
            'headless': LaunchConfiguration('headless'),
            'front_camera': 'true',
            'gripper_camera_preview': LaunchConfiguration('gripper_camera_preview'),
            'spawn_x': LaunchConfiguration('spawn_x'),
            'spawn_y': LaunchConfiguration('spawn_y'),
            'spawn_yaw': LaunchConfiguration('spawn_yaw'),
        }.items())])

    # ---------- 2. TF одометрии и IMU ----------
    nav_bridge = Node(
        package='ros_gz_bridge', executable='parameter_bridge', name='gz_bridge_cart_nav',
        parameters=[{'config_file': cfg('gz_bridge_cart_nav.yaml'), 'use_sim_time': True}],
        output='screen')

    # ---------- 3. SLAM: карта по лидару, map -> odom ----------
    # slam_toolbox в Kilted — lifecycle-нода. autostart штатного launch шлёт
    # configure/activate событием один раз — при одновременном старте десятков нод activate
    # терялся (нода оставалась inactive, map -> odom не появлялся). Поэтому
    # переходами управляет свой lifecycle_manager (как у Nav2).
    slam = GroupAction([
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(
                get_package_share_directory('slam_toolbox'), 'launch', 'online_async_launch.py')),
            launch_arguments={'use_sim_time': 'true', 'use_lifecycle_manager': 'true',
                              'slam_params_file': cfg('slam_cart.yaml')}.items()),
        Node(package='nav2_lifecycle_manager', executable='lifecycle_manager',
             name='lifecycle_manager_slam', output='screen',
             parameters=[{'use_sim_time': True, 'autostart': True, 'bond_timeout': 30.0,
                          'node_names': ['slam_toolbox']}]),
    ])

    # ---------- 4. Nav2 ----------
    # bond_timeout: lifecycle_manager из navigation_launch не читает params_file, а
    # штатных 4 с при одновременном старте всей симуляции не хватало — bringup обрывался.
    nav2 = GroupAction([SetParameter('bond_timeout', 30.0), IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(
            get_package_share_directory('nav2_bringup'), 'launch', 'navigation_launch.py')),
        launch_arguments={
            'use_sim_time': 'true',
            'params_file': cfg('nav2_cart.yaml'),
            'autostart': 'true',
            # без композиции: с ней LoadComposableNodes подвисал на 2-м компоненте
            'use_composition': 'False',
        }.items())])

    # ---------- 5. OctoMap по передней камере ----------
    octomap = Node(
        package='octomap_server', executable='octomap_server_node', name='octomap_server',
        parameters=[{'use_sim_time': True,
                     'frame_id': 'map', 'base_frame_id': 'base_footprint',
                     'resolution': 0.05,
                     'sensor_model.max_range': 3.0,
                     # пол и потолок комнаты не нужны
                     'point_cloud_min_z': 0.05, 'point_cloud_max_z': 1.6,
                     'occupancy_min_z': 0.05, 'occupancy_max_z': 1.6}],
        remappings=[('cloud_in', '/front_camera/points')],
        condition=IfCondition(LaunchConfiguration('use_octomap')),
        output='screen')

    rviz = Node(
        package='rviz2', executable='rviz2', name='rviz2_nav',
        arguments=['-d', os.path.join(share, 'rviz', 'cart_nav.rviz')],
        parameters=[{'use_sim_time': True}],
        condition=IfCondition(use_rviz), output='screen')

    return LaunchDescription([
        DeclareLaunchArgument('use_rviz', default_value='true'),
        DeclareLaunchArgument(
            'use_moveit', default_value='false',
            description='MoveIt для руки (тяжело; для езды не нужен)'),
        DeclareLaunchArgument('headless', default_value='false'),
        DeclareLaunchArgument('use_octomap', default_value='true'),
        DeclareLaunchArgument(
            'gripper_camera_preview', default_value='true',
            description='камера гриппера 640x400 — при езде она не нужна, экономим GPU/CPU'),
        DeclareLaunchArgument('spawn_x', default_value='-2.0'),
        DeclareLaunchArgument('spawn_y', default_value='0.0'),
        DeclareLaunchArgument('spawn_yaw', default_value='0.0'),
        cart,
        # после спавна тележки (cart_xarm6_gz: спавн через 4 с)
        TimerAction(period=6.0, actions=[nav_bridge]),
        TimerAction(period=10.0, actions=[slam, octomap, rviz]),
        # Nav2 ждёт map -> base_footprint: после первой карты slam_toolbox
        TimerAction(period=30.0, actions=[nav2]),
    ])

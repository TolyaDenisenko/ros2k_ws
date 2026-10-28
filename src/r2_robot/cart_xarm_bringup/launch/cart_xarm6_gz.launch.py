#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
cart_xarm6_gz.launch.py — тележка quasi_cart (r2_robot) + xArm6 с вакуумным
гриппером и камерой на ЭФ (по умолчанию Orbbec Gemini 335Le, camera_model:=d435i —
старая RealSense), симуляция в Gazebo Sim + RViz. Вакуум в симуляции —
DetachableJoint + нода vacuum_gripper_sim (сервис /vacuum_gripper/set).
Документация и команды проверки: docs/STATUS.md.

URDF: quasi_cart_description/urdf/cart_with_arm.urdf.xacro
  Рука закреплена на верхней плоской платформе тележки
  (arm_mount_joint: base_link -> arm_mount_link). Точка крепления зависит
  от robot_model: c2 — центр (-0.0075 0 0.849), c2l-v3 — (-0.25 -0.1295 0.849).

Запуск:
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py use_moveit:=false   # обычный RViz без MoveIt
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py use_moveit:=false use_rviz:=false  # только Gazebo
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py arm_initial_positions:="0 0 0 0 0 0"
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py robot_model:=c2l-v3
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py headless:=true                   # Gazebo без GUI
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py camera_model:=d435i               # старая камера
  ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py gripper_camera_preview:=true     # лёгкий режим 640x400
"""
import os

from launch import LaunchDescription
from launch.actions import (DeclareLaunchArgument, IncludeLaunchDescription,
                            TimerAction)
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import (Command, FindExecutable, LaunchConfiguration,
                                  PathJoinSubstitution, PythonExpression)
from launch_ros.actions import ComposableNodeContainer, Node, SetParameter
from launch_ros.descriptions import ComposableNode
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
    spawn_x_arg = DeclareLaunchArgument('spawn_x', default_value='-0.5')
    spawn_y_arg = DeclareLaunchArgument('spawn_y', default_value='0.0')
    spawn_z_arg = DeclareLaunchArgument('spawn_z', default_value='0.07')
    spawn_yaw_arg = DeclareLaunchArgument(
        'spawn_yaw', default_value='0.0',
        description='Ориентация тележки, рад; стол в мире при x=1.05 (верх z=0.925)')
    # Начальная поза руки (рад, joint1..joint6) — задаётся через initial_value
    # gz_ros2_control, рука сразу спавнится в ней, а xarm6_traj_controller при
    # активации удерживает текущую позу. По умолчанию: рука развёрнута вбок
    # (+Y тележки, стол — по +X), сложена над платформой, камера смотрит в сторону
    # и ~20° вниз. Нулевая поза выносила гриппер вперёд к краю платформы.
    arm_initial_positions_arg = DeclareLaunchArgument(
        'arm_initial_positions', default_value='1.5708 -0.9 -1.3 0 1.0 0',
        description='Начальные углы joint1..joint6 (рад) через пробел')
    robot_model = LaunchConfiguration('robot_model', default='c2l-v3')
    use_moveit = LaunchConfiguration('use_moveit')
    camera_model = LaunchConfiguration('camera_model')
    # Условия «какая камера»: gemini335le (Orbbec, quasi_camera_description) или
    # d435i (старая RealSense из xarm_description, топики /camera/...).
    is_orbbec = IfCondition(PythonExpression(["'", camera_model, "' != 'd435i'"]))
    is_d435i = IfCondition(PythonExpression(["'", camera_model, "' == 'd435i'"]))
    front_camera = LaunchConfiguration('front_camera')

    # ---------- 1. URDF тележки + руки (xacro) ----------
    cart_xacro = os.path.join(
        get_package_share_directory('quasi_cart_description'),
        'urdf', 'cart_with_arm.urdf.xacro')

    robot_description_content = Command([
        FindExecutable(name='xacro'), ' ', cart_xacro,
        ' use_sim:=true',
        ' camera_model:=', camera_model,
        ' gripper_camera_preview:=', LaunchConfiguration('gripper_camera_preview'),
        ' robot_model:=', robot_model,
        ' front_camera:=', front_camera,
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
            # headless:=true — только сервер Gazebo без GUI (-s) и рендер без окна
            # (для проверок по SSH/CI; RViz при этом работает как обычно)
            'gz_args': [PythonExpression(
                ["'-r -v 3 -s --headless-rendering ' if '",
                 LaunchConfiguration('headless'), "' == 'true' else '-r -v 3 '"]),
                LaunchConfiguration('world')],
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

    # ---------- 5. Мосты Gazebo <-> ROS 2 ----------
    # 5a. Общий: /clock, колёса, езда, вакуум (/vacuum/*), позы мира (/gz/world_poses).
    bridge_cfg = lambda name: os.path.join(  # noqa: E731
        get_package_share_directory('cart_xarm_bringup'), 'config', name)
    gz_bridge = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        name='gz_bridge',
        parameters=[{'config_file': bridge_cfg('gz_bridge.yaml'),
                     'use_sim_time': True}],
        output='screen',
    )
    # Прореживание часов: /clock_gz (1000 Гц, каждый шаг физики) -> /clock 200 Гц.
    # Самый частый таймер у нод — 50 Гц (collision_monitor), запаса хватает.
    # Физику и ros2_control (живёт внутри gz) это не касается. Сама нода —
    # на wall time: её use_sim_time ждал бы /clock, который она же публикует
    # (явный False перекрывает глобальный SetParameter(use_sim_time) этого launch).
    clock_throttle = Node(
        package='topic_tools', executable='throttle', name='clock_throttle',
        arguments=['messages', '/clock_gz', '200.0', '/clock'],
        parameters=[{'use_sim_time': False}],
        output='screen',
    )
    # 5b. Камера гриппера: Orbbec -> /gripper_camera/{color,aligned_depth}/...;
    #     D435i -> /camera/... (отдельные yaml, чтобы не смешивать интерфейсы).
    gz_bridge_orbbec = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        name='gz_bridge_gripper_camera',
        parameters=[{'config_file': bridge_cfg('gz_bridge_gripper_camera.yaml'),
                     'use_sim_time': True}],
        output='screen',
        condition=is_orbbec,
    )
    gz_bridge_d435i = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        name='gz_bridge_d435i',
        parameters=[{'config_file': bridge_cfg('gz_bridge_d435i.yaml'),
                     'use_sim_time': True}],
        output='screen',
        condition=is_d435i,
    )

    # 5c. Облако точек камеры Orbbec из цвета + выровненной глубины (как на железе
    #     строит драйвер): /gripper_camera/points в gripper_camera_color_optical_frame.
    #     gz-облако rgbd_camera не используем (его система координат зависит от
    #     версии gz-sensors). Отбрасывает inf-точки вне [near, far] автоматически.
    camera_points = ComposableNodeContainer(
        name='gripper_camera_proc', namespace='',
        package='rclcpp_components', executable='component_container_mt',
        composable_node_descriptions=[ComposableNode(
            package='depth_image_proc',
            plugin='depth_image_proc::PointCloudXyzrgbNode',
            name='gripper_camera_points',
            remappings=[
                ('rgb/image_rect_color', '/gripper_camera/color/image_raw'),
                ('rgb/camera_info', '/gripper_camera/color/camera_info'),
                ('depth_registered/image_rect', '/gripper_camera/aligned_depth/image_raw'),
                ('points', '/gripper_camera/points'),
            ],
            parameters=[{'use_sim_time': True, 'queue_size': 5}],
        )],
        output='screen',
        condition=is_orbbec,
    )

    # 5e. Навигационные датчики тележки: лидар /scan, ToF /rangeN_pcl
    #     (gz_bridge_cart_sensors.yaml) и передняя камера /front_camera/...
    #     (облако /front_camera/points — тем же depth_image_proc, что у гриппера).
    gz_bridge_cart_sensors = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        name='gz_bridge_cart_sensors',
        parameters=[{'config_file': bridge_cfg('gz_bridge_cart_sensors.yaml'),
                     'use_sim_time': True}],
        output='screen',
    )
    # Стойки тележки в поле лидара: /scan_raw -> /scan без секторов стоек
    # (на железе — blackout_angles драйвера ydlidar, config/lidar_blackout.yaml).
    lidar_blackout_filter = Node(
        package='cart_xarm_bringup',
        executable='lidar_blackout_filter.py',
        name='lidar_blackout_filter',
        parameters=[bridge_cfg('lidar_blackout.yaml'), {'use_sim_time': True}],
        output='screen',
    )
    gz_bridge_front_camera = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        name='gz_bridge_front_camera',
        parameters=[{'config_file': bridge_cfg('gz_bridge_front_camera.yaml'),
                     'use_sim_time': True}],
        output='screen',
        condition=IfCondition(front_camera),
    )
    front_camera_points = ComposableNodeContainer(
        name='front_camera_proc', namespace='',
        package='rclcpp_components', executable='component_container_mt',
        composable_node_descriptions=[ComposableNode(
            package='depth_image_proc',
            plugin='depth_image_proc::PointCloudXyzrgbNode',
            name='front_camera_points',
            remappings=[
                ('rgb/image_rect_color', '/front_camera/color/image_raw'),
                ('rgb/camera_info', '/front_camera/color/camera_info'),
                ('depth_registered/image_rect', '/front_camera/aligned_depth/image_raw'),
                ('points', '/front_camera/points'),
            ],
            parameters=[{'use_sim_time': True, 'queue_size': 5}],
        )],
        output='screen',
        condition=IfCondition(front_camera),
    )

    # 5h. Кнопка вакуума в RViz (интерактивный маркер у присоски, /vacuum_button)
    vacuum_button = Node(
        package='xarm_grasping',
        executable='vacuum_rviz_button.py',
        name='vacuum_rviz_button',
        parameters=[{'use_sim_time': True}],
        output='screen',
    )

    # 5f. Истинная сцена Gazebo в RViz (/scene_markers): стол, pick_box,
    #     cylinder_block — для сравнения с маркерами grasp_analyzer.
    scene_markers = Node(
        package='xarm_grasping',
        executable='scene_markers.py',
        name='scene_markers',
        parameters=[{'use_sim_time': True}],
        output='screen',
    )

    # 5g. Парковка руки (имитация тормозов): пока тележка едет (/cmd_vel != 0),
    #     траекторный контроллер выключен — рука держит позу, команды движения
    #     отклоняются. Вручную: сервис /arm_parking/set (SetBool).
    arm_parking = Node(
        package='xarm_grasping',
        executable='arm_parking.py',
        name='arm_parking',
        parameters=[{'use_sim_time': True}],
        output='screen',
    )

    # 5d. Вакуум в симуляции: сервис /vacuum_gripper/set (SetBool) проверяет касание
    #     присоски с объектом и шлёт attach/detach в плагин DetachableJoint.
    vacuum_sim = Node(
        package='xarm_grasping',
        executable='vacuum_gripper_sim.py',
        name='vacuum_gripper_sim',
        # если всё же завершится — перезапустить (кнопка RViz без неё не работает)
        respawn=True, respawn_delay=2.0,
        parameters=[{'use_sim_time': True,
                     'robot_model_name': 'cart_xarm6',
                     'object_names': ['pick_box'],
                     'object_sizes': [0.10, 0.05, 0.05]}],
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
    # grasp_analyzer работает в base_frame=link_base. Значения ниже — для
    # дефолтной сцены: c2l-v3, спавн (-0.3,0), тележка оседает до z≈0.061
    # => link_base в мире ≈ (-0.54, -0.135, 0.910) (замер /gz/robot_pose + TF).
    #   table_height = 0.925 (верх стола) - 0.910 = 0.015
    #   block_x/y    = pick_box (0.60, 0) - link_base  = (1.14, 0.135)
    # При другой тележке/спавне пересчитать (block_* — только запасная поза,
    # если облако точек не дало объект).
    grasp_analyzer = Node(
        package='xarm_grasping',
        executable='grasp_analyzer.py',
        name='grasp_analyzer',
        parameters=[
            {'use_sim_time': True},
            # Топики/фрейм зависят от камеры (Orbbec — единый интерфейс docs/01 §5)
            {'color_topic': PythonExpression(
                ["'/camera/color/image_raw' if '", camera_model, "' == 'd435i' "
                 "else '/gripper_camera/color/image_raw'"])},
            {'points_topic': PythonExpression(
                ["'/camera/depth/points' if '", camera_model, "' == 'd435i' "
                 "else '/gripper_camera/points'"])},
            {'camera_frame': PythonExpression(
                ["'camera_color_optical_frame' if '", camera_model, "' == 'd435i' "
                 "else 'gripper_camera_color_optical_frame'"])},
            {'base_frame': 'link_base'},
            {'analyze_period': 2.0},
            {'table_height': 0.015},
            {'block_x': 1.14},
            {'block_y': 0.135},
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
            # камера гриппера в модели MoveIt: планировщик учитывает её корпус
            # (только Orbbec из quasi_camera_description; D435i — без камеры)
            'add_gripper_camera': PythonExpression(
                ["'false' if '", camera_model, "' == 'd435i' else 'true'"]),
            'gripper_camera_model': camera_model,
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
    # после спавна контроллеров (иначе нечего выключать)
    parking_delay = TimerAction(period=9.0, actions=[arm_parking])
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
            'headless', default_value='false',
            description='true — Gazebo без GUI (только сервер, для проверок/CI)'),
        DeclareLaunchArgument(
            'camera_model', default_value='gemini335le',
            description='Камера на фланце: gemini335le (Orbbec 335Le) | d435i (старая)'),
        DeclareLaunchArgument(
            'gripper_camera_preview', default_value='false',
            description='true — лёгкий режим камеры 640x400 вместо 1280x800 '
                        '(для отладки движений при низком RTF)'),
        DeclareLaunchArgument(
            'robot_model', default_value='c2l-v3',
            description='Тип тележки (файл quasi_cart_description/urdf/'
                        '<robot_model>.urdf.xacro): c2 | c2l-v3'),
        DeclareLaunchArgument(
            'front_camera', default_value='true',
            description='Передняя камера тележки Orbbec 335Le (/front_camera/..., '
                        '640x400). false — меньше нагрузка на GPU'),
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
        clock_throttle,
        gz_bridge_orbbec,
        gz_bridge_d435i,
        camera_points,
        gz_bridge_cart_sensors,
        lidar_blackout_filter,
        gz_bridge_front_camera,
        front_camera_points,
        vacuum_sim,
        scene_markers,
        vacuum_button,
        spawn_delay,
        controllers_delay,
        parking_delay,
        analyzer_delay,
        rviz_node,
        moveit_world_tf,
        moveit_delay,
    ])

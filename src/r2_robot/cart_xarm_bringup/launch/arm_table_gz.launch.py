#!/usr/bin/env python3
"""
arm_table_gz.launch.py — рука xArm6 на столе (без тележки): отладка захвата
вакуумной присоской и обучение, без подъезда тележки.

Та же сборка руки, что на тележке (arm_on_table.urdf.xacro -> cart-arm.xacro):
присоска, камера гриппера Orbbec (/gripper_camera/...), вакуум DetachableJoint
на каждый объект из vacuum_targets. Мир: worlds/arm_table.world (стол, верх
z=0.75; pick_box и cylinder_block в 0.35–0.45 м перед базой).

Вакуум (vacuum_gripper_sim):
  кнопка в RViz — кубик у присоски (инструмент Interact): клик вкл/выкл;
  ros2 topic pub --once /vacuum_gripper/cmd std_msgs/msg/Bool "{data: true}"
  ros2 service call /vacuum_gripper/set std_srvs/srv/SetBool "{data: true}"
  состояние: /vacuum_gripper/state, /vacuum_gripper/held, /vacuum_gripper/status
  можно ли брать: /vacuum_gripper/ready (Bool), /vacuum_gripper/contact (JSON),
  подсветка цели в RViz: /vacuum_gripper/markers; кнопка — светофор

Что на столе и куда ставить (docs/PICK_PLACE.md):
  ros2 topic echo /objects                      # объекты: класс, форма, поза
  ros2 action send_goal /pick_place xarm_grasping_msgs/action/PickPlace \
      "{object_id: pick_box_0}" --feedback      # взять и поставить в зону

Запуск:
  ros2 launch cart_xarm_bringup arm_table_gz.launch.py
  ros2 launch cart_xarm_bringup arm_table_gz.launch.py headless:=true use_moveit:=false
"""
import os

import yaml

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, TimerAction
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import (Command, FindExecutable, LaunchConfiguration,
                                  PathJoinSubstitution, PythonExpression)
from launch_ros.actions import ComposableNodeContainer, Node, SetParameter
from launch_ros.descriptions import ComposableNode
from launch_ros.parameter_descriptions import ParameterValue
from launch_ros.substitutions import FindPackageShare

# Верх столешницы в мире Gazebo = высота спавна базы руки (arm_table.world)
TABLE_TOP_Z = '0.75'
# Объекты для присоски: config/arm_table_objects.yaml (единый список; мир и мост
# генерирует tools/gen_arm_table_scene.py). (имя, форма, размер X Y Z — у
# цилиндра описанный бокс 2r 2r h)
def load_objects():
    path = os.path.join(get_package_share_directory('cart_xarm_bringup'),
                        'config', 'arm_table_objects.yaml')
    with open(path) as f:
        objs = yaml.safe_load(f)['objects']
    return [(o['name'], o['shape'],
             list(o['size']) if o['shape'] == 'box'
             else [2 * o['size'][0], 2 * o['size'][0], o['size'][1]]) for o in objs]


def generate_launch_description():
    share = get_package_share_directory('cart_xarm_bringup')
    OBJECTS = load_objects()
    use_moveit = LaunchConfiguration('use_moveit')
    use_rviz = LaunchConfiguration('use_rviz')
    names = [o[0] for o in OBJECTS]
    sizes = [v for o in OBJECTS for v in o[2]]

    # ---------- 1. URDF руки на столе ----------
    robot_description = {'robot_description': ParameterValue(Command([
        FindExecutable(name='xacro'), ' ',
        os.path.join(get_package_share_directory('quasi_cart_description'),
                     'urdf', 'arm_on_table.urdf.xacro'),
        ' use_sim:=true',
        ' camera_model:=gemini335le',
        ' gripper_camera_preview:=', LaunchConfiguration('gripper_camera_preview'),
        ' vacuum_targets:="', ' '.join(names), '"',
        ' xarm_initial_positions:="', LaunchConfiguration('arm_initial_positions'), '"',
    ]), value_type=str)}

    # ---------- 2. Gazebo ----------
    gz_sim = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([PathJoinSubstitution([
            FindPackageShare('ros_gz_sim'), 'launch', 'gz_sim.launch.py'])]),
        launch_arguments={
            'gz_args': [PythonExpression(
                ["'-r -v 3 -s --headless-rendering ' if '",
                 LaunchConfiguration('headless'), "' == 'true' else '-r -v 3 '"]),
                os.path.join(share, 'worlds', 'arm_table.world')],
            'on_exit_shutdown': 'true',
        }.items())

    # ---------- 3. robot_state_publisher и спавн (база на столешнице) ----------
    rsp = Node(
        package='robot_state_publisher', executable='robot_state_publisher',
        parameters=[robot_description, {'use_sim_time': True}], output='screen')
    spawn_robot = Node(
        package='ros_gz_sim', executable='create',
        arguments=['-name', 'xarm6_table', '-topic', 'robot_description',
                   '-x', '0', '-y', '0', '-z', TABLE_TOP_Z],
        parameters=[{'use_sim_time': True}], output='screen')

    # ---------- 4. Мосты: clock, вакуум, позы; камера гриппера ----------
    bridge_cfg = lambda name: os.path.join(share, 'config', name)  # noqa: E731
    gz_bridge = Node(
        package='ros_gz_bridge', executable='parameter_bridge', name='gz_bridge',
        parameters=[{'config_file': bridge_cfg('gz_bridge_arm_table.yaml'),
                     'use_sim_time': True}], output='screen')
    gz_bridge_camera = Node(
        package='ros_gz_bridge', executable='parameter_bridge',
        name='gz_bridge_gripper_camera',
        parameters=[{'config_file': bridge_cfg('gz_bridge_gripper_camera.yaml'),
                     'use_sim_time': True}], output='screen')
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
        )], output='screen')

    # ---------- 5. Контроллеры руки ----------
    spawners = [Node(
        package='controller_manager', executable='spawner',
        arguments=[c, '--controller-manager', '/controller_manager'],
        parameters=[{'use_sim_time': True}], output='screen')
        for c in ('joint_state_broadcaster', 'xarm6_traj_controller')]

    # ---------- 6. Вакуум, кнопка RViz, сцена, анализатор ----------
    # Корень модели — link «world» URDF (= link_base), его поза в gz = точка спавна.
    vacuum_sim = Node(
        package='xarm_grasping', executable='vacuum_gripper_sim.py',
        name='vacuum_gripper_sim',
        # если всё же завершится — перезапустить (кнопка RViz без неё не работает)
        respawn=True, respawn_delay=2.0,
        parameters=[{'use_sim_time': True, 'robot_model_name': 'xarm6_table',
                     'root_frame': 'world', 'object_names': names,
                     'object_sizes': sizes,
                     'object_shapes': [o[1] for o in OBJECTS]}], output='screen')
    vacuum_button = Node(
        package='xarm_grasping', executable='vacuum_rviz_button.py',
        name='vacuum_rviz_button', parameters=[{'use_sim_time': True}], output='screen')
    scene_markers = Node(
        package='xarm_grasping', executable='scene_markers.py', name='scene_markers',
        parameters=[{'use_sim_time': True, 'robot_model_name': 'xarm6_table',
                     'root_frame': 'world', 'object_names': names,
                     'object_shapes': [o[1] for o in OBJECTS], 'object_sizes': sizes,
                     'static_cylinder': False,
                     # центр столешницы в мире gz (arm_table.world)
                     'table_pose': [0.3, 0.0, 0.725], 'table_size': [1.0, 1.2, 0.05]}],
        output='screen')
    # base_frame = link_base: верх стола z=0 (база стоит на столешнице)
    grasp_analyzer = Node(
        package='xarm_grasping', executable='grasp_analyzer.py', name='grasp_analyzer',
        parameters=[{'use_sim_time': True,
                     'color_topic': '/gripper_camera/color/image_raw',
                     'points_topic': '/gripper_camera/points',
                     'camera_frame': 'gripper_camera_color_optical_frame',
                     'base_frame': 'link_base', 'analyze_period': 2.0,
                     'table_height': 0.0, 'block_x': 0.35, 'block_y': -0.20}],
        output='screen')

    # ---------- 6b. Что на столе и куда ставить: восприятие, сцена MoveIt, pick&place ----
    # object_perception: /objects (класс, форма, поза), action /detect_objects;
    # scene_manager: Planning Scene + зоны (config/place_zones.yaml), /find_place_pose;
    # pick_place_server: action /pick_place (нужен MoveIt: use_moveit:=true).
    object_perception = Node(
        package='xarm_grasping', executable='object_perception.py', name='object_perception',
        parameters=[{'use_sim_time': True, 'base_frame': 'link_base', 'table_height': 0.0}],
        output='screen')
    scene_manager = Node(
        package='xarm_grasping', executable='scene_manager.py', name='scene_manager',
        parameters=[{'use_sim_time': True, 'base_frame': 'link_base',
                     'table_center': [0.3, 0.0], 'table_size': [1.0, 1.2]}],
        output='screen')
    pick_place = Node(
        package='xarm_grasping', executable='pick_place_server.py', name='pick_place_server',
        parameters=[{'use_sim_time': True}], output='screen',
        condition=IfCondition(use_moveit))

    # ---------- 7. RViz / MoveIt ----------
    # Конфиги RViz общие с тележкой (Fixed Frame base_link есть и здесь;
    # дисплеи тележки просто пустые).
    rviz = Node(
        package='rviz2', executable='rviz2', name='rviz2',
        arguments=['-d', os.path.join(share, 'rviz', 'cart_xarm.rviz')],
        parameters=[{'use_sim_time': True}], output='screen',
        condition=IfCondition(PythonExpression(
            ["'", use_rviz, "' == 'true' and '", use_moveit, "' != 'true'"])))
    moveit = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([PathJoinSubstitution([
            FindPackageShare('xarm_moveit_config'), 'launch',
            '_robot_moveit_common.launch.py'])]),
        launch_arguments={
            'robot_type': 'xarm', 'dof': '6', 'add_gripper': 'false',
            'add_vacuum_gripper': 'true', 'prefix': '', 'hw_ns': 'xarm',
            'ros2_control_plugin': 'gz_ros2_control/GazeboSimSystem',
            'use_sim_time': 'true',
            # world -> link_base уже в URDF (тождественно) — второй издатель не нужен
            'publish_attach_tf': 'false',
            'use_rviz': use_rviz,
            'rviz_config': os.path.join(share, 'rviz', 'cart_xarm_moveit.rviz'),
            'controllers_name': 'fake_controllers',
            'moveit_controller_manager_key': 'moveit_simple_controller_manager',
            'moveit_controller_manager_value':
                'moveit_simple_controller_manager/MoveItSimpleControllerManager',
        }.items(),
        condition=IfCondition(use_moveit))

    return LaunchDescription([
        SetParameter('use_sim_time', True),
        DeclareLaunchArgument('use_moveit', default_value='true'),
        DeclareLaunchArgument('use_rviz', default_value='true'),
        DeclareLaunchArgument(
            'headless', default_value='false',
            description='true — Gazebo без GUI (сервер + рендер без окна)'),
        DeclareLaunchArgument(
            'gripper_camera_preview', default_value='false',
            description='true — камера гриппера 640x400 вместо 1280x800'),
        # Старт = поза наблюдения pick_place_server (присоска вниз в (0.30, 0, 0.40)
        # над столом, камера ~0.47 м — видны все объекты и обе зоны; IK из
        # arm_goto_tcp.solve_ik). Объекты распознаются сразу после запуска.
        DeclareLaunchArgument(
            'arm_initial_positions', default_value='0 -0.596 -1.013 0 1.609 0',
            description='Начальные углы joint1..joint6 (рад) через пробел'),
        gz_sim, rsp, gz_bridge, gz_bridge_camera, camera_points,
        TimerAction(period=3.0, actions=[spawn_robot]),
        TimerAction(period=6.0, actions=spawners),
        TimerAction(period=8.0, actions=[vacuum_sim, vacuum_button, scene_markers]),
        TimerAction(period=10.0, actions=[grasp_analyzer, object_perception, scene_manager]),
        TimerAction(period=16.0, actions=[pick_place]),
        TimerAction(period=10.0, actions=[rviz]),
        TimerAction(period=12.0, actions=[moveit]),
    ])

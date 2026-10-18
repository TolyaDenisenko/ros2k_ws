"""
Запуск анализатора захватов камерой гриппера + RViz-маркеры.

    ros2 launch xarm_grasping grasp_analyzer.launch.py

Сначала запустите симуляцию (xarm_gz_bringup или cart_xarm_bringup),
иначе у узла не будет ни толиков камеры, ни TF.
"""
import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    default_params = os.path.join(
        get_package_share_directory('xarm_grasping'),
        'config', 'grasp_params.yaml')

    return LaunchDescription([
        DeclareLaunchArgument('use_sim_time', default_value='true',
                              description='Синхронизация со /clock из Gazebo (gz-sim)'),
        DeclareLaunchArgument('params_file', default_value=default_params,
                              description='YAML с параметрами сегментации/захватов'),
        Node(
            package='xarm_grasping',
            executable='grasp_analyzer.py',
            name='grasp_analyzer',
            output='screen',
            parameters=[
                LaunchConfiguration('params_file'),
                {'use_sim_time': LaunchConfiguration('use_sim_time')},
            ],
        ),
    ])

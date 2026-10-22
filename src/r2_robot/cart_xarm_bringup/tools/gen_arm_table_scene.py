#!/usr/bin/env python3
"""Генерирует объекты мира arm_table.world и мост gz_bridge_arm_table.yaml
из config/arm_table_objects.yaml (единый список объектов сцены «рука на столе»).

    python3 src/r2_robot/cart_xarm_bringup/tools/gen_arm_table_scene.py

В мире заменяется блок между маркерами <!-- OBJECTS BEGIN --> / <!-- OBJECTS END -->.
Проверяет, что объекты не пересекаются друг с другом (по описанным окружностям).
"""
import math
import os
import sys

import yaml

PKG = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TABLE_TOP = 0.75          # верх стола в мире Gazebo (= -z спавна руки)
BEGIN, END = '    <!-- OBJECTS BEGIN', '    <!-- OBJECTS END -->'
COLOR = '<material><ambient>0.1 1.0 0.1 1</ambient><diffuse>0.1 1.0 0.1 1</diffuse>' \
        '<emissive>0.0 0.35 0.0 1</emissive></material>'


def inertia(o):
    m = o['mass']
    if o['shape'] == 'box':
        x, y, z = o['size']
        return m * (y * y + z * z) / 12, m * (x * x + z * z) / 12, m * (x * x + y * y) / 12
    r, h = o['size']
    return m * (3 * r * r + h * h) / 12, m * (3 * r * r + h * h) / 12, m * r * r / 2


def geometry(o):
    if o['shape'] == 'box':
        return '<box><size>{} {} {}</size></box>'.format(*o['size'])
    return '<cylinder><radius>{}</radius><length>{}</length></cylinder>'.format(*o['size'])


def height(o):
    return o['size'][2] if o['shape'] == 'box' else o['size'][1]


def radius(o):
    return 0.5 * math.hypot(o['size'][0], o['size'][1]) if o['shape'] == 'box' else o['size'][0]


def model(o):
    ixx, iyy, izz = inertia(o)
    z = TABLE_TOP + height(o) / 2 + 0.0005        # 0.5 мм запас от проникновения
    g = geometry(o)
    return f'''    <model name="{o['name']}">
      <pose>{o['xy'][0]} {o['xy'][1]} {z:.4f} 0 0 {o.get('yaw', 0.0)}</pose>
      <plugin filename="gz-sim-pose-publisher-system" name="gz::sim::systems::PosePublisher">
        <publish_model_pose>true</publish_model_pose>
        <publish_link_pose>false</publish_link_pose>
        <publish_collision_pose>false</publish_collision_pose>
        <publish_visual_pose>false</publish_visual_pose>
        <publish_sensor_pose>false</publish_sensor_pose>
        <use_pose_vector_msg>true</use_pose_vector_msg>
        <update_frequency>10</update_frequency>
      </plugin>
      <link name="link">
        <inertial>
          <mass>{o['mass']}</mass>
          <inertia>
            <ixx>{ixx:.3e}</ixx><iyy>{iyy:.3e}</iyy><izz>{izz:.3e}</izz>
            <ixy>0</ixy><ixz>0</ixz><iyz>0</iyz>
          </inertia>
        </inertial>
        <visual name="visual">
          <geometry>{g}</geometry>
          {COLOR}
        </visual>
        <collision name="collision">
          <geometry>{g}</geometry>
          <surface><friction><ode><mu>1.0</mu><mu2>1.0</mu2></ode></friction></surface>
        </collision>
      </link>
    </model>
'''


def bridge(objs):
    out = ['# СГЕНЕРИРОВАНО tools/gen_arm_table_scene.py из config/arm_table_objects.yaml — не править руками.',
           '# gz_bridge сценария «рука на столе»: /clock, поза руки, вакуум и поза каждого объекта.',
           '# Камера гриппера — gz_bridge_gripper_camera.yaml (общий с тележкой).',
           '- {ros_topic_name: "/clock", gz_topic_name: "/clock", ros_type_name: "rosgraph_msgs/msg/Clock", '
           'gz_type_name: "gz.msgs.Clock", direction: GZ_TO_ROS}',
           '- {ros_topic_name: "/gz/robot_pose", gz_topic_name: "/xarm6_table/pose", '
           'ros_type_name: "tf2_msgs/msg/TFMessage", gz_type_name: "gz.msgs.Pose_V", direction: GZ_TO_ROS}']
    for o in objs:
        n = o['name']
        out += [f'# {n}',
                f'- {{ros_topic_name: "/vacuum/{n}/attach", gz_topic_name: "/vacuum/{n}/attach", '
                f'ros_type_name: "std_msgs/msg/Empty", gz_type_name: "gz.msgs.Empty", direction: ROS_TO_GZ}}',
                f'- {{ros_topic_name: "/vacuum/{n}/detach", gz_topic_name: "/vacuum/{n}/detach", '
                f'ros_type_name: "std_msgs/msg/Empty", gz_type_name: "gz.msgs.Empty", direction: ROS_TO_GZ}}',
                f'- {{ros_topic_name: "/vacuum/{n}/state", gz_topic_name: "/vacuum/{n}/state", '
                f'ros_type_name: "std_msgs/msg/String", gz_type_name: "gz.msgs.StringMsg", direction: GZ_TO_ROS}}',
                f'- {{ros_topic_name: "/gz/object_pose", gz_topic_name: "/model/{n}/pose", '
                f'ros_type_name: "tf2_msgs/msg/TFMessage", gz_type_name: "gz.msgs.Pose_V", direction: GZ_TO_ROS}}']
    return '\n'.join(out) + '\n'


def main():
    with open(os.path.join(PKG, 'config', 'arm_table_objects.yaml')) as f:
        objs = yaml.safe_load(f)['objects']
    bad = []
    for i, a in enumerate(objs):
        for b in objs[i + 1:]:
            d = math.dist(a['xy'], b['xy'])
            if d < radius(a) + radius(b) + 0.01:
                bad.append(f"{a['name']} и {b['name']}: {d:.3f} м < {radius(a) + radius(b) + 0.01:.3f}")
    if bad:
        sys.exit('пересекаются:\n  ' + '\n  '.join(bad))
    wpath = os.path.join(PKG, 'worlds', 'arm_table.world')
    w = open(wpath).read()
    i, j = w.index(BEGIN), w.index(END)
    block = (BEGIN + ' — СГЕНЕРИРОВАНО tools/gen_arm_table_scene.py из '
             'config/arm_table_objects.yaml, не править руками -->\n'
             + ''.join(model(o) for o in objs))
    open(wpath, 'w').write(w[:i] + block + w[j:])
    open(os.path.join(PKG, 'config', 'gz_bridge_arm_table.yaml'), 'w').write(bridge(objs))
    print(f'объектов: {len(objs)} -> worlds/arm_table.world, config/gz_bridge_arm_table.yaml')


if __name__ == '__main__':
    main()

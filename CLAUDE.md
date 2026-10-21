# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

ROS 2 **Kilted** workspace (Ubuntu 24.04, Gazebo Sim 9): xArm6 with vacuum gripper and wrist camera mounted on the `quasi_cart` mobile base, plus a grasp analyzer. The repo root *is* the colcon workspace. Docs (README.md, CART_MODELS.md, INSTALL_RUN.md) and commit messages are in Russian — keep that convention.

## Build / run

```bash
cd ~/ros2k_ws                      # always build from the root, not src/
source /opt/ros/kilted/setup.bash
colcon build --symlink-install     # single package: --packages-select <pkg>
source install/setup.bash
```

`--symlink-install` makes edits to xacro/launch/worlds/python live without rebuilding; **new** files still need a rebuild of their package. Clean build: `rm -rf build install log`.

Main launches:
- `ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py` — cart+arm in Gazebo, MoveIt RViz, grasp_analyzer. Args: `use_moveit`, `use_rviz`, `headless` (no Gazebo GUI), `camera_model` (`gemini335le` default | `d435i`), `gripper_camera_preview`, `robot_model` (`c2l-v3-scaled` default, `c2`; see CART_MODELS.md), `spawn_x/y/z/yaw`, `arm_initial_positions`.
- `ros2 launch cart_xarm_bringup cart_xarm6_rviz.launch.py` — URDF check in RViz only.
- `ros2 launch xarm_gz_bringup xarm6_gz.launch.py add_vacuum_gripper:=true` — arm alone on a table.
- `ros2 launch xarm_moveit_config xarm6_moveit_fake.launch.py add_vacuum_gripper:=true` — MoveIt with fake hardware.
- URDF sanity: `xacro .../quasi_cart_description/urdf/cart_with_arm.urdf.xacro > /tmp/c.urdf && check_urdf /tmp/c.urdf`.

There is no test suite or linter configured.

## Living docs (keep updated)

- `docs/STATUS.md` — status board, known issues, decisions, changelog (newest first). Add an entry for every behavior change.
- `docs/CAMERA_VACUUM_SIM.md` — how the Orbbec camera / simulated vacuum / scene work + verification commands.

## Architecture

- `src/r2_robot/quasi_cart_description` — cart URDFs (`<robot_model>.urdf.xacro`), `cart_with_arm.urdf.xacro` (top-level), `cart-arm.xacro` (calls `xarm_device` macro to attach the arm), `cart-base.urdf.xacro` (nav camera flags `enable_camera_*`, all off by default).
- `src/r2_robot/cart_xarm_bringup` — main launch, worlds (`cart_table_offline.world`), `config/gz_bridge.yaml` (ros_gz bridge topics), RViz configs.
- `src/xarm_ros2/*` — fork of UFactory xarm_ros2. Wrist camera (D435i) is hardwired inside `xarm_description/urdf/xarm_device_macro.xacro` (`add_realsense_d435i`); gz sensors in `realsense.gazebo.xacro`. MoveIt builds its **own** robot model via `uf_ros_lib/moveit_configs_builder.py` (arm only, no cart, no camera by default) — changes to the arm URDF must be mirrored there. `_robot_moveit_common.launch.py` gets `use_sim_time` passed directly from our launch files.
- `src/r2_robot/quasi_camera_description` — camera xacro macros (Orbbec Gemini 335Le), bracket on `link_eef`, gz `rgbd_camera` sensor, per-model yaml. Hooked into `xarm_device_macro.xacro` via `add_gripper_camera`. Camera topics/frames follow `/gripper_camera/...`, `gripper_camera_*_optical_frame`.
- Simulated vacuum: gz `DetachableJoint` (in `cart-arm.xacro`, parent `link6`, child `pick_box`) driven by `xarm_grasping/scripts/vacuum_gripper_sim.py` (service `/vacuum_gripper/set`, contact check). `arm_goto_tcp.py` moves the TCP to a point without MoveIt (for testing).
- `src/xarm_gz_bringup` — standalone arm-on-table scenario.
- `src/xarm_grasping` — `scripts/grasp_analyzer.py` (ROS node: point cloud → table/object → grasp candidates, markers, `best_grasp_pose`) and `xarm_grasping/grasp_generator.py` (pure-Python cylinder grasp generator, no ROS).

## Gotchas

- The gz **Sensors system must be declared only in the world**, not in the robot model — a duplicate causes `Scene already exists` + segfault.
- Packages with `COLCON_IGNORE` (xarm_gazebo, realsense_gazebo_plugin, d435i_xarm_setup, xarm_planner, xarm_moveit_servo) are Gazebo Classic / unavailable in Kilted — don't re-enable. `src/object_recognition_msgs` is an empty dangling submodule.
- Known issues in grasp pipeline: `grasp_analyzer` still assumes a cylinder and silently falls back to the a-priori block pose; `table_height`/`block_x/y` are passed from launch as constants measured for c2l-v3-scaled at spawn (-0.3,0) (recompute if cart/spawn changes); table detection assumes a downward-looking optical frame. The table is at x=1.05 (top z=0.925); default spawn_x=-0.3 keeps the c2l-v3-scaled body clear of the table; the arm base (world x≈-0.54, at the handle end) is ~1.14 m from `pick_box` (x=0.6) — out of xArm6 reach (~0.7 m), scene layout still to be fixed.
- Vacuum in sim is a logical weld (DetachableJoint), not physics. The plugin welds the target at load; `vacuum_gripper_sim` detaches it at startup.
- gz does not resolve `package://` for our packages: mesh paths use `file://$(find pkg)/...`. In shell tests never `pkill -f` patterns that appear in your own command line.
- Planned work (camera_model:=gemini335le|d555, new `quasi_camera_description`, unified `/gripper_camera/...` topic/frame naming, grasp pipeline rewrite) is described in the design docs 00–10; follow their naming contract when implementing.

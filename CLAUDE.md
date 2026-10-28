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
- `ros2 launch cart_xarm_bringup cart_xarm6_gz.launch.py` — cart+arm in Gazebo, MoveIt RViz, grasp_analyzer. Args: `use_moveit`, `use_rviz`, `headless` (no Gazebo GUI), `camera_model` (`gemini335le` default | `d435i`), `gripper_camera_preview`, `robot_model` (`c2l-v3` default, `c2`; see CART_MODELS.md), `front_camera` (cart front Orbbec, default true), `spawn_x/y/z/yaw`, `arm_initial_positions`.
- `ros2 launch cart_xarm_bringup cart_xarm6_rviz.launch.py` — URDF check in RViz only.
- `ros2 launch cart_xarm_bringup arm_table_gz.launch.py` — same arm (camera, vacuum, analyzer) fixed on a table, no cart (`arm_on_table.urdf.xacro`, `worlds/arm_table.world`); for grasp debugging/training. TF `world` = `link_base`, table top z=0.
- `ros2 launch cart_xarm_bringup cart_nav_gz.launch.py` — cart navigation: Nav2 + slam_toolbox + OctoMap (front camera) in `worlds/cart_nav.world`; params `config/nav2_cart.yaml` (see docs/NAV.md).
- `ros2 launch xarm_gz_bringup xarm6_gz.launch.py add_vacuum_gripper:=true` — arm alone on a table.
- `ros2 launch xarm_moveit_config xarm6_moveit_fake.launch.py add_vacuum_gripper:=true` — MoveIt with fake hardware.
- URDF sanity: `xacro .../quasi_cart_description/urdf/cart_with_arm.urdf.xacro > /tmp/c.urdf && check_urdf /tmp/c.urdf`.

There is no test suite or linter configured.

## Living docs (keep updated)

- `docs/STATUS.md` — status board, known issues, decisions, changelog (newest first). Add an entry for every behavior change.
- `docs/CAMERA_VACUUM_SIM.md` — how the Orbbec camera / simulated vacuum / scene work + verification commands.
- `docs/NAV.md` — cart navigation (Nav2, SLAM, OctoMap, maps, troubleshooting).
- `docs/CLAUDE_MISTAKES.md` — Claude's past mistakes in this project and lessons; read it before nav/sensor work and add new entries.

## Architecture

- `src/r2_robot/quasi_cart_description` — cart URDFs (`<robot_model>.urdf.xacro`), `cart_with_arm.urdf.xacro` (top-level), `cart-arm.xacro` (calls `xarm_device` macro to attach the arm), `cart-base.urdf.xacro` (nav camera flags `enable_camera_*`, all off by default).
- `src/r2_robot/cart_xarm_bringup` — main launch, worlds (`cart_table_offline.world`), `config/gz_bridge.yaml` (ros_gz bridge topics), RViz configs.
- `src/xarm_ros2/*` — fork of UFactory xarm_ros2. Wrist camera (D435i) is hardwired inside `xarm_description/urdf/xarm_device_macro.xacro` (`add_realsense_d435i`); gz sensors in `realsense.gazebo.xacro`. MoveIt builds its **own** robot model via `uf_ros_lib/moveit_configs_builder.py` (arm only, no cart, no camera by default) — changes to the arm URDF must be mirrored there. `_robot_moveit_common.launch.py` gets `use_sim_time` passed directly from our launch files.
- `src/r2_robot/quasi_camera_description` — camera xacro macros (Orbbec Gemini 335Le), bracket on `link_eef`, gz `rgbd_camera` sensor, per-model yaml. Hooked into `xarm_device_macro.xacro` via `add_gripper_camera`. Camera topics/frames follow `/gripper_camera/...`, `gripper_camera_*_optical_frame`. Bracket = CAD mesh `Camera_Stand_1300` on the flange; camera back-mounted on its tab, looking along the tool axis.
- Simulated vacuum: gz `DetachableJoint` (in `cart-arm.xacro`, parent `link6`, child `pick_box`) driven by `xarm_grasping/scripts/vacuum_gripper_sim.py` (service `/vacuum_gripper/set`, topic `/vacuum_gripper/cmd`, contact check, picks nearest object). One DetachableJoint per model in xacro arg `vacuum_targets`; gz topics `/vacuum/<model>/{attach,detach,state}`. RViz button: `vacuum_rviz_button.py` (interactive marker `/vacuum_button`). `arm_goto_tcp.py` moves the TCP to a point without MoveIt (for testing).
- `src/xarm_gz_bringup` — standalone arm-on-table scenario.
- `src/xarm_grasping` — `scripts/grasp_analyzer.py` (older ROS node: point cloud → grasp candidates, assumes cylinder) and the pick&place stack (docs/PICK_PLACE.md): `xarm_grasping/perception.py` (pure numpy: table, clusters, box/cylinder fit, rule classes from `config/objects.yaml`), `object_perception.py` (/objects, action /detect_objects), `scene_manager.py` (MoveIt Planning Scene, attach on vacuum hold, zones `config/place_zones.yaml`, action /find_place_pose), `pick_place_server.py` (action /pick_place via move_group interfaces; moveit_py/MTC are not available in Kilted). Interfaces in `src/xarm_grasping_msgs`. The wrist camera sees only beyond 0.25 m and sits ~7 cm above the suction tip — detect only from the observe pose.

## Gotchas

- The gz **Sensors system must be declared only in the world**, not in the robot model — a duplicate causes `Scene already exists` + segfault.
- Packages with `COLCON_IGNORE` (xarm_gazebo, realsense_gazebo_plugin, d435i_xarm_setup, xarm_planner, xarm_moveit_servo) are Gazebo Classic / unavailable in Kilted — don't re-enable. `src/object_recognition_msgs` is an empty dangling submodule.
- Known issues in grasp pipeline: `grasp_analyzer` still assumes a cylinder; it rejects implausible objects (range/radius/height guards) and uses the a-priori block pose only with `use_prior_pose:=true`; `scene_markers.py` shows the true Gazebo scene in RViz (`/scene_markers`) for comparison; `table_height`/`block_x/y` are passed from launch as constants measured for c2l-v3 at spawn (-0.3,0) (recompute if cart/spawn changes); table detection assumes a downward-looking optical frame. The table is at x=1.05 (top z=0.925); default spawn_x=-0.3 keeps the c2l-v3 body clear of the table; the arm base (world x≈-0.54, at the handle end) is ~1.14 m from `pick_box` (x=0.6) — out of xArm6 reach (~0.7 m), scene layout still to be fixed.
- The cart lidar sees the cart's own posts (~0.46 m). The gz bridge publishes `/scan_raw`; `cart_xarm_bringup/scripts/lidar_blackout_filter.py` publishes `/scan` without the post sectors (`config/lidar_blackout.yaml` = `blackout_angles` of the ydlidar driver from quasi_ros_c1, as on hardware). Don't work around it with min-range hacks, and don't move sensors away from their real poses to fix sim artifacts.
- DDS load: with default Fast DDS simple discovery ~50 sim processes burn 30–50% CPU per idle node in the `dds.asyn` thread (check with `top -H -p <pid>`), Nav2 bringup can time out (`"spin" action server not available`). **Don't use Fast DDS Discovery Server**: clients don't see the full graph, Nav2 costmaps (published only when `count_subscribers` > 0) stop publishing. CycloneDDS was tried (2026-10-28) and is NOT better (see docs/STATUS.md); stay on default Fast DDS. Note rmw_cyclonedds_cpp caps one domain at 32 ROS processes (`MaxAutoParticipantIndex`), the Nav2 sim needs ~32+.
- gz publishes `/clock` every physics step (1 ms → 1000 Hz). The bridge maps it to `/clock_gz`; `clock_throttle` (topic_tools) republishes `/clock` at 200 Hz. Don't bridge gz `/clock` straight to `/clock`. Also the cart wheels `JointStatePublisher` needs `<update_rate>` (otherwise ~1 kHz). `clock_throttle` must keep `use_sim_time: False`.
- Vacuum in sim is a logical weld (DetachableJoint), not physics. The plugin welds the target at load; `vacuum_gripper_sim` detaches it at startup.
- gz does not resolve `package://` for our packages: mesh paths use `file://$(find pkg)/...`. In shell tests never `pkill -f` patterns that appear in your own command line.
- When testing in sim yourself, isolate it (`ROS_DOMAIN_ID`, `GZ_PARTITION`) — the user often runs their own sim in parallel.
- Grasp algorithms from the R2 robot (GPD for two-finger grippers, segmentation, shape fitting) are kept for later in `docs/reference/quasi_ros_r2_grasp/` (COLCON_IGNORE).
- Planned work (camera_model:=gemini335le|d555, new `quasi_camera_description`, unified `/gripper_camera/...` topic/frame naming, grasp pipeline rewrite) is described in the design docs 00–10; follow their naming contract when implementing.

# ros2k_ws — тележка quasi_cart + xArm6 с присоской и камерой (ROS 2 Kilted, Gazebo Sim)

Вся документация — в одной папке **[docs/](docs/README.md)**. Начать отсюда:

| Что нужно | Где |
|---|---|
| Оглавление всей документации | [docs/README.md](docs/README.md) |
| Установить, собрать, первый запуск | [docs/INSTALL.md](docs/INSTALL.md) |
| Запустить нужный сценарий (рука на столе, на тележке, навигация, без руки) | [docs/RUN_SCENARIOS.md](docs/RUN_SCENARIOS.md) |
| Что сейчас работает, что сломано, журнал изменений | [docs/STATUS.md](docs/STATUS.md) |
| Что сделано и что осталось | [docs/CHECKLIST.md](docs/CHECKLIST.md) |

Сборка:

```bash
cd ~/ros2k_ws
source /opt/ros/kilted/setup.bash
colcon build --symlink-install
source install/setup.bash
```

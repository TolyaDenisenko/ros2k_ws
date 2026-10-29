# Задача: почему Python-ноды жгут CPU (и нужен ли C++)

Статус: **не начата**. Записано 2026-10-28, выполнить 2026-10-29. Работает человек
или Claude: шаги ниже самодостаточны, скрипты прошлой сессии не сохранились.

## Зачем

В `cart_nav_gz` суммарно ~740% из 1200% CPU (12 ядер). Из них ≈ 240% — Python-ноды,
при этом три из них почти ничего не делают:

| Нода | CPU | Что делает по коду |
|---|---|---|
| `xarm_grasping/scripts/arm_parking.py` | 77% | раз в 0,2 с проверяет, ехала ли тележка |
| `xarm_grasping/scripts/vacuum_gripper_sim.py` | 66% | таймер 10 Гц, подписки на позы |
| `xarm_grasping/scripts/grasp_analyzer.py` | 42% | анализ раз в 2 с, облако точек |

Остальные: `joint_state_publisher` 19%, `vacuum_rviz_button` 13%, `scene_markers` 13%,
`lidar_blackout_filter` 11%. Для сравнения Nav2 целиком ≈ 145%, Gazebo ≈ 250%.

Что известно точно: у `arm_parking` нагружен **один поток самого Python-процесса** (72%),
потоки DDS около 1% (`top -H -p <pid>`). Значит дело не в сети и не в обнаружении DDS.

## Гипотеза (не проверена)

Каждая Python-нода с `use_sim_time` принимает `/clock` (сейчас 200 Гц, прореживает
`clock_throttle`, launch `cart_xarm6_gz.launch.py:181-186`). Python принимает и
разбирает каждое сообщение, а в `MultiThreadedExecutor` (у `arm_parking` и
`vacuum_gripper_sim`) каждое сообщение дополнительно будит потоки. Нагрузка должна
падать пропорционально частоте `/clock`.

## Эксперимент (≈ 10 минут)

Правила: **изолированная симуляция**, чужую не трогать (`CLAUDE.md`). Свой `ROS_DOMAIN_ID`
и `GZ_PARTITION`, `headless:=true use_rviz:=false`. Не использовать `pkill -f` с
паттерном из собственной командной строки. Fast DDS Discovery Server не включать
(пропадают costmap).

1. Запуск с разной частотой `/clock`. Временно поменять `'200.0'` в
   `cart_xarm6_gz.launch.py:183` (лучше сразу добавить launch-аргумент `clock_rate`,
   по умолчанию 200) на 200, 50 и 20:
   ```bash
   cd ~/ros2k_ws && source /opt/ros/kilted/setup.bash && source install/setup.bash
   export ROS_DOMAIN_ID=71 GZ_PARTITION=cpu_task
   ros2 launch cart_xarm_bringup cart_nav_gz.launch.py headless:=true use_rviz:=false
   ```
2. Через ~100 с после старта (Nav2 `active`) замерить CPU по потокам в **другом**
   терминале с тем же `ROS_DOMAIN_ID`:
   ```bash
   for n in arm_parking vacuum_gripper_sim grasp_analyzer; do
     P=$(ps -eo pid,args | awk -v n="$n" '$0 ~ n".py" && !/awk/ {print $1; exit}')
     echo "== $n"; top -H -b -n2 -d5 -w 200 -p $P | awk '/^top -/{n++} n==2' | sed -n 7,10p
   done
   ```
   Прогон на каждую частоту повторить минимум дважды: прошлые замеры шумные.
   Браузер и прочее тяжёлое на время замера закрыть.
3. Записать таблицу «частота `/clock` → CPU трёх нод».

## Как читать результат

| Результат | Вывод | Что делать |
|---|---|---|
| CPU падает примерно пропорционально частоте | виноват поток `/clock` в rclpy | оставить `clock_rate` 50 (проверить, что Nav2 и ros2_control не страдают), C++ не нужен |
| CPU почти не меняется | дело в самих нодах | профилировать (`py-spy dump/top`; может потребоваться `sudo sysctl kernel.yama.ptrace_scope=0`), искать цикл без ожидания, лишние подписки, исполнитель |
| Падает частично | вклад и часов, и кода | сначала часы, затем профилирование остатка |

## Про переписывание на C++

Решение принимать только после эксперимента.
- `arm_parking` (137 строк) и `vacuum_gripper_sim` (453) — кандидаты, если rclpy
  окажется тяжёлым сам по себе: небольшие, без numpy.
- `grasp_analyzer` (445) — плохой кандидат: numpy, scipy, cv_bridge; к тому же
  вытесняется стеком `object_perception` + `pick_place_server`
  (`docs/PICK_PLACE.md`).
- Цена C++: сборка и CMake вместо правки без пересборки, чужим студентам сложнее
  (учебная книга `docs/book/` построена на Python-нодах).

## Критерий готовности

Суммарный CPU трёх нод в покое ≤ 30% (сейчас ≈ 185%) при том, что Nav2 `active`,
цель навигации `SUCCEEDED`, costmap и OctoMap приходят. По итогам: запись в
`docs/STATUS.md` (с таблицей замеров) и, если нашлась причина-ловушка, в
`docs/CLAUDE_MISTAKES.md`.

## Уже сделано по теме CPU (2026-10-28), не повторять

- `clock_throttle`: `/clock` 1000 → 200 Гц (мост отдаёт `/clock_gz`),
  у ноды явный `use_sim_time: False`.
- `JointStatePublisher` колёс: `<update_rate>30` (было ~850 Гц),
  `cart_with_arm.urdf.xacro`.
- Проверено и **отвергнуто**: Fast DDS Discovery Server (ломает costmap),
  CycloneDDS (не лучше; в домене максимум 32 ROS-процесса). Подробности — `STATUS.md`.
- Быстрое снижение нагрузки без правок кода: `headless:=true` (≈ −1 ядро),
  `use_rviz:=false`, `use_octomap:=false`.

## Результаты 2026-10-29 (Claude, изолированная сим)

Замер CPU **пустой** ноды rclpy (без подписок и таймеров), в живой симуляции навигации
(`/clock` 200 Гц), 4 повтора по 8 с:

| Часы ноды | SingleThreadedExecutor | MultiThreadedExecutor |
|---|---|---|
| системное время (`use_sim_time:=false`) | 0.1% | 0.2% |
| время симуляции (`use_sim_time:=true`) | **9%** | **84%** |

Вывод: гипотеза про `/clock` подтверждена. Сам поток часов стоит ≈9% ядра на каждую
Python-ноду, а в связке с `MultiThreadedExecutor` (его используют `arm_parking` и
`vacuum_gripper_sim`) — ≈84%. Число потоков не влияет (2 потока: 84%, 4: 88%, 12: 91%).
Настоящая `arm_parking` держит 77–96% во всех проверенных конфигурациях (часы системные или симуляции, 2–12 потоков); с `SingleThreadedExecutor` её отдельно не проверяли.

Что остаётся проверить: `/clock` 50 и 20 Гц (эксперимент из этого файла) и переписать
`arm_parking` на `SingleThreadedExecutor` (блокирующий `switch()` с ожиданием future —
переделать на `call_async` и колбэк). Поскольку цена потока часов приходится на каждую
Python-ноду отдельно, частота `/clock` и тип исполнителя важнее, чем замена языка.

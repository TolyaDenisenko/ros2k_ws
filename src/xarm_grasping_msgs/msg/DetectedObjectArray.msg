# Все объекты на столе; header.frame_id — кадр поз (base_frame, обычно link_base)
std_msgs/Header header
DetectedObject[] objects
# Высота столешницы в header.frame_id (z найденной плоскости)
float64 table_z

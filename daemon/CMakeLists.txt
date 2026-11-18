cmake_minimum_required(VERSION 3.16)
project(asus-hub-daemon LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(_GNU_SOURCE)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CJSON REQUIRED libcjson)
pkg_check_modules(UDEV REQUIRED libudev)

add_executable(asus-hub-daemon
    src/main.c
    src/core/file_utils.c
    src/core/proc.c
    src/hwmon/category.c
    src/hwmon/hwmon.c
    src/hwmon/sensor.c
    src/info/cpu_freq.c
    src/info/cpu_info.c
    src/info/gpu_info.c
    src/info/nvidia.c
    src/info/profile.c
    src/info/ram_info.c
    src/server/commands.c
    src/server/server.c
    src/stats/cpu_stats.c
    src/stats/fan_stats.c
    src/stats/gpu_stats.c
    src/stats/pick.c
    src/stats/ram_stats.c
    src/stats/stats.c
)

target_include_directories(asus-hub-daemon PRIVATE
    src
    ${CJSON_INCLUDE_DIRS}
    ${UDEV_INCLUDE_DIRS}
)

target_link_libraries(asus-hub-daemon PRIVATE ${CJSON_LIBRARIES} ${UDEV_LIBRARIES})
target_compile_options(asus-hub-daemon PRIVATE -Wall -Wextra ${CJSON_CFLAGS_OTHER})

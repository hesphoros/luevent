cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# 设置项目名称和版本
project(luevent VERSION 1.0 LANGUAGES C)

# 设置 C 标准为 C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置源文件列表
set(SOURCES
    src/lu_event.c
    src/lu_mm-internal.c
    src/main.c
)

# 创建可执行文件
add_executable(luevent ${SOURCES})

# 将 include 和 compat 目录添加到目标的头文件搜索路径
target_include_directories(luevent PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/compat
)

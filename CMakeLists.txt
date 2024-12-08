cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(luevent VERSION 1.0 LANGUAGES C)

# 设置 C 标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

# 添加 include 和 compat 目录到头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/compat)

# 自动查找 src/ 目录下的所有 .c 文件
file(GLOB_RECURSE SOURCES "src/*.c")

# 创建一个库目标 luevent
add_library(luevent ${SOURCES})

# 设置目标的 include 路径
target_include_directories(luevent PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 启用测试功能
enable_testing()

# 将 tests 目录作为子目录
add_subdirectory(tests)

# 将 examples 目录作为子目录
add_subdirectory(examples)

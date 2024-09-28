cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(luevent VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.c")

add_library(luevent ${SOURCES})

# 设置目标属性
target_include_directories(luevent PUBLIC include)

# 添加测试
enable_testing()
add_subdirectory(tests)

# 示例
add_subdirectory(examples)
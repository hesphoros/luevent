# tests/CMakeLists.txt

# 设置最小的 CMake 版本
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(TestProject)


set(CMAKE_C_STANDARD 11)

# 添加 include 目录，告诉编译器在哪里查找头文件
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories($(PROJECT_SOURCE_DIR)/include)
#include_directories(./include)
# 添加测试用例的可执行文件
add_executable(test_executable ${CMAKE_SOURCE_DIR}/tests/test.c)


# 注册测试
#add_test(NAME MyTest COMMAND test_executable)

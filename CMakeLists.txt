cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# 设置项目名称和版本
project(luevent VERSION 1.0 LANGUAGES C)

# 设置 C 标准为 C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

# 添加 include 和 compat 目录到头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/compat)

# 获取 src 目录下所有源文件
file(GLOB_RECURSE SOURCES "src/*.c")

# 创建静态库或共享库（luevent）
add_library(luevent ${SOURCES})

# 设置目标的 include 路径
target_include_directories(luevent PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 启用测试功能
enable_testing()

# 添加 tests 目录作为子目录，确保 tests 目录下有 CMakeLists.txt 文件
add_subdirectory(tests)

# 添加 examples 目录作为子目录，确保 examples 目录下有 CMakeLists.txt 文件
add_subdirectory(examples)

# 可选：安装配置（如果需要安装）
install(TARGETS luevent DESTINATION lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

# 可选：设置安装时的 CMake 配置
set(CMAKE_INSTALL_PREFIX "/usr/local")  # 设置安装路径（可以修改为适合你的路径）

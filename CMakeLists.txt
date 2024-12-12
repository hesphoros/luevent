cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# 设置项目名称和版本
project(luevent VERSION 1.0 LANGUAGES C CXX)

# 设置 C 和 C++ 标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 17) # 设置 C++ 为 C++17
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置源文件列表
set(SOURCES
    src/lu_event.c
    src/lu_mm-internal.c
    src/main.cpp    
    src/lu_error.cpp
)

# 添加编译选项（可选）
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 创建可执行文件
add_executable(luevent ${SOURCES})

# 链接必要的库
target_link_libraries(luevent pthread)

# 添加头文件搜索路径
target_include_directories(luevent PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/compat
)

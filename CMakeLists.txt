cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# 设置项目名称和版本
project(luevent VERSION 1.0 LANGUAGES C CXX)

# 设置 C 和 C++ 标准
# set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD 99) # 设置 C 为 C99
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17) # 设置 C++ 为 C++20

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# 导出编译命令，以便与调试工具（如 GDB）配合使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置源文件列表
set(SOURCES
    src/lu_event.c
    src/lu_mm-internal.c
    src/main.c    
    src/lu_error.c
    src/lu_log.c
    src/lu_util.c
    src/lu_hash_table.c
)

 



# 添加编译选项（可选）
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_options(-Wno-unused-variable -Wno-unused-function -Wno-deprecated-declarations)
    add_compile_options(-fanalyzer)
endif()


# 创建可执行文件
add_executable(luevent ${SOURCES})


if(UNIX)
    target_link_libraries(luevent pthread)
endif()

# 添加头文件搜索路径
target_include_directories(luevent PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/compat
)



# 设置默认的构建类型为 Debug（如果没有指定）
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fsanitize=address,undefined")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -fsanitize=address,undefined")
endif()


# 为 Debug 构建类型添加调试信息并禁用优化
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")  # -g 添加调试信息，-O0 禁用优化
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")  # C 编译器也设置调试信息
endif()
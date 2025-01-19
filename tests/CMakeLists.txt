cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(lueventGTest VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Google Test 
find_package(GTest REQUIRED)

# 判断是否找到 GTest
if(GTest_FOUND)
    message(STATUS "Google Test found at: ${GTest_DIR}")
else()
    message(FATAL_ERROR "Google Test not found. You may need to install it.")
endif()

# 启用 AddressSanitizer（ASAN）
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

# 启用 Undefined Behavior Sanitizer（UBSan）
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
endif()
# 定义源文件
set(SOURCES
    ${PROJECT_SOURCE_DIR}/test_main.cpp  # 你的测试文件
    # ${PROJECT_SOURCE_DIR}/test_luevent.cpp  # 如果你有这个文件，可以取消注释
)

# 查找安装好的 luevent 库（假设库已安装在 tests/lib 下）
find_library(LUEVENT_LIB NAMES libluevent_static.a PATHS ${PROJECT_SOURCE_DIR}/lib REQUIRED)

# 假设头文件目录位于 ../include
include_directories(${PROJECT_SOURCE_DIR}/../include)

# 创建测试可执行文件
add_executable(lueventGTest ${SOURCES})

# 启用测试
enable_testing()

# 链接 GTest 库和安装好的 luevent 库
target_link_libraries(lueventGTest PRIVATE
    pthread
    GTest::GTest
    ${LUEVENT_LIB}
)

# 添加测试
add_test(NAME LueventTest COMMAND lueventGTest)


# 将静态库和动态库文件添加到默认清理规则
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
    "${PROJECT_SOURCE_DIR}/lib/libluevent_static.a"
    #"${PROJECT_SOURCE_DIR}/lib/libluevent_shared.so"
)

set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES
    "${PROJECT_SOURCE_DIR}/lib/libluevent_shared.so"
)
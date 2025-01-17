cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(LueventGTest VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Google Test
find_package(GTest)

# 判断是否找到 GTest
if(GTest_FOUND)
    message(STATUS "Google Test found at: ${GTest_DIR}")
else()
    message(WARNING "Google Test not found. You may need to install it or set GTest_DIR.")
endif()

# 定义源文件
set(SOURCES
    ${PROJECT_SOURCE_DIR}/test_main.c  # 你的测试文件
)

# 创建目标 LueventGTest 可执行文件
add_executable(LueventGTest ${SOURCES})

# 添加头文件路径
target_include_directories(LueventGTest PUBLIC
    ${PROJECT_SOURCE_DIR}/include  # 你的头文件目录
)

# 链接 GTest 库
if(GTest_FOUND)
    target_link_libraries(LueventGTest PRIVATE
        pthread
        GTest::GTest
        GTest::GMock
    )
else()
    message(WARNING "Proceeding without Google Test.")
endif()

# 启用测试
enable_testing()

# 添加测试
add_test(NAME LueventTest COMMAND LueventGTest)
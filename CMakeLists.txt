cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# 设置项目名称和版本
project(luevent VERSION 1.0 LANGUAGES C CXX)

# 设置 C 和 C++ 标准
set(CMAKE_C_STANDARD 17) # 设置 C 为 C17
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_CXX_EXTENSIONS OFF)


file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.c")



# 添加编译选项（可选）
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -g -O0)
    add_compile_options(-Wno-unused-variable -Wno-unused-function -Wno-deprecated-declarations)
    add_compile_options(-fanalyzer)
    
endif()

# 创建可执行文件
# add_executable(luevent ${SOURCES})

# 创建静态库
add_library(luevent_static STATIC ${SOURCES})
# 创建动态库
#add_library(luevent_shared SHARED ${SOURCES})

# 链接 pthread 库(针对 UNIX)
if(UNIX)
    #target_link_libraries(luevent pthread)
    target_link_libraries(luevent_static pthread)
    #target_link_libraries(luevent_shared pthread)
    #target_link_libraries(luevent gtest)
endif()

# 添加头文件搜索路径
target_include_directories(luevent_static PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/compat
)

#target_include_directories(luevent_shared PUBLIC
#    ${PROJECT_SOURCE_DIR}/include
#    ${PROJECT_SOURCE_DIR}/compat
#)


# 设置默认的构建类型为 Debug（如果没有指定）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

# 设置调试信息并启用 AddressSanitizer 和 UndefinedBehaviorSanitizer
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer")

# 为 Release 构建类型设置优化标志
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -DNDEBUG")
# 如果是 Windows 平台，链接所需的库（如 ws2_32.lib）
if(WIN32)
    target_link_libraries(luevent_static ws2_32)
    #target_link_libraries(luevent_shared ws2_32)
endif()


# 设置安装规则
install(TARGETS luevent_static #luevent_shared
    ARCHIVE DESTINATION ~/codespace/luevent/tests/lib
    LIBRARY DESTINATION ~/codespace/luevent/tests/lib
    RUNTIME DESTINATION ~/codespace/luevent/tests/bin
)

# 将静态库和动态库文件添加到默认清理规则
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
    "${PROJECT_SOURCE_DIR}/tests/lib/libluevent_static.a"
    #"${PROJECT_SOURCE_DIR}/lib/libluevent_shared.so"
)
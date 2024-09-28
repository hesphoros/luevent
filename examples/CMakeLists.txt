# 示例可执行文件
file(GLOB_RECURSE EXAMPLE_SOURCES "*.c")

foreach(example_file ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${example_file} NAME_WE)
    add_executable(${example_name} ${example_file})
    target_link_libraries(${example_name} PRIVATE luevent)
endforeach()

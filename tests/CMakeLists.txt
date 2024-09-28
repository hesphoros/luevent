# 测试可执行文件
file(GLOB_RECURSE TEST_SOURCES "*.c")

foreach(test_file ${TEST_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE luevent)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

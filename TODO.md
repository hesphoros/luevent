
rm -rf build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make



lu_event_log_logv_ 和 lu_evutil_vsnprintf 的完整代码。
日志相关全局变量的定义和初始化。
多线程使用的场景描述。

lu_log.c 文件第 357 行：
确保 lu_init_event 函数中传递的参数 log_event 和 data 都是有效指针。

lu_log.c 文件第 413 行：
检查 lu_event_log_logv_ 函数的实现，特别是 severity、errstr、file、line 和 fmt 参数的处理情况。

lu_log.c 文件第 174 行：
查看 lu_event_warn 函数，确保所有调用的格式化字符串和参数都是有效的。

main.c 文件第 155 行：
在 test_logging 函数中，检查调用 lu_event_warn 时传递的参数是否正确。
# Development Log

## 2025-1-17

- [X] 完成了luevent内部的lu_mm_pool 内存池的修改，lu_mm_pool内存池参考了ngx内存池
- [X] 添加了GTest来进行单元测试
- [ ] 完成luevent的基本功能，包括事件的创建、订阅、发布、删除等

## 2025-1-18 

- [X] 添加了luevent-memory-pool的错误码

## 2025-1-19

- [X] 修复了lu_errorn 中的内存泄露；泄露原因是lu_hash_table rehash 存在问题

## 2025-1-21

✅hesphoros

- [ ] 完成lu_hash_table的gtest
- [x] 完成lu_default_file_log的封装
- [x] 完善**lu_enable_default_file_logging** 中的filename参数如果包含目录则创建
- [x] 完成函数**lu_strip_directory**
- [x] 完成函数**lu_contain_directory**
- [x] 完成了lu_enable_default_file_logging中的filename不为空的情况
- [x] 修复了**lu_evutil_get_directory** 函数内部计算目录strlen的错误
- [x] 完成test_lu_evutil_get_dict
- [x] feat: 添加VSCode配置文件并优化日志路径处理

## 2025-1-22
✅hesphoros

- [ ] 对tests下的test_main内部的TEST进行模块划分
- [ ]  解决undefined reference to lu_mm_pool_alloc(lu_mm_pool_s*, unsigned long) 等问题
- [ ] 完善lu_event的支持

1. 网络IO事件
2. 定时器事件(时间事件)
3. 信号事件
4. 活动的事件队列
5. 信号操作的函数接口
6. IO操作的函数接口

~~~cpp
只实现epollops, pollops,selectops
/* Array of backends in order of preference. */
static const struct eventop *eventops[] = {
#ifdef EVENT__HAVE_EVENT_PORTS
	&evportops,
#endif
#ifdef EVENT__HAVE_WORKING_KQUEUE
	&kqops,
#endif
#ifdef EVENT__HAVE_EPOLL
	&epollops,
#endif
#ifdef EVENT__HAVE_DEVPOLL
	&devpollops,
#endif
#ifdef EVENT__HAVE_POLL
	&pollops,
#endif
#ifdef EVENT__HAVE_SELECT
	&selectops,
#endif
#ifdef _WIN32
	&win32ops,
#endif
#ifdef EVENT__HAVE_WEPOLL
	&wepollops,
#endif
	NULL
};
~~~


![alt text](image.png)

## 2025-1-23

no progress

## 2025-1-24

1. 完成ev_base 后端的封装，支持epool select poll 等后端

- [ ] 完善了mm_internal 中的memzero模块,~lu_event_mm_memzero_~
- [ ] 修复了lu_event_mm_free_中的悬挂指针问题
- [ ] lu_log_functions_global_中完善了memzero模块，日志回调函数也支持了memzero的回调
- [ ] 修复了lu_mm_aligned_malloc_fn_ 函数指针的返回值int-> void*
- [ ] 编写monotonic_time的documenation文档
- [ ] 编写struct_timespec_documenation文档
- [ ] 在lu_event_internal中对lu_min_heap_ 重复包含问题进行修复
- [ ] 添加了lu_evthread-internal.h用于内部保证线程安全
- [ ] 优化了lu_utils中的时间函数 时间加法  时间减法  时间比较
- [ ] 更新对posix的单调时钟support
- [ ] 完成此方法lu_evutil_configure_monotonic_time_

## 2025-1-25

- [X] lu_evutil_monotonic_timer_t 定义且完成单调计时器
- [X] 重构了lu_event_struct ,移动了lu_event_t 和lu_callback_t 到lu_event_struct中
- [X] 修复了lu_event_base_s中的active_queues active_later_queue 成员；
	   其类型TAILQ_HEAD(lu_evcallback_list, lu_event_callback_t)
- [X] 添加了lu_evsiganl-internal.h 用于内部的信号处理
- [X] 在lu_evsiganl-internal中提案加了LU_EVTHREAD_GET_ID LU_EVBASE_IN_THREAD。。。等宏函数
- [X] 在lu_evsiganl.c中完成lu_evthread_condition_callbacks_t 等 lu_evthread_set_condition_callbacks等的定义
- [X] 完成了lu_evsignal_info_t 结构体，保存signal的相关信息
- [X] 修复了lu_min_heap_ 中的函数，去除了static修饰符，,使代码全文可见
- [X] 在lu_utils中添加了断言辅助宏函数
- [X] 完善了mm_internal `lu_event_base_new_with_config`函数中的最小时间堆的初始化，以及信号通知
- [X] 在main.c 中include 新增头文件

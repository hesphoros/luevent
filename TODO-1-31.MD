

```markdown
# 编译警告与错误任务列表

## 1. 隐式声明函数
以下函数在代码中被使用，但没有找到声明。需要为这些函数添加正确的声明或包含相应的头文件。

- [X] `lu_evmap_io_clear_`
- [X] `lu_evmap_signal_clear_`
- [X] `lu_event_changelist_freemem_`
- [X] `LU_EVTHREAD_FREE_COND`
- [X] `lu_event_callback_cancel_nolock_`
- [X] `event_base_once`
- [X] `lu_event_queue_remove_timeout`
- [X] `event_queue_remove_active`
- [X] `event_to_event_callback`
- [X] `event_queue_remove_active_later`
- [X] `event_queue_remove_inserted`
- [X] `evmap_io_del_`
- [X] `evmap_signal_del_`
- [X] `event_haveevents`
- [X] `N_ACTIVE_CALLBACKS`
- [X] `evthread_notify_base`
- [X] `lu_event_debug_note_del_`
- [X] `LU_EVTHREAD_COND_WAIT`
- [X] `lu_evthread_is_debug_lock_held_`
- [X] feat: 添加事件优先级设置与删除函数
- [X] style: 移除多余空行
- [X] refactor: 添加错误检查并优化代码
- [X] perf: 完善lu_common_timeout_list_s的实现,优化相关结构定义
## 2. 未定义函数或宏
以下函数或宏在代码中使用，但未定义。

- [X] `lu_evutil_assert_impl`
- [ ] `lu_epoll_init`
- [ ] `lu_epoll_nochangelist_add`
- [ ] `lu_epollnochangelist_del`
- [ ] `lu_epoll_dispatch`
- [ ] `lu_epoll_dealloc`

## 3. 未定义的静态函数
以下静态函数在头文件中声明，但在源文件中未定义。

- [ ] `lu_evutil_assert_impl`
- [ ] `lu_epoll_init`
- [ ] `lu_epoll_nochangelist_add`
- [ ] `lu_epollnochangelist_del`
- [ ] `lu_epoll_dispatch`
- [ ] `lu_epoll_dealloc`

## 4. 语法问题
- [X] `LU_EVLOCK_ASSERT_LOCKED` 宏的 `do` 和 `while(0)` 闭合符号问题。
```

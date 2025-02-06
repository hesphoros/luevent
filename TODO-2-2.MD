### 隐式函数声明修复任务列表
lu_event_callback_activate_nolock_
- [X] **`lu_evtimer_assign` 未声明**
  - **问题**：函数被调用但未声明/定义
  - **修复**：
    1. 在头文件添加原型声明：
    ```c
    void lu_evtimer_assign(struct event*, lu_event_base_t*, lu_event_callback_fn, void*);
    ```
    2. 在对应源文件实现该函数

- [X] **`event_del_nolock_` 命名冲突**
  - **问题**：`event_`前缀与系统库冲突
  void event_active_nolock_(struct event *ev, int res, short count);
  - **修复**：
    ```c
    // 原错误调用
    event_del_nolock_(...);
    // 修正为项目前缀版本
    lu_event_del_nolock_(...);
    ```

- [ ] **`evmap_signal_active_` 实现缺失**
  - **问题**：头文件声明但无具体实现
  - **修复步骤**：
    1. 在`lu_evmap.c`中添加实现：
    ```c
    void evmap_signal_active_(lu_event_base_t *base, int sig, int ncalls) {
        // 遍历信号映射表并激活事件
        LU_EVBASE_LOCK(base);
        lu_evmap_signal_t *ctx;
        GET_SIGNAL_SLOT(ctx, &base->sigmap, sig, evmap_signal);
        if (ctx) {
            LU_EVENT_LIST_ACTIVE(&ctx->events, ncalls);
        }
        LU_EVBASE_UNLOCK(base);
    }
    ```
    2. 更新头文件声明一致性检查

---

### 其他高优先级待办项
```markdown
- [X] **`LU_EVBASE_ASSERT_LOCKED` 宏未定义**
  - 现象：`warning: implicit declaration of function 'LU_EVBASE_ASSERT_LOCKED'`
  - 修复：在`lu_event-internal.h`添加：
    ```c
    #define LU_EVBASE_ASSERT_LOCKED(base) \
        LU_EVUTIL_ASSERT((base)->th_owner == lu_evthread_get_id_())
    ```

- [X] **函数指针类型转换警告**
  - 现象：`ISO C forbids conversion of function pointer to object pointer`
  - 修复：使用了uintptr_t进行转换

- [X] **`lu_epoll_init` 等epoll函数未实现**
  - 现象：`warning: ‘lu_epoll_init’ used but never defined`
  - 方案：创建`src/lu_epoll.c`实现所有epoll相关后端
# Development Log

## 2025-1-17

- [X] 完成了luevent内部的lu_mm_pool 内存池的修改，lu_mm_pool内存池参考了ngx内存池
- [X] 添加了GTest来进行单元测试
- [ ] 完成luevent的基本功能，包括事件的创建、订阅、发布、删除等

## 2025-1-18 

- [X] 添加了luevent-memory-pool的错误码

## 2025-1-19

- [X] 修复了lu_errorn中错误表(lu_hash_table)的内存泄露 ，entry(lu_error_info_t)下的error_msg(const char*)内存泄露未free
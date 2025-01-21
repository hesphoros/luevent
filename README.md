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
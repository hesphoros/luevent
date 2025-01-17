# 错误记录
## 2025-1-18

- 错误记录：LOG MODEL  ERROR FATAL 中出现内存泄漏，且一直在控制台循环输出 疑似溢出
- 问题原因： 日志模型中，在调用errorv和fatal时，使用了LU_EV_NORETURN 宏，导致函数无法正常返回，导致内存泄漏。

- 错误代码：

~~~c++

#define LU_EV_NORETURN          __attribute__((noreturn))

LU_EVENT_EXPORT_SYMBOL void lu_event_errorv(const char* file, int line, const char* fmt, ...)LU_EV_CHECK_FMT(3,4) LU_EV_NORETURN;
LU_EVENT_EXPORT_SYMBOL void lu_event_fatal(const char* file, int line, const char* fmt, ...) LU_EV_CHECK_FMT(3,4) LU_EV_NORETURN;
~~~

使用了LU_EV_NORETURN 宏，导致函数无法正常返回，导致内存泄漏。

- 解决方案：

将LU_EV_NORETURN 宏去掉，使得函数正常返回，并修复内存泄漏。
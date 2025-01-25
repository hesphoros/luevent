#ifndef _LU_THREAD_INCLUDED_H_
#define _LU_THREAD_INCLUDED_H_

#include <lu_visibility.h>

#ifdef __cplusplus
extern "C" {
#endif


#if ! defined(LU_EVENT__DISABLE_THREAD_SUPPORT)
/** 此结构描述了线程库用于锁定的接口。它用于告诉 evthread_set_lock_callbacks() 
 * 如何在此平台上使用锁定。
*/
typedef struct lu_evthread_lock_callback_s {
    int lock_api_version;//TODELETE:
    unsigned supported_lock_types;//TODELETE:
    void* (*alloc)(unsigned lock_type);
    void (*free)(void* lock,unsigned lock_type);
    int (*lock)(unsigned mode,void * lock);
    int (*unlock)(unsigned mode,void * lock);
}lu_evthread_lock_callback_t;

/** Sets a group of functions that Libevent should use for locking.
 * For full information on the required callback API, see the
 * documentation for the individual members of evthread_lock_callbacks.
 *
 * Note that if you're using Windows or the Pthreads threading library, you
 * probably shouldn't call this function; instead, use
 * evthread_use_windows_threads() or evthread_use_posix_threads() if you can.
 */
LU_EVENT_EXPORT_SYMBOL
    int lu_evthread_set_lock_callbacks(const lu_evthread_lock_callback_t *);


struct timeval;

typedef struct lu_evthread_condition_callbacks_s
{
    int condition_api_version;

    void* (*alloc_condition)(unsigned condition_type);
    void* (*free_condition)(void* cond);
    /// @brief 函数用于向条件变量发送信号。仅在持有条件的关联锁时才会调用此函数。
    /// @param cond 
    /// @param broadcast 如果“broadcast”为 1，则应唤醒所有等待“cond”的线程；否则，只有一个
    /// @return 成功返回 0，失败返回 -1。
    void* (*signal_condition)(void* cond,int broadcast);
    /** 等待条件变量的函数。调用此函数时将保持锁定“lock”；
     * 在等待条件发出信号时应释放该锁定，并且当此函数返回时应再次保持该锁定。
     * 如果提供了超时，则等待事件发出信号的间隔为秒；如果为 NULL，则函数应无限期等待。
     * 如果发生错误，则函数应返回 -1；
     * 如果条件发出信号，则返回 0；如果超时，则返回 1。*/
    void* (*wait_condition)(void* cond,void* lock,const struct timeval* tv);

}lu_evthread_condition_callbacks_t;

LU_EVENT_EXPORT_SYMBOL
    int lu_evthread_set_condition_callbacks(const lu_evthread_condition_callbacks_t*);
#endif /* ! defined(LU_EVENT__DISABLE_THREAD_SUPPORT) */

#ifdef __cplusplus
}
#endif

#endif /* _LU_THREAD_INCLUDED_H_ */
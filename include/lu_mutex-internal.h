#ifndef THREAD_SAFETY_ANALYSIS_MUTEX_H
#define THREAD_SAFETY_ANALYSIS_MUTEX_H

#include <pthread.h>  // POSIX 线程库
#include <stdbool.h>   // 布尔类型支持

// 线程安全属性宏（仅适用于 clang）
#if defined(__clang__) && (!defined(SWIG))
#define THREAD_ANNOTATION_ATTRIBUTE__(x)   __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x)   // 无操作
#endif

// 线程安全属性宏定义
#define CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(capability(x))

#define SCOPED_CAPABILITY \
  THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

#define GUARDED_BY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

#define PT_GUARDED_BY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))

#define ACQUIRED_BEFORE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquired_before(__VA_ARGS__))

#define ACQUIRED_AFTER(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquired_after(__VA_ARGS__))

#define REQUIRES(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))

#define REQUIRES_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(requires_shared_capability(__VA_ARGS__))

#define ACQUIRE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))

#define ACQUIRE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquire_shared_capability(__VA_ARGS__))

#define RELEASE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

#define RELEASE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(release_shared_capability(__VA_ARGS__))

#define RELEASE_GENERIC(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(release_generic_capability(__VA_ARGS__))

#define TRY_ACQUIRE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))

#define TRY_ACQUIRE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_shared_capability(__VA_ARGS__))

#define EXCLUDES(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

#define ASSERT_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(assert_capability(x))

#define ASSERT_SHARED_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(assert_shared_capability(x))

#define RETURN_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(lock_returned(x))

#define NO_THREAD_SAFETY_ANALYSIS \
  THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)


// 互斥锁结构体定义，使用 POSIX 线程库中的 `pthread_mutex_t`
typedef pthread_mutex_t Mutex;

// 线程安全的锁操作接口定义（不使用 C++ 的类和成员函数）

// 初始化互斥锁
int Mutex_Init(Mutex* mtx) {
    return pthread_mutex_init(mtx, NULL);
}

// 销毁互斥锁
int Mutex_Destroy(Mutex* mtx) {
    return pthread_mutex_destroy(mtx);
}

// 获取互斥锁
int Mutex_Lock(Mutex* mtx) {
    return pthread_mutex_lock(mtx);
}

// 获取共享锁（如果有读写锁机制，则用此函数）
int Mutex_ReaderLock(Mutex* mtx) {
    // 假设这里使用的仅为普通互斥锁，没有读写锁机制
    return pthread_mutex_lock(mtx);  // 对于 C 语言中的互斥锁，读写锁通常是通过自定义锁机制来实现的
}

// 解锁互斥锁
int Mutex_Unlock(Mutex* mtx) {
    return pthread_mutex_unlock(mtx);
}

// 释放共享锁
int Mutex_ReaderUnlock(Mutex* mtx) {
    return pthread_mutex_unlock(mtx);  // 共享锁也用普通的互斥锁来解锁
}

// 尝试获取互斥锁
int Mutex_TryLock(Mutex* mtx) {
    return pthread_mutex_trylock(mtx);
}

// 尝试获取共享锁
int Mutex_ReaderTryLock(Mutex* mtx) {
    return pthread_mutex_trylock(mtx);  // 和普通锁一样，尝试获取
}


// 互斥锁生命周期管理结构体
typedef struct {
    Mutex* mutex;
    bool locked;
} MutexLocker;

// 初始化锁定管理
int MutexLocker_Init(MutexLocker* locker, Mutex* mtx) {
    locker->mutex = mtx;
    locker->locked = false;
    return Mutex_Lock(mtx);  // 默认构造时锁定互斥锁
}

// 销毁锁定管理
void MutexLocker_Destroy(MutexLocker* locker) {
    if (locker->locked) {
        Mutex_Unlock(locker->mutex);
    }
}

// 锁定管理
int MutexLocker_Lock(MutexLocker* locker) {
    int result = Mutex_Lock(locker->mutex);
    if (result == 0) {
        locker->locked = true;
    }
    return result;
}

// 解锁管理
int MutexLocker_Unlock(MutexLocker* locker) {
    int result = Mutex_Unlock(locker->mutex);
    if (result == 0) {
        locker->locked = false;
    }
    return result;
}

// 宏简化：使用 `MutexLocker` 来管理锁
#define MUTEX_LOCK(locker, mtx) \
    MutexLocker_Init(locker, mtx)

#define MUTEX_UNLOCK(locker) \
    MutexLocker_Destroy(locker)

#endif  // THREAD_SAFETY_ANALYSIS_MUTEX_H

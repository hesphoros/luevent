#include "lu_util.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h> 
#include <pthread.h>
#include <assert.h>
#include <arpa/inet.h>
#include <errno.h>
#include "lu_hash.h"
#include "lu_memory_manager.h"


//缓存存储
typedef struct lu_cached_sock_errs_entry_s {
    int code;
    char *msg;
    UT_hash_handle hh;  // 用于 uthash 哈希表的处理
}lu_cached_sock_errs_entry_t;

static pthread_mutex_t linux_socket_errors_lock_ = PTHREAD_MUTEX_INITIALIZER;
static lu_cached_sock_errs_entry_t  *lu_cached_sock_errs_map_ = NULL; 

int lu_evutil_snprintf(char *str, size_t size, const char *format,...){
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = lu_evutil_vsnprintf(str, size, format, ap);
    va_end(ap);
    return ret;
}

int lu_evutil_vsnprintf(char *str, size_t size, const char *format, va_list ap){
    int ret;
    if(!size || !str)
        return 0;
    ret = vsnprintf(str, size, format, ap);
    // 如果输出被截断，保证字符串以 '\0' 结束
    // if output is truncated, ensure string ends with '\0'
    if (ret >= (int)size) {
        str[size - 1] = '\0';
    }

    str[size-1] = '\0';
    return ret;
}

int lu_evutil_socket_geterror(lu_evutil_socket_t sock_t){
    int optval ;
    lu_evutil_socklen_t optvallen = sizeof(optval);
    int err = errno;
    // 使用 getsockopt 获取最后的错误信息
      // 检查是否是非阻塞套接字操作错误（EAGAIN 或 EWOULDBLOCK）
    if ((err == EAGAIN || err == EWOULDBLOCK) && sock_t >= 0) {
         // 获取套接字的 SO_ERROR 错误
        if (getsockopt(sock_t, SOL_SOCKET, SO_ERROR, (void*)&optval, &optvallen) < 0)
            return errno;  
        // 如果 SO_ERROR 非零，表示有实际的套接字错误
        if (optval)
            return optval;
    }
    
    return err;   
}

const char *lu_evutil_socket_error_to_string(int errcode){
    lu_cached_sock_errs_entry_t *errs, *newerr;
    char *msg = NULL;
    // 锁定缓存，保证线程安全
    pthread_mutex_lock(&linux_socket_errors_lock_);

    // 查找缓存中的错误信息
    HASH_FIND_INT(lu_cached_sock_errs_map_, &errcode, errs);
    if (errs) {
        msg = errs->msg;
        pthread_mutex_unlock(&linux_socket_errors_lock_);
        return msg;
    }

    // 如果缓存中没有，获取标准错误信息
    if (errcode == EAGAIN || errcode == EWOULDBLOCK) {
        msg = strdup("Operation would block");
    } else {
        // 使用 strerror 获取标准错误消息
        msg = strdup(strerror(errcode));
    }

    // 如果 strdup 失败
    if (!msg) {
        msg = strdup("Memory allocation failed during socket error");
        pthread_mutex_unlock(&linux_socket_errors_lock_);
        return msg;
    }

    // 创建新的缓存条目
    newerr = (lu_cached_sock_errs_entry_t *)mm_malloc(sizeof(lu_cached_sock_errs_entry_t));
    if (!newerr) {
        free(msg);
        msg = mm_strdup("malloc failed during socket error");
        pthread_mutex_unlock(&linux_socket_errors_lock_);
        return msg;
    }

    newerr->code = errcode;
    newerr->msg = msg;

    // 插入哈希表中
    HASH_ADD_INT(lu_cached_sock_errs_map_, code, newerr);

    pthread_mutex_unlock(&linux_socket_errors_lock_);
    return msg;
}
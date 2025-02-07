#include "lu_util.h"
#include <stdio.h>

#include <pthread.h>
#include <assert.h>

#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "lu_memory_manager.h"
#include "lu_hash_table-internal.h"
#include "lu_erron.h"
#include "lu_log-internal.h"
#define _GNU_SOURCE
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>



lu_hash_table_t* lu_cached_sock_errs_map;

//缓存存储 socket 错误信息
typedef struct lu_cached_sock_errs_entry_s {
    int code;
    char *msg;
}lu_cached_sock_errs_entry_t;

static pthread_mutex_t linux_socket_errors_lock_ = PTHREAD_MUTEX_INITIALIZER;


static lu_hash_table_t  *lu_cached_sock_errs_map_ = NULL;



// 析构函数，释放缓存
__attribute__((destructor)) void lu_util_sock_hash_table_destructor(void) {
    lu_hash_table_destroy(lu_cached_sock_errs_map_);  // 清理哈希表
}

__attribute__((constructor)) void lu_util_sock_hash_table_constructor(void) {
    lu_cached_sock_errs_map_ = lu_hash_table_init(LU_HASH_TABLE_DEFAULT_SIZE);
}



// 内联函数实现
void lu_evutil_assert_impl(const char *file, int line, const char *cond, const char *func) {
    fprintf(stderr, "Assertion %s failed in %s:%d\n", cond, file, line);
    LU_EVENT_LOG_ERRORX(LU_EVENT_ERROR_ABORT_, "%s:%d: Assertion %s failed in %s", file, line, cond, func);
    abort();
}


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
    if (!str || size == 0 || !format) {
        return -1;
    }
    ret = vsnprintf(str, size, format, ap);
    // 如果输出被截断，保证字符串以 '\0' 结束
    // if output is truncated, ensure string ends with '\0'
    if ((ret < 0 || ret >= (int)size)){
        str[size - 1] = '\0';
    }


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


    errs= (lu_cached_sock_errs_entry_t*)LU_HASH_TABLE_FIND(lu_cached_sock_errs_map_,errcode);
    // 查找缓存中的错误信息

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

    // 插入缓存
    LU_HASH_TABLE_INSERT(lu_cached_sock_errs_map_, errcode, newerr);

    pthread_mutex_unlock(&linux_socket_errors_lock_);
    return msg;
}


static int
lu_evutil_issetugid(void)
{

    // 检查当前进程的有效用户 ID（euid）和有效组 ID（egid）
    // 如果 euid 不等于 real uid 或 egid 不等于 real gid，说明进程已经调用了 setuid() 或 setgid()
    if (getuid() != geteuid() || getgid() != getegid()) {
        return 1;  // 返回 1，表示进程已经调用了 setuid() 或 setgid()
    }
    return 0;  // 否则返回 0，表示进程未调用 setuid() 或 setgid()
}


const char *lu_evutil_getenv_(const char *varname)
{
    //如果是非 root 用户，则不允许获取环境变量
    if (lu_evutil_issetugid())
		return NULL;
    char * value = getenv(varname);
    if (value == NULL) {
        return NULL;
        LU_EVENT_LOG_WARN("getenv(%s) failed", varname);
    }
    return value;

}

/* 大写和小写表用于将字符映射到大写/小写
* 等价物。 */
/* Upper-casing and lowercasing tables to map characters to upper/lowercase
 * equivalents. */
static const unsigned char LU_EVUTIL_TOUPPER_TABLE[256] = {
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
  64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
  80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
  96,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
  80,81,82,83,84,85,86,87,88,89,90,123,124,125,126,127,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
  192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
  224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
};
static const unsigned char LU_EVUTIL_TOLOWER_TABLE[256] = {
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
  64,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,91,92,93,94,95,
  96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
  192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
  224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
};

char LU_EVUTIL_TOLOWER_(char c){
	return ((char)LU_EVUTIL_TOLOWER_TABLE[(lu_uint8_t)c]);
}
char LU_EVUTIL_TOUPPER_(char c)
{
	return ((char)LU_EVUTIL_TOUPPER_TABLE[(lu_uint8_t)c]);
}

int lu_evutil_check_dict_file_exist(const char *path){
    if (path == NULL || strlen(path) == 0) {
        return LU_ERROR_INVALID_PATH;
    }
        struct stat sb;
    if (stat(path, &sb) == 0) {
        return 0; // 文件存在
    }
        return -1; // 文件不存在
}

int lu_evutil_check_contain_directory(const char *filename){
    if (filename == NULL || strlen(filename) == 0) {
        return LU_ERROR_INVALID_PATH;
    }
    while(*filename){
        if(*filename == '/' || *filename == '\\'){
            return 1;//包含目录
        }
        filename++;
    }
    return 0;
}

const char* lu_evutil_get_directory(const char *filename,char * out_buf,size_t out_buffer_size)
{
    const char *last_slash_pos = NULL;
    const char *p = filename;

    // 找到最后一个斜杠的位置
    while (*p) {
        if (*p == '/' || *p == '\\') {
            last_slash_pos = p;
        }
        p++;
    }
    //如果没有找到目录分隔符，则返回原路径
    if(!last_slash_pos){
        return filename;
    }

      // 计算目录部分的长度（即最后一个斜杠之前的部分）
    size_t dir_len = last_slash_pos - filename;

    // 如果目录部分长度大于或等于输出缓冲区大小，返回错误
    if (dir_len >= out_buffer_size) {
        printf("ERROR: out buffer size is not enough to hold the directory\n");
        return NULL;
    }

      // 将目录部分复制到输出缓冲区，并确保以空字符结束
    strncpy(out_buf, filename, dir_len);
    out_buf[dir_len] = '\0';
   return out_buf;
}


int lu_evutil_create_dictionay(const char * path){

    if(lu_evutil_check_dict_file_exist(path) == 0){
        return 0; // 文件存在，不再创建
    }

    if (path == NULL || strlen(path) == 0) {
        return LU_ERROR_INVALID_PATH;
    }

    // 创建目录
    char temp_path[1024];
    snprintf(temp_path, sizeof(temp_path), "%s", path);

    // 去掉路径末尾的斜杠
    size_t len = strlen(temp_path);
    if (temp_path[len - 1] == '/') {
        temp_path[len - 1] = '\0';
    }

    // 尝试创建目录
    if (mkdir(temp_path, 0755) == 0) {
        return 0; // 成功创建
    }

    // 如果目录创建失败，且是因为父目录不存在
    if (errno == ENOENT) {
        // 递归创建父目录
        char *last_slash = strrchr(temp_path, '/');
        if (last_slash) {
            *last_slash = '\0'; // 切割路径
            if (lu_evutil_create_dictionay(temp_path) == 0) {
                // 父目录创建成功后，再次尝试创建目标目录
                return mkdir(path, 0755) == 0 ? 0 : -1;
            }
        }
    }

    return -1; // 目录创建失败
}


int lu_evutil_make_socket_closeonexec(lu_evutil_socket_t fd){
    int flags;
    if((flags = fcntl(fd, F_GETFD, NULL)) < 0){
        LU_EVENT_LOG_WARN("fcntl(%d, F_GETFD)", fd);
        return -1;
    }
    /**
     * FD_CLOEXEC 标志的作用：它告诉操作系统，当调用 exec() 系列函数（如 execvp()、execl() 等）时，
     * 自动关闭该文件描述符。这样做通常是为了避免在执行子进程时，
     * 父进程持有的文件描述符被传递到子进程中，造成资源泄漏或不必要的文件打开。
     */
    if (!(flags & FD_CLOEXEC)) {
		if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1) {
			LU_EVENT_LOG_WARN("fcntl(%d, F_SETFD)", fd);
			return -1;
		}
	}

    return 0;
}


int lu_evutil_make_internal_pipe_(lu_evutil_socket_t fd[2]){
    if(pipe(fd) == 0){
        if(lu_evutil_fast_socket_nonblocking(fd[0]) < 0 ||
                lu_evutil_fast_socket_nonblocking(fd[1]) < 0 ||
                lu_evutil_fast_socket_closeonexec(fd[0]) < 0 ||
                lu_evutil_fast_socket_closeonexec(fd[1]) < 0){
                close(fd[0]);
                close(fd[1]);
                return -1;
            }
        return 0;
    }else{
        LU_EVENT_LOG_WARN("%s pipe",__func__);
        return -1;
    }

}

int lu_evutil_fast_socket_nonblocking(lu_evutil_socket_t fd){
    if(fcntl(fd, F_SETFL, O_NONBLOCK) == -1){
        LU_EVENT_LOG_WARN("fcntl(%d,F_SETFL)",fd);
        return -1;
    }
    return 0;
}


int lu_evutil_fast_socket_closeonexec(lu_evutil_socket_t fd){
    if(fcntl(fd, F_SETFD, FD_CLOEXEC) == -1){
        LU_EVENT_LOG_WARN("fcntl(%d,F_SETFD)",fd);
        return -1;
    }
    return 0;
}




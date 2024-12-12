#include "lu_erron.h"
#include "lu_memory_manager.h"
#include "lu_mutex-internal.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
 



static const char* lu_error_strings_global[LU_MAX_ERROR_CODE + 1] = {
    [LU_ERROR_OPERATION_NOT_PERMITTED] = "Operation not permitted",
    [LU_ERROR_NO_SUCH_FILE_OR_DIRECTORY] = "No such file or directory",
    [LU_ERROR_NO_SUCH_PROCESS] = "No such process",
    [LU_ERROR_INTERRUPTED_SYSTEM_CALL] = "Interrupted system call",
    [LU_ERROR_INPUT_OUTPUT_ERROR] = "Input/output error",
    [LU_ERROR_NO_SUCH_DEVICE_OR_ADDRESS] = "No such device or address",
    [LU_ERROR_ARGUMENT_LIST_TOO_LONG] = "Argument list too long",
    [LU_ERROR_EXEC_FORMAT_ERROR] = "Exec format error",
    [LU_ERROR_BAD_FILE_DESCRIPTOR] = "Bad file descriptor",
    [LU_ERROR_NO_CHILD_PROCESSES] = "No child processes",
    [LU_ERROR_TRY_AGAIN] = "Try again",
    [LU_ERROR_OUT_OF_MEMORY] = "Out of memory",
    [LU_ERROR_PERMISSION_DENIED] = "Permission denied",
    [LU_ERROR_BAD_ADDRESS] = "Bad address",
    [LU_ERROR_BLOCK_DEVICE_REQUIRED] = "Block device required",
    [LU_ERROR_DEVICE_OR_RESOURCE_BUSY] = "Device or resource busy",
    [LU_ERROR_FILE_EXISTS] = "File exists",
    [LU_ERROR_CROSS_DEVICE_LINK] = "Cross-device link",
    [LU_ERROR_NO_SUCH_DEVICE] = "No such device",
    [LU_ERROR_NOT_A_DIRECTORY] = "Not a directory",
    [LU_ERROR_IS_A_DIRECTORY] = "Is a directory",
    [LU_ERROR_INVALID_ARGUMENT] = "Invalid argument",
    [LU_ERROR_FILE_TABLE_OVERFLOW] = "File table overflow",
    [LU_ERROR_TOO_MANY_OPEN_FILES] = "Too many open files",
    [LU_ERROR_NOT_A_TYPEWRITER] = "Not a typewriter",
    [LU_ERROR_TEXT_FILE_BUSY] = "Text file busy",
    [LU_ERROR_FILE_TOO_LARGE] = "File too large",
    [LU_ERROR_NO_SPACE_LEFT_ON_DEVICE] = "No space left on device",
    [LU_ERROR_ILLEGAL_SEEK] = "Illegal seek",
    [LU_ERROR_READ_ONLY_FILE_SYSTEM] = "Read-only file system",
    [LU_ERROR_TOO_MANY_LINKS] = "Too many links",
    [LU_ERROR_BROKEN_PIPE] = "Broken pipe",
    [LU_ERROR_MATH_ARGUMENT_OUT_OF_DOMAIN_OF_FUNCTION] = "Math argument out of domain of function",
    [LU_ERROR_RESULT_TOO_REPRESENTABLE] = "Result too representable"
};
 


// 错误信息结构体
typedef struct lu_error_info_s{
    int error_code;       // 错误码
    const char* error_message;  // 错误信息
    int is_loaded;        // 标记错误信息是否已加载
} lu_error_info_t;


// 哈希表
static lu_error_info_t* lu_error_hash_table[LU_HASH_TABLE_SIZE] = {0};

// 声明一个全局 Mutex 对象，来保护 error_table 的线程安全
static Mutex error_table_mutex;
 

const char* get_error_message_(int error_code);
const char* load_error_string_(int error_code);
unsigned int hash_(int error_code);
lu_error_info_t* get_or_create_error_entry_(int index)  REQUIRES(error_table_mutex);
void        cleanup_error_table_(void) REQUIRES(error_table_mutex);
static void initialize_error_table_(void) REQUIRES(error_table_mutex);


// 错误信息的加载函数
const char* load_error_string_(int error_code) {
    switch (error_code) {
        case 0x100: return "Operation not permitted";
        case 0x101: return "No such file or directory";
        case 0x102: return "No such process";
        case 0x103: return "Interrupted system call";
        case 0x104: return "Input/output error";
        case 0x105: return "No such device or address";
        case 0x106: return "Argument list too long";
        case 0x107: return "Exec format error";
        case 0x108: return "Bad file descriptor";
        case 0x109: return "No child processes";
        case 0x10A: return "Try again";
        case 0x10B: return "Out of memory";
        case 0x10C: return "Permission denied";
        case 0x10D: return "Bad address";
        case 0x10E: return "Block device required";
        case 0x10F: return "Device or resource busy";
        case 0x110: return "File exists";
        case 0x111: return "Cross-device link";
        case 0x112: return "No such device";
        case 0x113: return "Not a directory";
        case 0x114: return "Is a directory";
        case 0x115: return "Invalid argument";
        case 0x116: return "File table overflow";
        case 0x117: return "Too many open files";
        case 0x118: return "Not a typewriter";
        case 0x119: return "Text file busy";
        case 0x11A: return "File too large";
        case 0x11B: return "No space left on device";
        case 0x11C: return "Illegal seek";
        case 0x11D: return "Read-only file system";
        case 0x11E: return "Too many links";
        case 0x11F: return "Broken pipe";
        case 0x120: return "Math argument out of domain of function";
        case 0x121: return "Result too representable";
        default: return "Unknown error";
    }
}
 
unsigned int hash_(int error_code) {
    int hash = error_code % LU_HASH_TABLE_SIZE;
    
    printf("hash_ is %d\n", error_code);
    return hash;
}



// 错误码字符串访问函数 采用数组形式
const char* lu_get_error_string(int errno) {
    if (errno < 0 || errno > LU_MAX_ERROR_CODE) {
        static char buffer[64];  // 用于返回更详细的错误信息
        snprintf(buffer, sizeof(buffer), "Unknown error: %d", errno);
        return buffer;
    }

    #if SUPPORTS_NON_TRIVIAL_DESIGNATED_INITIALIZERS
    return lu_error_strings_global[errno];
    #else
    return get_error_message_(errno);
    #endif
}

const char* get_error_message_(int error_code) {
    
    return lu_error_strings_global[error_code];
    
}




lu_error_info_t* get_or_create_error_entry_(int index)  REQUIRES(error_table_mutex) {
    //unsigned int index = hash_(error_code);  // 使用index计算哈希值
    printf("Enter get_or_create_error_entry_ function\n");
    // 如果哈希表条目为空，则初始化
    if (!lu_error_hash_table[index]) {
        lu_error_hash_table[index] = (lu_error_info_t*) mm_malloc(sizeof(lu_error_info_t));
        if (!lu_error_hash_table[index]) {
            fprintf(stderr, "Memory allocation failed for error entry\n");
            printf("Memory allocation failed for error entry\n");
            exit(EXIT_FAILURE);
        }
        memset(lu_error_hash_table[index], 0, sizeof(lu_error_info_t));  // 初始化条目
    }

    return lu_error_hash_table[index];
}


// 错误表清理
void cleanup_error_table_(void) REQUIRES(error_table_mutex)  {
    MutexLocker lock;  // 自动加锁
    MUTEX_LOCK(&lock,&error_table_mutex);  // 保护哈希表的访问
    for (int i = 0; i < LU_HASH_TABLE_SIZE; i++) {
        if (lu_error_hash_table[i]) {
            mm_free(lu_error_hash_table[i]);  // 释放内存
            lu_error_hash_table[i] = NULL;  // 防止重复释放
        }
    }
    MUTEX_UNLOCK(&error_table_mutex);  // 解锁
}


static void initialize_error_table_(void) REQUIRES(error_table_mutex) {
    static int initialized = 0;
    MutexLocker lock;  // 自动加锁
   
    if (!initialized) {
        if (atexit(cleanup_error_table_) != 0) {
            fprintf(stderr, "Failed to register cleanup function\n");
            exit(EXIT_FAILURE);
        }
        initialized = 1;
    }
    MUTEX_LOCK(&lock,&error_table_mutex);  // 保护哈希表的访问
}

// 构造函数，用于初始化静态变量
__attribute__((constructor)) void error_table_initializer(void) REQUIRES(error_table_mutex)  {
    initialize_error_table_();
}


 

__attribute__((destructor)) void error_table_finalizer(void) NO_THREAD_SAFETY_ANALYSIS  {
    cleanup_error_table_();  // 清理哈希表
    
}

// 错误码字符串哈希表访问函数
const char* lu_get_error_string_hash(int errno)  ASSERT_CAPABILITY(&error_table_mutex) {
    if (errno < 0 || errno > LU_MAX_ERROR_CODE) {
        return "Unknown error";
    }

    unsigned int index = hash_(errno);  // 使用index计算哈希值
    printf("index is %d\n", index);

    MutexLocker lock;  // 自动加锁
    MUTEX_LOCK(&lock,&error_table_mutex);  // 保护哈希表的访问
    // 获取或创建哈希表条目的指针
    lu_error_info_t* entry = get_or_create_error_entry_(index);
    printf("entry is %p\n", entry);
    printf("entry->error_code is %d\n", entry->error_code);
    printf("entry->error_message is %s\n", entry->error_message);
    printf("entry->is_loaded is %d\n", entry->is_loaded);

    // 如果尚未加载错误信息，则进行加载
    if (!entry->is_loaded) {
        entry->error_code = errno;

         
        entry->error_message = get_error_message_(errno);
         

        entry->is_loaded = 1;  // 标记为已加载
    }
    MUTEX_UNLOCK(&error_table_mutex);  // 解锁

    return entry->error_message;
}

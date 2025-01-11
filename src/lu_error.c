#include "lu_erron.h"
#include "lu_memory_manager.h"
// #include "lu_mutex-internal.h"
#include <stdio.h>
#include "lu_util.h"
#include <string.h>
#include "lu_hash_table-internal.h"
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
 


// error info struct
typedef struct lu_error_info_s{
    int error_code;       // 错误码
    const char* error_message;  // 错误信息
    int is_loaded;        // 标记错误信息是否已加载
} lu_error_info_t;


// hash table for error strings
static lu_hash_table_t* lu_error_hash_table = NULL;

// 声明一个全局 Mutex 对象，来保护 error_table 的线程安全
static pthread_mutex_t  error_table_mutex;



static const char*   get_error_message_(int error_code);
static const char*         load_error_string_(int error_code);
static lu_error_info_t*    get_or_create_error_entry_(int error_code) ;
static void                cleanup_error_table_(void) ;
static void         initialize_error_table_(void) ;


// 错误信息的加载函数
static const char* load_error_string_(int error_code) {
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
 


// 错误码字符串访问函数 采用数组形式
const char* lu_get_error_string(int errno) {
    if (errno < 0 || errno > LU_MAX_ERROR_CODE) {
        static char buffer[64];  // 用于返回更详细的错误信息
        snprintf(buffer, sizeof(buffer), "Unknown error: %d", errno);
        return buffer;
    }
     
    return get_error_message_(errno);
     
}

static const char* get_error_message_(int error_code) {
    
    return lu_error_strings_global[error_code];
}




static lu_error_info_t* get_or_create_error_entry_(int error_code) {
   
   lu_error_info_t* entry = (lu_error_info_t*)LU_HASH_TABLE_FIND(lu_error_hash_table, error_code);

 // 如果哈希表中没有找到该条目，则创建新条目
    if (!entry) {
        entry = (lu_error_info_t*)mm_malloc(sizeof(lu_error_info_t));
        if (!entry) {
            fprintf(stderr, "Memory allocation failed for error entry\n");
            exit(EXIT_FAILURE);
        }
        memset(entry, 0, sizeof(lu_error_info_t));  // 初始化条目

        // 插入哈希表
        LU_HASH_TABLE_INSERT(lu_error_hash_table, error_code, entry);
    }

    return entry;
}


// 错误表清理
static void cleanup_error_table_(void)  {
    pthread_mutex_lock(&error_table_mutex);  // 加锁
    //清理 entries
      // 遍历哈希表并清理每个条目
    for (int i = 0; i < LU_MAX_ERROR_CODE + 1; i++) {
        lu_error_info_t* entry = LU_HASH_TABLE_FIND(lu_error_hash_table, i);  // 获取每个条目
        if (entry) {
            mm_free(entry);  // 释放条目的内存
            LU_HASH_TABLE_DELETE(lu_error_hash_table, i);  // 从哈希表中删除该条目
        }
    }

    LU_HASH_TABLE_DESTROY(lu_error_hash_table); // 销毁哈希表
    pthread_mutex_unlock(&error_table_mutex); 
    pthread_mutex_destroy(&error_table_mutex);  // 销毁锁
}

static int initialized = 0;
static void initialize_error_table_(void) {

  if (!initialized) {
        // 使用互斥锁保护初始化过程
        pthread_mutex_lock(&error_table_mutex);
        if (!initialized) {  // 再次检查
            if (pthread_mutex_init(&error_table_mutex, NULL) != 0) {
                fprintf(stderr, "Failed to initialize mutex\n");
                exit(EXIT_FAILURE);
            }
            // 初始化哈希表
            if (lu_error_hash_table == NULL) 
             lu_error_hash_table = LU_HASH_TABLE_INIT(LU_EVENT_HASH_TABLE_SIZE);
        
            initialized = 1;
        }
        pthread_mutex_unlock(&error_table_mutex);
    }
}

// constructor函数，用于初始化静态变量
__attribute__((constructor)) void error_table_initializer(void){
   
    initialize_error_table_();
}


 
// destructor函数，用于清理静态变量
__attribute__((destructor)) void error_table_finalizer(void) {
    cleanup_error_table_();  // 清理哈希表
    
}

// 错误码字符串哈希表访问函数
const char* lu_get_error_string_hash(int errno)  {
    if (errno < 0 || errno > LU_MAX_ERROR_CODE) {
        return "Unknown error";
    }
 
    int lock_status = pthread_mutex_lock(&error_table_mutex);
    if (lock_status != 0) {
        // printf("Failed to lock error_table_mutex: %d\n", lock_status);
        return "Lock failed";           
    }   

    lu_error_info_t* entry = get_or_create_error_entry_(errno);

    // 如果尚未加载错误信息，则进行加载 使用惰性加载机制
    if (!entry->is_loaded) {
        entry->error_code = errno;         
        entry->error_message = get_error_message_(errno);
        entry->is_loaded = 1;  // 标记为已加载
    }
    pthread_mutex_unlock(&error_table_mutex);  // 解锁~

    return entry->error_message;
}

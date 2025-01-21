#ifndef _TEST_LU_ERRON_HPP_
#define _TEST_LU_ERRON_HPP_

#include "test_common.h"

 // 测试 lu_get_error_string_hash 函数
TEST(LuErrorTest, GetErrorStringHash) {
    // 假设返回值是哈希值的字符串表示
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_OPERATION_NOT_PERMITTED), "Operation not permitted");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_NO_SUCH_FILE_OR_DIRECTORY), "No such file or directory");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_NO_SUCH_PROCESS), "No such process");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_INTERRUPTED_SYSTEM_CALL), "Interrupted system call");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_INPUT_OUTPUT_ERROR), "Input/output error");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_NO_SUCH_DEVICE_OR_ADDRESS), "No such device or address");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_ARGUMENT_LIST_TOO_LONG), "Argument list too long");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_EXEC_FORMAT_ERROR), "Exec format error");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_BAD_FILE_DESCRIPTOR), "Bad file descriptor");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_NO_CHILD_PROCESSES), "No child processes");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_TRY_AGAIN), "Try again");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_OUT_OF_MEMORY), "Out of memory");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_PERMISSION_DENIED), "Permission denied");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_BAD_ADDRESS), "Bad address");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_BLOCK_DEVICE_REQUIRED), "Block device required");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_DEVICE_OR_RESOURCE_BUSY), "Device or resource busy");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_FILE_EXISTS), "File exists");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_CROSS_DEVICE_LINK), "Cross-device link");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_NO_SUCH_DEVICE), "No such device");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_NOT_A_DIRECTORY), "Not a directory");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_IS_A_DIRECTORY), "Is a directory");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_INVALID_ARGUMENT), "Invalid argument");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_FILE_TABLE_OVERFLOW), "File table overflow");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_TOO_MANY_OPEN_FILES), "Too many open files");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_NOT_A_TYPEWRITER), "Not a typewriter");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_TEXT_FILE_BUSY), "Text file busy");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_FILE_TOO_LARGE), "File too large");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_NO_SPACE_LEFT_ON_DEVICE), "No space left on device");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_ILLEGAL_SEEK), "Illegal seek");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_READ_ONLY_FILE_SYSTEM), "Read-only file system");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_TOO_MANY_LINKS), "Too many links");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_BROKEN_PIPE), "Broken pipe");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_MATH_ARGUMENT_OUT_OF_DOMAIN_OF_FUNCTION), "Math argument out of domain of function");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_RESULT_TOO_REPRESENTABLE), "Result too representable");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_LUEVENT_MEMORY_POOL_OK), "Luevent MemoyPool OK");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_LUEVENT_MEMORY_POOL_ERROR), "Luevent MemoyPool ERROR");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_LUEVENT_MEMORY_POOL_AGAGIN), "Luevent MemoryPool AGAIN");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_LUEVENT_MEMORY_POOL_BUSY), "Luevent MemoryPool BUSY");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_LUEVENT_MEMORY_POOL_DONE), "Luevent MemoryPool DONE");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_LUEVENT_MEMORY_POOL_DECLINED), "Luevent MemoryPool DECLINED");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_LUEVENT_MEMORY_POOL_ABORT), "Luevent MemoryPool ABORT");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_INVALID_PATH), "INVALID PATH");
    EXPECT_STREQ(lu_get_error_string_hash(-1),"Unknown error: -1");
    EXPECT_STREQ(lu_get_error_string_hash(1000000),"Unknown error: 1000000");

}



 // 测试错误字符串
TEST(LuErrorTest, GetErrorString) {
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_OPERATION_NOT_PERMITTED), "Operation not permitted");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_NO_SUCH_FILE_OR_DIRECTORY), "No such file or directory");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_NO_SUCH_PROCESS), "No such process");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_INTERRUPTED_SYSTEM_CALL), "Interrupted system call");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_INPUT_OUTPUT_ERROR), "Input/output error");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_NO_SUCH_DEVICE_OR_ADDRESS), "No such device or address");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_ARGUMENT_LIST_TOO_LONG), "Argument list too long");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_EXEC_FORMAT_ERROR), "Exec format error");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_BAD_FILE_DESCRIPTOR), "Bad file descriptor");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_NO_CHILD_PROCESSES), "No child processes");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_TRY_AGAIN), "Try again");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_OUT_OF_MEMORY), "Out of memory");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_PERMISSION_DENIED), "Permission denied");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_BAD_ADDRESS), "Bad address");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_BLOCK_DEVICE_REQUIRED), "Block device required");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_DEVICE_OR_RESOURCE_BUSY), "Device or resource busy");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_FILE_EXISTS), "File exists");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_CROSS_DEVICE_LINK), "Cross-device link");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_NO_SUCH_DEVICE), "No such device");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_NOT_A_DIRECTORY), "Not a directory");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_IS_A_DIRECTORY), "Is a directory");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_INVALID_ARGUMENT), "Invalid argument");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_FILE_TABLE_OVERFLOW), "File table overflow");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_TOO_MANY_OPEN_FILES), "Too many open files");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_NOT_A_TYPEWRITER), "Not a typewriter");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_TEXT_FILE_BUSY), "Text file busy");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_FILE_TOO_LARGE), "File too large");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_NO_SPACE_LEFT_ON_DEVICE), "No space left on device");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_ILLEGAL_SEEK), "Illegal seek");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_READ_ONLY_FILE_SYSTEM), "Read-only file system");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_TOO_MANY_LINKS), "Too many links");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_BROKEN_PIPE), "Broken pipe");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_MATH_ARGUMENT_OUT_OF_DOMAIN_OF_FUNCTION), "Math argument out of domain of function");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_RESULT_TOO_REPRESENTABLE), "Result too representable");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_LUEVENT_MEMORY_POOL_OK), "Luevent MemoyPool OK");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_LUEVENT_MEMORY_POOL_ERROR), "Luevent MemoyPool ERROR");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_LUEVENT_MEMORY_POOL_AGAGIN), "Luevent MemoryPool AGAIN");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_LUEVENT_MEMORY_POOL_BUSY), "Luevent MemoryPool BUSY");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_LUEVENT_MEMORY_POOL_DONE), "Luevent MemoryPool DONE");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_LUEVENT_MEMORY_POOL_DECLINED), "Luevent MemoryPool DECLINED");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_LUEVENT_MEMORY_POOL_ABORT), "Luevent MemoryPool ABORT");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_INVALID_PATH), "INVALID PATH");
    EXPECT_STREQ(lu_get_error_string(-1),"Unknown error: -1");
    EXPECT_STREQ(lu_get_error_string(1000000),"Unknown error: 1000000");
}






#endif/*_TEST_LU_ERRON_HPP_*/
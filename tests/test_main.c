#include "lu_erron.h"
#include <gtest/gtest.h>

#include <stdio.h>jj

// 测试 lu_get_error_string 函数
TEST(LuErrorTest, GetErrorString) {
    // 假设这些错误码对应的错误信息是预先定义的
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_OPERATION_NOT_PERMITTED), "Operation not permitted");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_NO_SUCH_FILE_OR_DIRECTORY), "No such file or directory");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_NO_SUCH_PROCESS), "No such process");
    EXPECT_STREQ(lu_get_error_string(LU_ERROR_INTERRUPTED_SYSTEM_CALL),"Interrupted system call");
}

// 测试 lu_get_error_string_hash 函数
TEST(LuErrorTest, GetErrorStringHash) {
    // 假设返回值是哈希值的字符串表示
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_OPERATION_NOT_PERMITTED), "Operation not permitted");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_NO_SUCH_FILE_OR_DIRECTORY), "No such file or directory");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_NO_SUCH_PROCESS), "No such process");
    EXPECT_STREQ(lu_get_error_string_hash(LU_ERROR_INTERRUPTED_SYSTEM_CALL),"Interrupted system call");
}



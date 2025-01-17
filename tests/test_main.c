#include <lu_errorn.h>


#include <gtest/gtest.h>
#include <lu_errorn.h> 
#include <stdio.h>

// 测试 lu_get_error_string 函数
TEST(LuErrorTest, GetErrorString) {
    // 假设这些错误码对应的错误信息是预先定义的
    EXPECT_STREQ(lu_get_error_string(ERROR_CODE_1), "Error 1 Description");
    EXPECT_STREQ(lu_get_error_string(ERROR_CODE_2), "Error 2 Description");
    EXPECT_STREQ(lu_get_error_string(ERROR_CODE_3), "Error 3 Description");
}

// 测试 lu_get_error_string_hash 函数
TEST(LuErrorTest, GetErrorStringHash) {
    // 假设返回值是哈希值的字符串表示
    EXPECT_STREQ(lu_get_error_string_hash(ERROR_CODE_1), "Error 1 Hash");
    EXPECT_STREQ(lu_get_error_string_hash(ERROR_CODE_2), "Error 2 Hash");
    EXPECT_STREQ(lu_get_error_string_hash(ERROR_CODE_3), "Error 3 Hash");
}

// 你可以根据需要添加更多的测试用例

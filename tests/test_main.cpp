#include "lu_erron.h"
#include "lu_hash_table-internal.h"
#include "lu_util.h"
#include "lu_log-internal.h"
#include "lu_util.h"
#include <gtest/gtest.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//For test_hash_table
typedef struct {
        char name[50];       // 姓名
        int id;              // ID号
        char gender;         // 性别 ('M' = 男, 'F' = 女)
} Person;


static Person people[100] = {
    {"张三", 1001, 'M'}, {"李四", 1002, 'F'}, {"王五", 1003, 'M'},
    {"赵六", 1004, 'F'}, {"孙七", 1005, 'M'}, {"周八", 1006, 'F'},
    {"吴九", 1007, 'M'}, {"郑十", 1008, 'F'}, {"钱十一", 1009, 'M'},
    {"刘十二", 1010, 'F'}, {"陈十三", 1011, 'M'}, {"杨十四", 1012, 'F'},
    {"黄十五", 1013, 'M'}, {"吴十六", 1014, 'F'}, {"沈十七", 1015, 'M'},
    {"蒋十八", 1016, 'F'}, {"魏十九", 1017, 'M'}, {"宋二十", 1018, 'F'},
    {"张二十一", 1019, 'M'}, {"王二十二", 1020, 'F'}, {"李二十三", 1021, 'M'},
    {"周二十四", 1022, 'F'}, {"郑二十五", 1023, 'M'}, {"赵二十六", 1024, 'F'},
    {"钱二十七", 1025, 'M'}, {"孙二十八", 1026, 'F'}, {"陈二十九", 1027, 'M'},
    {"刘三十", 1028, 'F'}, {"蒋三十一", 1029, 'M'}, {"魏三十二", 1030, 'F'},
    {"杨三十三", 1031, 'M'}, {"黄三十四", 1032, 'F'}, {"沈三十五", 1033, 'M'},
    {"宋三十六", 1034, 'F'}, {"朱三十七", 1035, 'M'}, {"王三十八", 1036, 'F'},
    {"李三十九", 1037, 'M'}, {"周四十", 1038, 'F'}, {"赵四十一", 1039, 'M'},
    {"孙四十二", 1040, 'F'}, {"陈四十三", 1041, 'M'}, {"刘四十四", 1042, 'F'},
    {"蒋四十五", 1043, 'M'}, {"魏四十六", 1044, 'F'}, {"杨四十七", 1045, 'M'},
    {"黄四十八", 1046, 'F'}, {"沈四十九", 1047, 'M'}, {"宋五十", 1048, 'F'},
    {"张五十一", 1049, 'M'}, {"王五十二", 1050, 'F'}, {"李五十三", 1051, 'M'},
    {"周五十四", 1052, 'F'}, {"郑五十五", 1053, 'M'}, {"赵五十六", 1054, 'F'},
    {"钱五十七", 1055, 'M'}, {"孙五十八", 1056, 'F'}, {"陈五十九", 1057, 'M'},
    {"刘六十", 1058, 'F'}, {"蒋六十一", 1059, 'M'}, {"魏六十二", 1060, 'F'},
    {"杨六十三", 1061, 'M'}, {"黄六十四", 1062, 'F'}, {"沈六十五", 1063, 'M'},
    {"宋六十六", 1064, 'F'}, {"张六十七", 1065, 'M'}, {"王六十八", 1066, 'F'},
    {"李六十九", 1067, 'M'}, {"周七十", 1068, 'F'}, {"赵七十一", 1069, 'M'},
    {"孙七十二", 1070, 'F'}, {"陈七十三", 1071, 'M'}, {"刘七十四", 1072, 'F'},
    {"蒋七十五", 1073, 'M'}, {"魏七十六", 1074, 'F'}, {"杨七十七", 1075, 'M'},
    {"黄七十八", 1076, 'F'}, {"沈七十九", 1077, 'M'}, {"宋八十", 1078, 'F'},
    {"张八十一", 1079, 'M'}, {"王八十二", 1080, 'F'}, {"李八十三", 1081, 'M'},
    {"周八十四", 1082, 'F'}, {"郑八十五", 1083, 'M'}, {"赵八十六", 1084, 'F'},
    {"钱八十七", 1085, 'M'}, {"孙八十八", 1086, 'F'}, {"陈八十九", 1087, 'M'},
    {"刘九十", 1088, 'F'}, {"蒋九十一", 1089, 'M'}, {"魏九十二", 1090, 'F'},
    {"杨九十三", 1091, 'M'}, {"黄九十四", 1092, 'F'}, {"沈九十五", 1093, 'M'},
    {"宋九十六", 1094, 'F'}, {"张九十七", 1095, 'M'}, {"王九十八", 1096, 'F'},
    {"李九十九", 1097, 'M'}, {"周一百", 1098, 'F'}, {"郑一百零一", 1099, 'M'},
    {"赵一百零二", 1100, 'F'}
};



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

TEST(Testluhashtable, test_lu_hash_table) {
    lu_hash_table_t *ht = LU_HASH_TABLE_INIT(8);  // 初始化哈希表大小为 8
    EXPECT_EQ(ht->element_count, 0);               // 确保初始元素个数为 0
    EXPECT_EQ(ht->table_size, 8);                  // 确保初始表格大小为 8

    // 插入100个元素到哈希表
    for (size_t i = 0; i < 100; ++i) {
        LU_HASH_TABLE_INSERT(ht, people[i].id, &people[i]);
    }

    // 查找并验证这些元素是否正确插入
    for (int i = 0; i < 100; ++i) {  // 使用 0 到 99 的范围
        Person* found_person = (Person*)LU_HASH_TABLE_FIND(ht, people[i].id);
        EXPECT_EQ(found_person->id, people[i].id);
        EXPECT_STREQ(found_person->name, people[i].name);
    }

    LU_HASH_TABLE_DESTROY(ht);  // 销毁哈希表
}



TEST(Testluevutil,test_evutil_get_dict){
    const char * filename = "./luevent/luevent.log";
   
    char out_path[] = "./luevent";
    char buffer[10];
    EXPECT_STREQ(lu_evutil_get_directory(filename,buffer,sizeof(buffer)),out_path);
}

TEST(Testlulog,test_enable_default_file_log){
    //lu_enable_default_file_logging(NULL, LU_EVENT_LOG_LEVEL_DEBUG);
    lu_enable_default_file_logging("./log/luevent.log",LU_EVENT_LOG_LEVEL_DEBUG);
    const char* filename = "./luevent/luevent.log";
    EXPECT_EQ(lu_evutil_check_contain_directory(filename),1);
 
   
    char* str_debug = "debug log";
    LU_EVENT_LOG_DEBUGX("Test  %s ",str_debug);
    char* str_error = "error log";
    LU_EVENT_LOG_ERRORV("Test %s",str_error);
    char * str_warn = "warn log";
    LU_EVENT_LOG_WARN("Test  %s ",str_warn);
    char* str_msg = "msg log";
    LU_EVENT_LOG_MSGX("Test  %s ",str_msg);
    
}


TEST(LuevutilTest, CreateDictionary){
    EXPECT_EQ(lu_evutil_create_dictionay("test"), 0);
    EXPECT_EQ(lu_evutil_create_dictionay(NULL), LU_ERROR_INVALID_PATH);
    EXPECT_EQ(lu_evutil_create_dictionay(""), LU_ERROR_INVALID_PATH);
    EXPECT_EQ(lu_evutil_create_dictionay("log1/log2/log3"), 0);
}

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





int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    //test_hash();
    return RUN_ALL_TESTS();
    
}


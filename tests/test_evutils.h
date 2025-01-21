#ifndef TEST_EVUTILS_H
#define TEST_EVUTILS_H

#include "test_common.h"

TEST(LuevutilTest, CreateDictionary){
    EXPECT_EQ(lu_evutil_create_dictionay("test"), 0);
    EXPECT_EQ(lu_evutil_create_dictionay(NULL), LU_ERROR_INVALID_PATH);
    EXPECT_EQ(lu_evutil_create_dictionay(""), LU_ERROR_INVALID_PATH);
    EXPECT_EQ(lu_evutil_create_dictionay("test_dict1/test_dict2/test_dict3"), 0);
}

TEST(Testluevutil,test_evutil_get_dict){
    const char * filename = "./luevent/luevent.log";
    char out_path[] = "./luevent";
    char buffer[10];
    EXPECT_STREQ(lu_evutil_get_directory(filename,buffer,sizeof(buffer)),out_path);
}



#endif /* TEST_EVUTILS_H */
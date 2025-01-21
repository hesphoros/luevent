#ifndef TEST_LULOG_HPP
#define TEST_LULOG_HPP

#include "test_common.h"
#include <string>



TEST(Testlulog,test_enable_default_file_log){
    //lu_enable_default_file_logging(NULL, LU_EVENT_LOG_LEVEL_DEBUG);
    lu_enable_default_file_logging("./log/luevent.log",LU_EVENT_LOG_LEVEL_DEBUG);
    const char* filename = "./luevent/luevent.log";
    EXPECT_EQ(lu_evutil_check_contain_directory(filename),1);
 
   
    std::string str_debug = "debug log";
    LU_EVENT_LOG_DEBUGX("Test  %s ",str_debug.c_str());
    std::string str_error = "error log";
    LU_EVENT_LOG_ERRORV("Test  %s",str_error.c_str());
    std::string str_warn = "warn log";
    LU_EVENT_LOG_WARN("Test  %s ",str_warn.c_str());
    std::string str_msg = "msg log";
    LU_EVENT_LOG_MSGX("Test  %s ",str_msg.c_str());
    
}




#endif // TEST_LULOG_HPP
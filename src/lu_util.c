#include "lu_util.h"
#include <stdio.h>


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
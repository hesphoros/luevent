#ifndef LU_EVENT_HTTP_WEBSERVER_H
#define LU_EVENT_HTTP_WEBSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>

#include "lu_webserver_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LU_WEBSERVER_MAX_FD     65536 //  //最大文件描述符
#define LU_WEBSERVER_MAX_EVENTS 1024 //  //最大事件数
const int TIMESLOT = 5; // 最小超时时间

typedef struct lu_http_webserver_s{
    lu_http_webserver_t config;

    
}lu_http_webserver_t;

#ifdef __cplusplus
}
#endif

#endif /* LU_EVENT_HTTP_WEBSERVER_H */

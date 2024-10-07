#ifndef LU_EVENT_UTIL_H
#define LU_EVENT_UTIL_H

#ifdef _WIN32
#define evutil_socket_t intptr_t
#else
#define evutil_socket_t int
#endif


#endif
#ifndef _LU_EVENT_MAP_H_
#define _LU_EVENT_MAP_H_

#include "lu_util.h"
#include "lu_event_struct.h"
#include <sys/queue.h>

#ifdef __cplusplus
extern "C" {
#endif




struct event_dlist {
    TAILQ_HEAD(, lu_event_t) events;
};

/** evmap_io 列表的条目：记录想要在给定 fd
 * 上读取或写入的所有事件，以及每个事件的数量。
*/
typedef struct lu_evmap_io_s {
	struct event_dlist events;
	lu_uint16_t nread;
	lu_uint16_t nwrite;
	lu_uint16_t nclose;
} lu_evmap_io_t;



typedef struct lu_event_map_entry_s{
    lu_evutil_socket_t fd;
    struct lu_event_map_entry_s* next;
    //HT_ENTRY(event_map_entry) map_node;
    union
    {
        lu_evmap_io_t evmap_io_;//用于存储io事件
    }ent;

}lu_event_map_entry_t;

#ifdef __cplusplus
}
#endif

#endif /* _LU_EVENT_MAP_H_ */
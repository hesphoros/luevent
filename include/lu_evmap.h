#ifndef _LU_EVENT_MAP_H_
#define _LU_EVENT_MAP_H_

#include "lu_util.h"
// #include "lu_event_struct.h"

#include <sys/queue.h>
#include "lu_changelist-internal.h"

#ifdef __cplusplus
extern "C" {
#endif



//如果定义了LU_EVMAP_USE_HASHTABLE，则io map使用hash表，否则使用链表。

// //DELETEME:LU_EVMAP_USE_HASHTABLE 仅供测试使用
// //#define LU_EVMAP_USE_HASHTABLE

// #ifdef LU_EVMAP_USE_HASHTABLE
// #include "lu_hash_table-internal.h"
// #define HASH_TABLE_NO_CACHE_HASH_VALUES
// struct lu_event_map_entry_s ;
// lu_hash_table_t  *lu_event_io_map_t ;

// #define  lu_event_io_map_t  lu_hash_table_t*

// #else
// #define lu_event_io_map_t lu_event_signal_map_t
// //typedef struct lu_event_signal_map_s lu_event_io_map_t;
// #endif //LU_EVMAP_USE_HASHTABLE

typedef struct lu_event_signal_map_s lu_event_signal_map_t;
typedef struct lu_evmap_io_s lu_evmap_io_t;
typedef lu_event_signal_map_t lu_event_io_map_t;
typedef struct lu_event_map_entry_s lu_event_map_entry_t;
//#define lu_event_io_map_t lu_event_signal_map_t
LIST_HEAD(lu_event_dlist, lu_event_t);

/** evmap_io 列表的条目：记录想要在给定 fd
 * 上读取或写入的所有事件，以及每个事件的数量。
*/
typedef struct lu_evmap_io_s {
	struct lu_event_dlist events;
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




typedef struct lu_event_signal_map_s{
   /* An array of evmap_io * or of evmap_signal *; empty entries are
	 * set to NULL. */
	void **entries;
	/* The number of entries available in entries */
	int nentries;
}lu_event_signal_map_t;



void lu_evmap_io_initmap(lu_event_io_map_t* ctx);
void lu_evmap_siganl_initmap(lu_event_signal_map_t* ctx);
void lu_event_changelist_init(lu_event_changelist_t* ctx);

#ifdef __cplusplus
}
#endif

#endif /* _LU_EVENT_MAP_H_ */
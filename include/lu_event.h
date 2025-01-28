#ifndef LU_EVENT_H
#define LU_EVENT_H

#include "lu_event-internal.h"
#include "lu_visibility.h"

typedef void (*lu_event_callback_fn)(lu_evutil_socket_t, short, void *);

lu_event_base_t*    lu_event_base_new(void);
lu_event_config_t*  lu_event_config_new(void);
lu_event_base_t*    lu_event_base_new_with_config(lu_event_config_t* );
void                lu_event_config_free(lu_event_config_t*);

LU_EVENT_EXPORT_SYMBOL int lu_event_assign(lu_event_t *ev, lu_event_base_t *base, lu_evutil_socket_t fd, short events, lu_event_callback_fn callback, void *callback_arg);


#endif  //LU_EVENT_H
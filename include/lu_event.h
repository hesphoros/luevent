#ifndef LU_EVENT_H
#define LU_EVENT_H

#include "lu_event-internal.h"

lu_event_base_t*    lu_event_base_new(void);
lu_event_config_t*  lu_event_config_new(void);
lu_event_base_t*    lu_event_base_new_with_config(lu_event_config_t* );
void                lu_event_config_free(lu_event_config_t*);

#endif  //LU_EVENT_H
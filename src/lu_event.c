#include "lu_memory_manager.h"
#include <stdio.h>
#include <limits.h>
#include "lu_event.h"
#include <error.h>
#include <stdlib.h>


 
 
lu_event_config_t * lu_event_config_new(void)
{
    lu_event_config_t *ev_cfg_t = mm_calloc(1, sizeof(*ev_cfg_t));
     
	if (ev_cfg_t == NULL)
		return (NULL);

	TAILQ_INIT(&ev_cfg_t->entries);
	ev_cfg_t->max_dispatch_interval.tv_sec = -1;
	ev_cfg_t->max_dispatch_callbacks = INT_MAX;
	ev_cfg_t->limit_callbacks_after_priority = 1;
    ev_cfg_t->max_dispatch_interval.tv_usec     = -1;

	return (ev_cfg_t);
}

lu_event_base_t *lu_event_base_new_with_config(lu_event_config_t * ev_cfg_t_) {
  return NULL;
}
void lu_event_config_free(lu_event_config_t * ev_cfg_t) {
	//TODO
	return;
}
lu_event_base_t *lu_event_base_new(void) {
  lu_event_base_t *ev_base_t = NULL;
  lu_event_config_t *ev_cfg_t = lu_event_config_new();
  if (ev_cfg_t) {
    ev_base_t = lu_event_base_new_with_config(ev_cfg_t);
    lu_event_config_free(ev_cfg_t);
  }
  return (ev_base_t);
}


 
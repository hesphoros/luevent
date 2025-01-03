#ifndef LU_CHANGELIST_INTERNAL_H
#define LU_CHANGELIST_INTERNAL_H

/*
  A "changelist" is a list of all the fd status changes that should be made
  between calls to the backend's dispatch function.  There are a few reasons
  that a backend would want to queue changes like this rather than processing
  them immediately.

    1) Sometimes applications will add and delete the same event more than
       once between calls to dispatch.  Processing these changes immediately
       is needless, and potentially expensive (especially if we're on a system
       that makes one syscall per changed event).

    2) Sometimes we can coalesce multiple changes on the same fd into a single
       syscall if we know about them in advance.  For example, epoll can do an
       add and a delete at the same time, but only if we have found out about
       both of them before we tell epoll.

    3) Sometimes adding an event that we immediately delete can cause
       unintended consequences: in kqueue, this makes pending events get
       reported spuriously.
 */

#include "lu_util.h"



typedef struct lu_event_change_s {
    /**The fd or signal whose events are to be changed. */    
    lu_evutil_socket_t fd;
    /* The events that were enabled on the fd before any of these changes
	   were made.  May include EV_READ or EV_WRITE. */
	short old_events;
    /* The changes that we want to make in reading and writing on this fd.
	 * If this is a signal, then read_change has EV_CHANGE_SIGNAL set,
	 * and write_change is unused. */
    lu_uint8_t read_change;
    lu_uint8_t write_change;
    lu_uint8_t error_change;
}lu_event_change_t;




/* Flags for read_change and write_change. */

// /* If set, add the event. */
// #define EV_CHANGE_ADD     0x01
// /* If set, delete the event.  Exclusive with EV_CHANGE_ADD */
// #define EV_CHANGE_DEL     0x02
// /* If set, this event refers a signal, not an fd. */
// #define EV_CHANGE_SIGNAL  EV_SIGNAL
// /* Set for persistent events.  Currently not used. */
// #define EV_CHANGE_PERSIST EV_PERSIST
// /* Set for adding edge-triggered events. */
// #define EV_CHANGE_ET      EV_ET



/* The value of fdinfo_size that a backend should use if it is letting
 * changelist handle its add and delete functions. */
// #define EVENT_CHANGELIST_FDINFO_SIZE sizeof(int)


//TODO: to be implemented
// /** Set up the data fields in a changelist. */
// void lu_event_changelist_init_(lu_event_changelist_t *changelist);
// /** Remove every change in the changelist, and make corresponding changes
//  * in the event maps in the base.  This function is generally used right
//  * after making all the changes in the changelist. */
// void lu_event_changelist_remove_all_(lu_event_changelist_t *changelist, lu_event_base_t *base);
// /** Free all memory held in a changelist. */
// void lu_event_changelist_freemem_(lu_event_changelist_t *changelist);

// /** Implementation of eventop_add that queues the event in a changelist. */
// int lu_event_changelist_add_(lu_event_base_t *base, lu_evutil_socket_t fd, short old, short events,
//     void *p);
// /** Implementation of eventop_del that queues the event in a changelist. */
// int lu_event_changelist_del_(lu_event_base_t *base, lu_evutil_socket_t fd, short old, short events,
//     void *p);

#endif /* LU_CHANGELIST_INTERNAL_H */
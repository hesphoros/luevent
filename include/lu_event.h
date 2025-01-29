#ifndef LU_EVENT_H
#define LU_EVENT_H

#include "lu_event-internal.h"
#include "lu_visibility.h"

typedef void (*lu_event_callback_fn)(lu_evutil_socket_t, short, void *);

lu_event_base_t*    lu_event_base_new(void);
lu_event_config_t*  lu_event_config_new(void);
lu_event_base_t*    lu_event_base_new_with_config(lu_event_config_t* );
void                lu_event_config_free(lu_event_config_t*);
static void lu_event_debug_assert_not_added_(const lu_event_t *ev);
static void lu_event_debug_assert_socket_nonblocking(lu_evutil_socket_t fd);
int lu_event_assign(lu_event_t *ev, lu_event_base_t *base, lu_evutil_socket_t fd, short events, lu_event_callback_fn callback, void *callback_arg);
static void lu_event_debug_note_setup_(const lu_event_t*ev) { (void)ev; }



/**
 * @name event flags
 *
 * Flags to pass to lu_event_new(), lu_event_assign(), lu_event_pending(), and
 * anything else with an argument of the form "short events"
 */
/**@{*/
/** Indicates that a timeout has occurred.  It's not necessary to pass
 * this flag to event_for new()/event_assign() to get a timeout. */
#define LU_EV_TIMEOUT	0x01
/** Wait for a socket or FD to become readable */
#define LU_EV_READ	0x02
/** Wait for a socket or FD to become writeable */
#define LU_EV_WRITE	0x04
/** Wait for a POSIX signal to be raised*/
#define LU_EV_SIGNAL	0x08
/**
 * Persistent event: won't get removed automatically when activated.
 *
 * When a persistent event with a timeout becomes activated, its timeout
 * is reset to 0.
 */
#define LU_EV_PERSIST	0x10
/** Select edge-triggered behavior, if supported by the backend. */
#define LU_EV_ET		0x20
/**
 * If this option is provided, then lu_event_del() will not block in one thread
 * while waiting for the event callback to complete in another thread.
 *
 * To use this option safely, you may need to use lu_event_finalize() or
 * lu_event_free_finalize() in order to safely tear down an event in a
 * multithreaded application.  See those functions for more information.
 **/
#define LU_EV_FINALIZE     0x40
/**
 * Detects connection close events.  You can use this to detect when a
 * connection has been closed, without having to read all the pending data
 * from a connection.
 *
 * Not all backends support LU_EV_CLOSED.  To detect or require it, use the
 * feature flag LU_EV_FEATURE_EARLY_CLOSE.
 **/
#define LU_EV_CLOSED	0x80
/**@}*/


int event_debug_mode_on_ = 0;


#endif  //LU_EVENT_H
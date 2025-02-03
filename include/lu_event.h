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
static void lu_event_debug_note_del_(const lu_event_t *ev) { (void)ev; }
static void lu_event_debug_note_teardown_(const  lu_event_t *ev) { (void)ev; }
static void lu_event_debug_assert_is_setup_(const lu_event_t*ev) { (void)ev; }

static void lu_event_debug_note_add_(const lu_event_t *ev) { (void)ev; }
int lu_event_priority_set(lu_event_t *ev, int pri);
void lu_event_debug_unassign(lu_event_t *ev);
void lu_event_base_free(lu_event_base_t *base);
int lu_event_del(lu_event_t* ev);
int lu_event_del_noblock(lu_event_t  *ev);
void lu_event_active_nolock_(lu_event_t *ev, int res, short count);
int 
    lu_event_add_nolock_(lu_event_t *ev,const struct timeval *tv, int tv_is_absolute);
int 
    lu_event_base_free_queues_(lu_event_base_t *base,int run_finalizers);
static int
    lu_event_base_cancel_single_callback_(lu_event_base_t *base,lu_event_callback_t *evcb,int run_finalizers);
int
    lu_event_once(lu_evutil_socket_t fd, short events,void (*callback)(lu_evutil_socket_t, short, void *),void *arg, const struct timeval *tv);

static void
    lu_event_queue_remove_active(lu_event_base_t *base, lu_event_callback_t *evcb);
static void
    lu_event_queue_remove_active_later(lu_event_base_t *base, lu_event_callback_t *evcb);




int lu_event_callback_activate_nolock_(lu_event_base_t *, lu_event_callback_t *);
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


#define ev_signal_next	ev_.ev_signal.ev_signal_next
#define ev_io_next	ev_.ev_io.ev_io_next
#define ev_io_timeout	ev_.ev_io.ev_timeout


#define ev_io_timeout	ev_.ev_io.ev_timeout
#define ev_callback ev_evcallback.evcb_cb_union.evcb_callback
#define ev_arg ev_evcallback.evcb_arg
#define ev_flags ev_evcallback.evcb_flags

#define ev_ncalls	ev_.ev_signal.ev_ncalls
#define ev_pncalls	ev_.ev_signal.ev_pncalls
#define ev_closure ev_evcallback.evcb_closure

//priority
#define ev_pri ev_evcallback.evcb_pri

/** Argument for event_del_nolock_. Tells event_del not to block on the event
 * if it's running in another thread. */
#define LU_EVENT_DEL_NOBLOCK 0
/** Argument for event_del_nolock_. Tells event_del to block on the event
 * if it's running in another thread, regardless of its value for EV_FINALIZE
 */
#define LU_EVENT_DEL_BLOCK 1
/** Argument for event_del_nolock_. Tells event_del to block on the event
 * if it is running in another thread and it doesn't have EV_FINALIZE set.
 */
#define LU_EVENT_DEL_AUTOBLOCK 2
/** Argument for event_del_nolock_. Tells event_del to proceed even if the
 * event is set up for finalization rather for regular use.*/
#define LU_EVENT_DEL_EVEN_IF_FINALIZING 3


#endif  //LU_EVENT_H
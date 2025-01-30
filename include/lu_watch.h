#ifndef LU_WATCH_H
#define LU_WATCH_H


typedef struct lu_evwatch_s lu_evwatch_s;


/** Contextual information passed from event_base_loop to the "prepare" watcher
 * callbacks. We define this as a struct rather than individual parameters to
 * the callback function for the sake of future extensibility. */
struct lu_evwatch_prepare_cb_info {
	/** The timeout duration passed to the underlying implementation's `dispatch`.
	 * See evwatch_prepare_get_timeout. */
	const struct timeval *timeout;
};

/** Contextual information passed from event_base_loop to the "check" watcher
 * callbacks. We define this as a struct rather than individual parameters to
 * the callback function for the sake of future extensibility. */
struct lu_evwatch_check_cb_info {
	/** Placeholder, since empty struct is not allowed by some compilers. */
	void *unused;
};


/**
  Prepare callback, invoked by event_base_loop immediately before polling for
  I/O.

  @param watcher the prepare watcher that invoked this callback.
  @param info contextual information passed from event_base_loop.
  @param arg additional user-defined argument, set in `evwatch_prepare_new`.
 */
typedef void (*lu_evwatch_prepare_cb)(lu_evwatch_s *, const struct lu_evwatch_prepare_cb_info *, void *);

/**
  Check callback, invoked by event_base_loop immediately after polling for I/O
  and before processing any active events.

  @param watcher the check watcher that invoked this callback.
  @param info contextual information passed from event_base_loop.
  @param arg additional user-defined argument, set in `evwatch_check_new`.
 */
typedef void (*lu_evwatch_check_cb)( lu_evwatch_s*, const struct lu_evwatch_check_cb_info *, void *);



union lu_evwatch_cb_u
{
    lu_evwatch_prepare_cb prepare;
	lu_evwatch_check_cb check;
};





#endif /* LU_WATCH_H */
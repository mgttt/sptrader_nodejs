//20170507 (好像170509有更新)
//NOTES https://github.com/litesync/uv_callback/blob/master/uv_callback.h
#ifndef UV_CALLBACK_H
#define UV_CALLBACK_H
#ifdef __cplusplus
extern "C" {
#endif

#include <uv.h>

	/* Typedefs */
	typedef struct uv_callback_s   uv_callback_t;
	typedef struct uv_call_s       uv_call_t;

	/* Callback Functions */
	typedef void* (*uv_callback_func)(uv_callback_t* handle, void *data);

	/* Functions */
	int uv_callback_init(uv_loop_t* loop, uv_callback_t* callback, uv_callback_func function, int callback_type);
	int uv_callback_fire(uv_callback_t* callback, void *data, uv_callback_t* notify);
	int uv_callback_fire_sync(uv_callback_t* callback, void *data, void** presult, int timeout);

	/* Constants */
#define UV_DEFAULT      0
#define UV_COALESCE     1

	/* Structures */
	struct uv_callback_s {
		union {
			uv_async_t async;
			void *data;
		};
		int usequeue;
		uv_call_t *queue;          /* queue of calls to this callback */
		uv_mutex_t mutex;          /* mutex used to access the queue */
		uv_callback_func function; /* the function to be called */
		void *arg;                 /* data argument for coalescing calls (when not using queue) */
		uv_idle_t idle;            /* idle handle used to drain the queue if new async request was sent while an old one was being processed */
	};
	struct uv_call_s {
		uv_call_t *next;          /* pointer to the next call in the queue */
		void *data;               /* data argument for this call */
		uv_callback_t *notify;    /* callback to be fired with the result of this one */
	};

#ifdef __cplusplus
}
#endif
#endif  // UV_CALLBACK_H

//NOTES https://github.com/litesync/uv_callback/blob/master/uv_callback.c
//20170507
#include <stdlib.h>
//#include "uv_callback.h"

// what is not covered: destroy of uv_callback handle does not release all the resources


/*****************************************************************************/
/* RECEIVER / CALLED THREAD **************************************************/
/*****************************************************************************/

#ifndef container_of
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

void uv_callback_idle_cb(uv_idle_t* handle);

/* Dequeue *******************************************************************/

void * dequeue_call(uv_callback_t* callback) {
	uv_call_t *current, *prev = NULL;

	uv_mutex_lock(&callback->mutex);

	current = callback->queue;
	while (current && current->next != NULL) {
		prev = current;
		current = current->next;
	}

	if (prev)
		prev->next = NULL;
	else
		callback->queue = NULL;

	uv_mutex_unlock(&callback->mutex);

	return current;
}

/* Callback Function Call ****************************************************/

void uv_callback_async_cb(uv_async_t* handle) {
	uv_callback_t* callback = (uv_callback_t*) handle;

	if (callback->usequeue) {
		uv_call_t *call;
		while (call = (uv_call_t *) dequeue_call(callback)) {
			void *result = callback->function(callback, call->data);
			if (call->notify) uv_callback_fire(call->notify, result, NULL);
			free(call);
		}
		uv_idle_start(&callback->idle, uv_callback_idle_cb);
	} else {
		callback->function(callback, callback->arg);
	}

}

void uv_callback_idle_cb(uv_idle_t* handle) {
	uv_callback_t* callback = container_of(handle, uv_callback_t, idle);
	uv_callback_async_cb((uv_async_t*)callback);
	uv_idle_stop(handle);
}

/* Initialization ************************************************************/

int uv_callback_init(uv_loop_t* loop, uv_callback_t* callback, uv_callback_func function, int callback_type) {
	int rc;

	if (!loop || !callback || !function) return UV_EINVAL;

	callback->function = function;
	callback->queue = NULL;

	switch(callback_type) {
		case UV_DEFAULT:
			callback->usequeue = 1;
			uv_mutex_init(&callback->mutex);
			break;
		case UV_COALESCE:
			callback->usequeue = 0;
			break;
		default:
			return UV_EINVAL;
	}

	rc = uv_idle_init(loop, &callback->idle);
	if (rc) return rc;

	return uv_async_init(loop, (uv_async_t*) callback, uv_callback_async_cb);
}

/*****************************************************************************/
/* SENDER / CALLER THREAD ****************************************************/
/*****************************************************************************/

/* Asynchronous Callback Firing **********************************************/

int uv_callback_fire(uv_callback_t* callback, void *data, uv_callback_t* notify) {

	if (!callback) return UV_EINVAL;

	/* if there is a notification callback set, then the call must use a queue */
	if (notify!=NULL && callback->usequeue==0) return UV_EINVAL;

	if (callback->usequeue) {
		/* allocate a new call info */
		uv_call_t *call = (uv_call_t *) malloc(sizeof(uv_call_t));
		if (!call) return UV_ENOMEM;
		/* save the call info */
		call->data = data;
		call->notify = notify;
		/* add the call to the queue */
		uv_mutex_lock(&callback->mutex);
		call->next = callback->queue;
		callback->queue = call;
		uv_mutex_unlock(&callback->mutex);
	} else {
		callback->arg = data;
	}

	/* call uv_async_send */
	return uv_async_send((uv_async_t*)callback);
}

/* Synchronous Callback Firing ***********************************************/

struct call_result {
	int timed_out;
	int called;
	void *data;
};

void callback_on_walk(uv_handle_t *handle, void *arg) {
	uv_close(handle, NULL);
}

void * on_call_result(uv_callback_t *callback, void *data) {
	uv_loop_t *loop = ((uv_handle_t*)callback)->loop;
	struct call_result *result = (call_result *) loop->data;
	result->called = 1;
	result->data = data;
	uv_stop(loop);
}

void on_timer(uv_timer_t *timer) {
	uv_loop_t *loop = timer->loop;
	struct call_result *result = (call_result*) loop->data;
	result->timed_out = 1;
	uv_stop(loop);
}

int uv_callback_fire_sync(uv_callback_t* callback, void *data, void** presult, int timeout) {
	struct call_result result = {0};
	uv_loop_t loop;
	uv_callback_t notify;
	uv_timer_t timer;
	int rc=0;

	if (!callback || callback->usequeue==0) return UV_EINVAL;

	/* allocate a new call info */
	uv_call_t *call = (uv_call_t *) malloc(sizeof(uv_call_t));
	if (!call) return UV_ENOMEM;
	/* set the call result */
	uv_loop_init(&loop);
	uv_callback_init(&loop, &notify, on_call_result, UV_DEFAULT);
	loop.data = &result;
	/* save the call info */
	call->data = data;
	call->notify = &notify;
	/* add the call to the queue */
	uv_mutex_lock(&callback->mutex);
	call->next = callback->queue;
	callback->queue = call;
	uv_mutex_unlock(&callback->mutex);

	/* call uv_async_send to fire the callback on the other thread */
	uv_async_send((uv_async_t*)callback);

	/* if a timeout is supplied, set a timer */
	if (timeout > 0) {
		uv_timer_init(&loop, &timer);
		uv_timer_start(&timer, on_timer, timeout, 0);
	}

	/* run the event loop */
	uv_run(&loop, UV_RUN_DEFAULT);

	/* exited the event loop */
	uv_walk(&loop, callback_on_walk, NULL);
	uv_run(&loop, UV_RUN_DEFAULT);
	uv_loop_close(&loop);

	/* store the result */
	if (presult) *presult = result.data;
	if (rc==0 && result.timed_out) rc = UV_ETIMEDOUT;
	if (rc==0 && result.called==0) rc = UV_UNKNOWN;
	return rc;

}

/* $Id$ 
 * 
 * Licensed to the Apache Software Foundation (ASF) under one or more 
 * contributor license agreements. See the NOTICE file distributed with  
 * this work for additional information regarding copyright ownership. 
 * The ASF licenses this file to you under the Apache License, Version  
 * 2.0 (the "License"); you may not use this file except in compliance  
 * with the License. You may obtain a copy of the License at 
 * 
 * http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and 
 * limitations under the License. 
 */ 

/*
 * etch_simpletimer.c  
 * thread-per-use one-shot relative timer
 * this should be replaced with a timer pool implementation when development resources permit 
 */

#include "etch_simpletimer.h"
#include "etch_mutex.h"
#include "etch_exception.h"
#include "etch_log.h"
#include "etch_mem.h"

typedef struct etch_timerdata
{
    void* userdata;
    int   durationms;
    int   is_started;
    etch_timer_callback usercallback;

} etch_timerdata;

void   etch_timerproc(void* data);
etch_timerdata* new_timerdata(const int, void*, etch_timer_callback);



/**
 * new_timer() 
 * etch_timer constructor
 * @param durationms timer duration in milliseconds.
 * @param passthrudata user data to be returned in expiration callback.
 * caller retains ownership of this memory.
 * @param callback expiration callback void (*callback) (void*, int);
 * in which creator's passthrudata is passed as the first parameter, and the timeout
 * reason passed in parameter 2 (reason value 0 is signaled, 1 is timeout, -1 error).
 * @return the timer object. caller owns this object but must not destroy it until 
 * after the expiration callback has completed and returned.
 */
etch_timer* new_timer(const int durationms, void* passthrudata, etch_timer_callback callback)
{
    etch_thread* timer_thread = NULL;
    etch_timerdata* timerdata = NULL;
    if (NULL == callback || durationms <= 0) return NULL;
    timerdata = new_timerdata(durationms, passthrudata, callback);

    /* create thread in wait state */
    if (NULL == (timer_thread = new_thread(etch_timerproc, timerdata)))
        etch_free(timerdata);
    else  /* timerdata wrapper freed in etch_timerproc */
    {   timer_thread->params.is_own_data = FALSE;
          /* semikludge to complete thread start and thus avoid crash in the 
           * event timer is destroyed without having been started. 
           * surely there is a cleaner way to do this. */
        etch_sleep(1);
    }

    return timer_thread;
}


/**
 * etch_timer_start() 
 * start the specified timer. a timer must always be explicitly started.
 */
int etch_timer_start(etch_timer* timer_thread)
{
    int result = -1;
    etch_thread_params* params = timer_thread? &timer_thread->params: NULL;
    etch_timerdata* timerdata = params? (etch_timerdata*) params->data: NULL;

    if (timerdata)
        if (0 == (result = timer_thread->start(timer_thread)))
            timerdata->is_started = TRUE;

    return result; 
}

 
/**
 * etch_timer_stop()
 * signal specified timer to stop waiting and exit its thread. 
 */
int etch_timer_stop(etch_timer* timer_thread)
{
    int result = -1;
    etch_wait_t* waiter;
    if (NULL == timer_thread) return -1;
    waiter = timer_thread->params.waitobj;

    if (waiter && timer_thread->params.threadstate == ETCH_THREADSTATE_STARTED)
    {
        etch_wait_set(waiter, 1);
        etch_join(timer_thread); //wait until timer thread finished
    }

    return result;
}


/* - - - - - - - 
 * private   
 * - - - - - - - 
 */

/**
 * new_timerdata() 
 * create timer thread data
 */
struct etch_timerdata* new_timerdata(const int durationms, void* passthrudata, 
    etch_timer_callback callback)
{
    etch_timerdata* timerdata = etch_malloc(sizeof(etch_timerdata), 0);
    timerdata->userdata     = passthrudata;
    timerdata->durationms   = durationms;
    timerdata->usercallback = callback;
    timerdata->is_started   = FALSE;
    return timerdata;
}


/**
 * etch_timerproc() 
 * timer thread procedure
 */
void etch_timerproc(void* data)
{
    etch_status_t status = ETCH_SUCCESS;
    etch_thread_params* params = (etch_thread_params*) data;
    etch_timerdata* timerdata = (etch_timerdata*) params->data; 
    etch_timer_callback usercallback = timerdata->usercallback;
    etch_thread* threadx = params->etchobj;
    const int durationms = timerdata->durationms;
    void* userdata = timerdata->userdata;
    int   result = 0;
    ETCH_ASSERT(usercallback);

    if (NULL == threadx->params.waitobj) {
        // TODO: pool
        status = etch_wait_create(&threadx->params.waitobj, NULL);
        ETCH_ASSERT(status == ETCH_SUCCESS);
    }
    /* start timer and wait for expiration or signal */
    status = etch_wait_timedwait(threadx->params.waitobj, 1, durationms);
    switch(status) {
        // TODO refactor this stuff and remove ETCH_TIMEOUT ..
        case ETCH_SUCCESS:
            result = ETCH_INTERRUPTED;
            break;
        case ETCH_ETIMEOUT:
            result = ETCH_TIMEOUT;
            break;
        default:
            result = -1;
    }

	//ETCHOBJ_FREE();  /* etch_timerdata wrapper */ 
	if (params->data) 
		etch_free(params->data); 
	params->data = NULL;
    

    etch_wait_destroy(threadx->params.waitobj);
    threadx->params.waitobj = NULL;

    /* pass ownership of userdata back to user */
    usercallback(userdata, result);
}


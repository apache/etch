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
 * etch_queue_apr.c
 * based on apr_queue, modified by Cisco as follows: 
 * timeouts were added on the push and pop waits, and functions were modified
 * to release their lock at a single exit point.
 */

#include "apr.h"
#include <stdio.h>
#include <stdlib.h>

//#include "apu.h"
#include "apr_portable.h"
#include "apr_thread_mutex.h"
#include "apr_thread_cond.h"
#include "apr_errno.h"

#include "etch_queue_apr.h"  /* Cisco */

/* #define QUEUE_DEBUG */

#ifdef QUEUE_DEBUG
static void Q_DBG(char*msg, etch_apr_queue_t *q) {
    fprintf(stderr, "%ld\t#%d in %d out %d\t%s\n", 
                    (size_t) apr_os_thread_current(), /* cast - Cisco */
                    q->nelts, q->in, q->out,
                    msg
                    );
    fflush(stdout);  /* Cisco */
}
#else
#define Q_DBG(x,y) 
#endif


/**
 * Detects when the etch_apr_queue_t is full. This utility function is expected
 * to be called from within critical sections, and is not threadsafe.
 */
#define etch_apr_queue_full(queue) ((queue)->nelts == (queue)->bounds)


/**
 * Detects when the etch_apr_queue_t is empty. This utility function is expected
 * to be called from within critical sections, and is not threadsafe.
 */
#define etch_apr_queue_empty(queue) ((queue)->nelts == 0)


/**
 * Callback routine that is called to destroy this
 * etch_apr_queue_t when its pool is destroyed.
 */
static apr_status_t etch_apr_queue_destroy(void *data) 
{
    etch_apr_queue_t *queue = data;

    apr_thread_cond_destroy(queue->not_empty);
    apr_thread_cond_destroy(queue->not_full);
    apr_thread_mutex_destroy(queue->one_big_mutex);

    return APR_SUCCESS;
}


/**
 * Initialize the etch_apr_queue_t.
 */
apr_status_t etch_apr_queue_create(etch_apr_queue_t **q, 
                                   unsigned int queue_capacity, 
                                   apr_pool_t *a)
{
    apr_status_t rv;
    etch_apr_queue_t *queue;
    queue = apr_palloc(a, sizeof(etch_apr_queue_t));
    *q = queue;

    /* nested doesn't work ;( */
    rv = apr_thread_mutex_create(&queue->one_big_mutex,
                                 APR_THREAD_MUTEX_NESTED,
                                 a);
    if (rv != APR_SUCCESS)  
        return rv;

    rv = apr_thread_cond_create(&queue->not_empty, a);
    if (rv != APR_SUCCESS)  
        return rv;

    rv = apr_thread_cond_create(&queue->not_full, a);
    if (rv != APR_SUCCESS) 
        return rv;

    /* Set all the data in the queue to NULL */
    queue->data = apr_pcalloc(a, queue_capacity * sizeof(void*));
    queue->bounds = queue_capacity;
    queue->nelts = 0;
    queue->in = 0;
    queue->out = 0;
    queue->terminated = 0;
    queue->full_waiters = 0;
    queue->empty_waiters = 0;

    apr_pool_cleanup_register(a, queue, etch_apr_queue_destroy, apr_pool_cleanup_null);

    return APR_SUCCESS;
}


/**
 * Push new data onto the queue. Blocks if the queue is full. Once
 * the push operation has completed, it signals other threads waiting
 * in apr_queue_pop() that they may continue consuming sockets.
 * @param timeout added by Cisco. now uses apr_thread_cond_timewait(). 
 * interval of time to wait. zero means forever, negative indicates no wait, 
 * otherwise wait time in *microseconds*.
 * @return APR_SUCCESS, APR_EAGAIN, APR_EOF, APR_EINTR, APR_TIMEUP, 
 * or some APR error
 */
apr_status_t etch_apr_queue_push(etch_apr_queue_t *queue, 
                                 apr_interval_time_t timeout, 
                                 void *data)
{
    apr_status_t rv;

    if (queue->terminated)  
        rv = APR_EOF; /* no more elements ever again */
    else
    if (APR_SUCCESS == (rv = apr_thread_mutex_lock(queue->one_big_mutex)))
    {
        do 
        {   if (etch_apr_queue_full(queue)) 
            {
                if (!queue->terminated) 
                {
                    if (-1 == timeout) 
                    {   rv = APR_EAGAIN; /* asked to not wait */
                        break;
                    }

                    queue->full_waiters++;

                    if (0 == timeout)
                        rv = apr_thread_cond_wait(queue->not_full, queue->one_big_mutex);
                    else
                        rv = apr_thread_cond_timedwait(queue->not_full, queue->one_big_mutex, timeout);

                    queue->full_waiters--;
                    if (rv != APR_SUCCESS)  
                        break;
                }

                /* If we wake up and it's still empty, then we were interrupted */
                if (etch_apr_queue_full(queue)) 
                {
                    Q_DBG("queue full (intr)", queue);
                    rv = queue->terminated? APR_EOF: APR_EINTR; 
                    break;
                }
            }

            queue->data[queue->in] = data;
            queue->in = (queue->in + 1) % queue->bounds;
            queue->nelts++;

            if (queue->empty_waiters) 
            {
                Q_DBG("sig !empty", queue);
                rv = apr_thread_cond_signal(queue->not_empty);
            }

        } while(0);

        apr_thread_mutex_unlock(queue->one_big_mutex);
    }

    return rv;
}


/**
 * Push new data onto the queue. Blocks if the queue is full. Once
 * the push operation has completed, it signals other threads waiting
 * in apr_queue_pop() that they may continue consuming sockets.
 */
apr_status_t etch_apr_queue_trypush(etch_apr_queue_t *queue, void *data)
{
    apr_status_t rv;

    if (queue->terminated)  
        rv = APR_EOF;  
    else
    if (APR_SUCCESS == (rv = apr_thread_mutex_lock(queue->one_big_mutex)))
    {
        if (etch_apr_queue_full(queue))
            rv = APR_EAGAIN;  
        else
        {   queue->data[queue->in] = data;
            queue->in = (queue->in + 1) % queue->bounds;
            queue->nelts++;

            if (queue->empty_waiters) 
            {
                Q_DBG("sig !empty", queue);
                rv  = apr_thread_cond_signal(queue->not_empty);               
            }
       }

       apr_thread_mutex_unlock(queue->one_big_mutex);
    }

    return rv;
}


/**
 * not thread safe
 */
unsigned int etch_apr_queue_size(etch_apr_queue_t *queue) {
    return queue->nelts;
}


/**
 * Retrieves the next item from the queue. If there are no
 * items available, it will block until one becomes available.
 * Once retrieved, the item is placed into the address specified by
 * 'data'.
 * @param timeout added by Cisco. now uses apr_thread_cond_timewait(). 
 * interval of time to wait. zero means forever, -1 means no wait, 
 * -2 means don't wait and ignore queue closed indicator,
 * otherwise timeout is blocking time in microseconds.
 * @return APR_SUCCESS, APR_EAGAIN, APR_EOF, APR_EINTR, APR_TIMEUP, 
 * or some APR error
 */
apr_status_t etch_apr_queue_pop(etch_apr_queue_t *queue, 
                                apr_interval_time_t timeout, 
                                void **data)
{
    apr_status_t rv;

    if (queue->terminated)  /* Cisco back door to clear closed queue */
    {   if (timeout != ETCHQUEUE_CLEARING_CLOSED_QUEUE)  
            return APR_EOF; /* no more elements ever again */
    }

    if (APR_SUCCESS == (rv = apr_thread_mutex_lock(queue->one_big_mutex)))
    {
        do 
        {   /* Keep waiting until we wake up and find that the queue is not empty. */
            if (etch_apr_queue_empty(queue)) 
            {
                if (-1 == timeout) 
                {   rv = APR_EAGAIN; /* asked to not wait */
                    break;
                }

                if (!queue->terminated) 
                {
                    queue->empty_waiters++;

                    if (0 == timeout)
                        rv = apr_thread_cond_wait(queue->not_empty, queue->one_big_mutex);
                    else 
                        rv = apr_thread_cond_timedwait(queue->not_empty, queue->one_big_mutex, timeout);

                    queue->empty_waiters--;

                    if (rv != APR_SUCCESS) /* rv will be APR_TIMEUP if timed out */ 
                        break;
                }

                /* If we wake up and it's still empty, then we were interrupted */
                if (etch_apr_queue_empty(queue)) 
                {
                    Q_DBG("queue empty (intr)", queue);
                    rv = queue->terminated? APR_EOF: APR_EINTR;
                    break;
                }
            } 

            *data = queue->data[queue->out];
            queue->nelts--;

            queue->out = (queue->out + 1) % queue->bounds;

            if (queue->full_waiters) 
            {
                Q_DBG("signal !full", queue);
                rv = apr_thread_cond_signal(queue->not_full);
            }

        } while(0);

        apr_thread_mutex_unlock(queue->one_big_mutex);
    }  

    return rv;
}


/**
 * Retrieves the next item from the queue. If there are no
 * items available, return APR_EAGAIN.  Once retrieved,
 * the item is placed into the address specified by 'data'.
 */
apr_status_t etch_apr_queue_trypop(etch_apr_queue_t *queue, void **data)
{
    apr_status_t rv;

    if (queue->terminated)  
        rv = APR_EOF; /* no more elements ever again */
    else
    if (APR_SUCCESS == (rv = apr_thread_mutex_lock(queue->one_big_mutex)))
    {
        if (etch_apr_queue_empty(queue)) 
            rv = APR_EAGAIN;
        else
        {   *data = queue->data[queue->out];
            queue->nelts--;

            queue->out = (queue->out + 1) % queue->bounds;

            if (queue->full_waiters) 
            {
                Q_DBG("signal !full", queue);
                rv = apr_thread_cond_signal(queue->not_full);
            }
       }

       apr_thread_mutex_unlock(queue->one_big_mutex);
    }

    return rv;
}


apr_status_t etch_apr_queue_interrupt_all(etch_apr_queue_t *queue)
{
    apr_status_t rv;
    Q_DBG("intr all", queue); 
   
    if (APR_SUCCESS == (rv = apr_thread_mutex_lock(queue->one_big_mutex)))
    { 
        apr_thread_cond_broadcast(queue->not_empty);
        apr_thread_cond_broadcast(queue->not_full);

        apr_thread_mutex_unlock(queue->one_big_mutex);
    }

    return rv;
}


/**
 * etch_apr_queue_unsafe_interrupt_all()
 * added by Cisco to opeate when lock already held since queue lock is not nestable.
 */
apr_status_t etch_apr_queue_unsafe_interrupt_all(etch_apr_queue_t *queue)
{
    Q_DBG("intr all", queue);    
 
    apr_thread_cond_broadcast(queue->not_empty);
    apr_thread_cond_broadcast(queue->not_full);

    return APR_SUCCESS;
}


apr_status_t etch_apr_queue_term(etch_apr_queue_t *queue)
{
    apr_status_t rv;
  
    if (APR_SUCCESS == (rv = apr_thread_mutex_lock(queue->one_big_mutex)))
    {
        /* we must hold one_big_mutex when setting this... otherwise,
         * we could end up setting it and waking everybody up just after a 
         * would-be popper checks it but right before they block
         */
        queue->terminated = 1;

        apr_thread_mutex_unlock(queue->one_big_mutex);
    }

    return etch_apr_queue_interrupt_all(queue);
}


/**
 * added by Cisco to close queue when lock already held
 */
apr_status_t etch_apr_queue_unsafeclose(etch_apr_queue_t *queue)
{
    queue->terminated = 1;
    return etch_apr_queue_unsafe_interrupt_all(queue);
}


/**
 * added by Cisco to access lock externally
 */
apr_status_t etch_apr_queue_lock(etch_apr_queue_t *queue)
{
    return apr_thread_mutex_lock(queue->one_big_mutex);
}


/**
 * added by Cisco to access lock externally
 */
apr_status_t etch_apr_queue_unlock(etch_apr_queue_t *queue)
{
    return apr_thread_mutex_unlock(queue->one_big_mutex);
}


/**
 * etch_apr_queue_trylock()
 * added by Cisco to access lock externally
 */
int etch_apr_queue_trylock(etch_apr_queue_t *queue) 
{
    return apr_thread_mutex_trylock(queue->one_big_mutex);
}

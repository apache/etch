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

/**
 * etch_queue.c  
 */

#include "etch_queue.h"
#include "etch_thread.h"
#include "etch_log.h"
#include "etch_mem.h"
#include "etch_runtime.h"
#include "etch_objecttypes.h"

/*
static const char* LOG_CATEGORY = "etch_queue";
*/

extern apr_pool_t*         g_etch_main_pool;

int  destroy_etch_queue(void*);
void etchqueue_clear(etch_queue*); 
int  etchqueue_iterable_first(etch_iterator*);
int  etchqueue_iterable_next(etch_iterator*);
int  etchqueue_iterable_has_next(etch_iterator*);



/**
 * new_queue()
 * etch_queue constructor
 */
etch_queue* new_queue(const int initialsize)
{
    etch_queue* newq = NULL;
    etch_apr_queue_t* aprq = NULL;
    apr_pool_t* newsubpool = NULL;
    int result = -1, qsize = initialsize > 0? initialsize: ETCH_DEFQSIZE;

    do
    {

        // TODO: pool
        result = apr_pool_create(&newsubpool, g_etch_main_pool);
        //printf("2 creating apr pool %p\n",newsubpool);
        if (0 != result) break; 
        /* set pool abort function */
        apr_pool_abort_set(etch_runtime_mem_abort, newsubpool);
        

        result = etch_apr_queue_create (&aprq, qsize, newsubpool);
        if (0 != result) break;

        newq = (etch_queue*) new_object(sizeof(etch_queue), ETCHTYPEB_ETCHQUEUE, CLASSID_ETCHQUEUE);

        newq->aprq = aprq;
        newq->subpool = newsubpool;
        newq->qcapacity = initialsize;
        ((etch_object*)newq)->destroy = destroy_etch_queue;
        /* assume content to be etch objects owned by the queue */
        newq->is_readonly  = FALSE; 
        newq->content_type = ETCHQUEUE_CONTENT_OBJECT;

        new_iterable(&newq->iterable, NULL, etchqueue_iterable_first, 
            etchqueue_iterable_next, etchqueue_iterable_has_next); 

        result = 0;

    } while(0);

    if (0 != result)
    {
        if (newsubpool) {
            //printf("3 destroying apr pool %p\n",newsubpool);
            apr_pool_destroy(newsubpool);
        }
    }

    return newq;
}


/*
 * destroy_etch_queue()
 * etch_queue destructor
 */
int destroy_etch_queue(void* data)
{
    etch_queue* queue = (etch_queue*)data;
    if (NULL == queue) return -1; 

    if  (-1 == etch_apr_queue_trylock(queue->aprq))
         return -1; 
    else etch_apr_queue_unlock(queue->aprq); 
               
    if (!is_etchobj_static_content(queue))
    {
        /* free etch-managed memory */
        etchqueue_clear(queue);

        /* free APR-managed memory - APR queue sets a callback such that 
         * destroying the memory pool destroys the APR queue object */
        if (queue->subpool)  {
            //printf("4 destroying apr pool %p\n",queue->subpool);
            apr_pool_destroy(queue->subpool);
        }

        queue->subpool = NULL;
        queue->aprq = NULL;
    }

    return destroy_objectex((etch_object*)queue);
}


/*
 * etchqueue_clear() 
 * frees queue content memory. 
 * this ensures the underlying queue is closed, so it is only invoked prior to
 * destruction of the queue in order to free any remaining queued etch objects.
 * all other memory associated with the queue, other than the shell object, 
 * is managed by APR. 
 */
void etchqueue_clear(etch_queue* thisx) 
{
    if (NULL == thisx || NULL == thisx->aprq || thisx->is_readonly) return;
    etchqueue_close(thisx, TRUE); /* close queue and notify all waiters */

    do
    {   void* qobj = NULL;
        queuecallback callback = thisx->freehook;
        int i = 0, is_freehandled = 0, result = 0;
        const int is_obj_content = thisx->content_type == ETCHQUEUE_CONTENT_OBJECT;

        while(1)
        {
            result = etchqueue_get_withwait (thisx, ETCHQUEUE_CLEARING_CLOSED_QUEUE, &qobj);
            if (0 != result) break;

            is_freehandled = callback? callback(i, qobj): FALSE; 

            if  (is_freehandled);
            else
            if  (is_obj_content)
                ((etch_object*)qobj)->destroy(qobj);            
            else etch_free(qobj);
        }  

    } while(0);
}

            
/**
 * etchqueue_size()
 * @return current queue depth
 * not thread-safe, caller should hold the mailbox queue lock
 */
int etchqueue_size(etch_queue* thisx) 
{
    return thisx && thisx->aprq? etch_apr_queue_size(thisx->aprq): 0;
}


/**
 * etchqueue_is_closed()
 * not thread-safe, caller should hold the mailbox queue lock
 */
int etchqueue_is_closed(etch_queue* thisx)
{
    return thisx && thisx->aprq? thisx->aprq->terminated != 0: TRUE; 
}


/**
 * etchqueue_is_full()
 * not thread-safe, caller should hold the mailbox queue lock
 */
int etchqueue_is_full(etch_queue* thisx)
{
    return thisx && thisx->aprq? thisx->aprq->nelts == thisx->aprq->bounds: TRUE; 
}


/**
 * etchqueue_close()
 * underlying implementation ensures operation atomicity
 */
int etchqueue_close(etch_queue* thisx, const int is_needlock)
{
    int result = 0;

    if (!etchqueue_is_closed(thisx)) 
    {
        if (is_needlock)
            result = etch_apr_queue_term(thisx->aprq);
        else
            result = etch_apr_queue_unsafeclose(thisx->aprq);

        result = result == 0? 0: -1;
    }
    return result;
}


/**
 * etchqueue_put()
 * push specified item onto the queue. if the queue is full, 
 * wait indefinitely until space becomes available.
 * @return 0 success, -1 error.
 */
int etchqueue_put(etch_queue* thisx, void* item)
{
    const int result = etchqueue_put_withwait(thisx, ETCH_INFWAIT, item);
    return result;
}


/**
 * etchqueue_put_withwait()
 * push specified item onto the queue. if the queue is full, 
 * waitms specified time interval until space becomes available.
 * @param waitms wait interval in milliseconds. specify ETCH_INFWAIT to wait
 * forever, ETCH_NOWAIT to not wait.
 * @return 0 success, -1 unsuccessful.
 * 
 */
int etchqueue_put_withwait(etch_queue* thisx, const int waitms, void* item)
{
    int result = 0, aprresult = 0;
    const int64 wait_usec = waitms < 0? -1: waitms * 1000;
    if (NULL == item || etchqueue_is_closed(thisx)) return -1;

    /*
     * etch_apr_queue_push() takes care of notifying threads waiting on a
     * queue slot to become available, if any.
     */
    aprresult = etch_apr_queue_push (thisx->aprq, wait_usec, item);

    switch(aprresult)
    {   case APR_SUCCESS: break;

        case APR_EAGAIN: /* timed out and queue still full */
        case APR_TIMEUP:
             result = ETCH_QUEUE_OPERATION_TIMEOUT;
             break;

        case APR_EINTR:  /* wait interrupted and queue still full */
             result = ETCH_QUEUE_OPERATION_CANCELED;
             break;

        case APR_EOF:    /* queue closed */
             default:    /* some error */
             result = -1;
    }

    return result;
}


/**
 * etchqueue_get()
 * pop specified item off the queue and return it. if the queue is empty, 
 * wait indefinitely for an item to arrive. popped item is returned in
 * out parameter.
 * @return 0 success, -1 error.
 */
int etchqueue_get(etch_queue* thisx, void** itemout)
{
    const int result = etchqueue_get_withwait(thisx, ETCH_INFWAIT, itemout);
    return result;
}


/**
 * etchqueue_get_withwait()
 * pop specified item off the queue and return it. if the queue is empty, 
 * wait specified time interval for an item to arrive. popped item is
 * returned in out parameter.
 * @param waitms wait interval in milliseconds. specify ETCH_INFWAIT to wait
 * forever, ETCH_NOWAIT to not wait, ETCHQUEUE_CLEARING_CLOSED_QUEUE to do a
 * get with no wait regardless of whether queue has been closed.
 * @return 0 success, -1 unsuccessful.
 * 
 */
int etchqueue_get_withwait(etch_queue* thisx, const int waitms, void** itemout)
{
    int result = 0, aprresult = 0;
    const int64 wait_usec = waitms < 0? waitms: waitms * 1000;

    if (etchqueue_is_closed(thisx))
        if (waitms != ETCHQUEUE_CLEARING_CLOSED_QUEUE)
            return -1;

    /*
     * etch_apr_queue_pop() takes care of notifying waiters if necessary
     */
    aprresult = etch_apr_queue_pop (thisx->aprq, wait_usec, itemout);

    switch(aprresult)
    {   case APR_SUCCESS: break;

        case APR_EAGAIN: /* timed out and queue still full */
        case APR_TIMEUP:
             result = ETCH_QUEUE_OPERATION_TIMEOUT; break;

        case APR_EINTR:  /* wait interrupted and queue still empty */
             result = ETCH_QUEUE_OPERATION_CANCELED; break;

        case APR_EOF:    /* queue empty */
             result = ETCH_QUEUE_EOF; break;

        default:         /* some error */
             result = -1;
    }

    return result;
}


/**
 * etchqueue_try_put()
 * @return 0 success, -1 unsuccessful.
 * 
 */
int etchqueue_try_put(etch_queue* thisx, void* item)
{
    const int result = etch_apr_queue_trypush(thisx->aprq, item);
    return result == 0? 0: -1;
}


/**
 * etchqueue_try_get()
 * @return 0 success, -1 unsuccessful.
 */
int etchqueue_try_get(etch_queue* thisx, void** itemout)
{
    const int result = etch_apr_queue_trypop(thisx->aprq, itemout);
    return result == 0? 0: -1;
}


 /**
 * etchqueue_notify_all()
 * wake up all waiters, 
 * @return 0 success, -1 unsuccessful.
 */
int etchqueue_notify_all(etch_queue* thisx)
{
    const int aprresult = etch_apr_queue_interrupt_all(thisx->aprq);
    return aprresult == 0? 0: -1;
}

 
/**
 * acquire the lock guarding the queue
 */
int etchqueue_lock(etch_queue* thisx) 
{
    return 0 == etch_apr_queue_lock(thisx->aprq)? 0: -1;
}


/**
 * release the lock guarding the queue
 */
int etchqueue_unlock(etch_queue* thisx)
{
    return 0 == etch_apr_queue_unlock(thisx->aprq)? 0: -1;
} 


/**
 * acquire the lock guarding the queue if currently availbale
 */
int etchqueue_trylock(etch_queue* thisx)
{
    return 0 == etch_apr_queue_trylock(thisx->aprq)? 0: -1;
}


/* - - - - - - - - - - 
 * i_iterable
 * - - - - - - - - - - 
 */

/*
 * etchqueue_iterable_first() 
 * i_iterable first() implementation
 */
int etchqueue_iterable_first(etch_iterator* iter)
{
    etch_queue* etchq = NULL;
    if (!iter || !iter->collection)  return -1;
    etchq = iter->collection;
    if (!etchqueue_size(etchq)) return -1;

    iter->current_value = etchq->aprq->data[0];   
    iter->ordinal = iter->current_value? 1: 0;
    return iter->ordinal? 0: -1;
}


/*
 * etchqueue_iterable_next() 
 * i_iterable next() implementation
 * functions as first() if there is no current position.
 */
int etchqueue_iterable_next(etch_iterator* iter)
{
    etch_queue* etchq = iter? iter->collection: NULL;
    const int count = etchqueue_size(etchq);
    if (!count || !iter->ordinal) return -1;
    
    iter->current_value = etchq->aprq->data[iter->ordinal]; 
    iter->ordinal = iter->current_value? ++iter->ordinal: 0;
    return iter->ordinal? 0: -1;
}


/*
 * etchqueue_iterable_has_next() 
 * i_iterable has_next() implementation.
 */
int etchqueue_iterable_has_next(etch_iterator* iter)
{
    etch_queue* etchq = iter? iter->collection: NULL;
    const int count = etchqueue_size(etchq);
    return count && iter->ordinal && (iter->ordinal <= count);
}

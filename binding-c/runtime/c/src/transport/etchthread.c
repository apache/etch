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
 * etchthread.c
 * threads, thread pool
 */

#include "apr_network_io.h"
#include "etch_global.h"
#include "etchthread.h"
#include "etchlog.h"

size_t etch_get_threadid(void* threadstruct);
int default_on_threadstart(void* param);
int default_on_threadexit (void* param);
int destroy_thread   (etch_thread*);
int etch_thread_start(etch_thread*);
int etch_thread_stop (etch_thread*);
int destroy_threadparams(etch_thread*);
etchmutex* loglock;
etchmutex  loglockx;
char* ETCHTHRE = "THRE";

#ifdef APR_POOL_DEBUG
APR_DECLARE(apr_size_t) apr_pool_num_bytes(apr_pool_t *p, int recurse);   
#endif


/**
 * etch_apr_threadproc()
 * internal thread proc. this is a wrapper around the user thread proc.
 * @param data expected to point at an etch_threadparams, which contains the
 * thread start amd stop handlers, and the real threadproc and threadproc data.
 */
static void* APR_THREAD_FUNC etch_apr_threadproc(apr_thread_t *thread, void *data)
{    
    etch_threadparams* params = (etch_threadparams*) data;
    etch_thread_callback on_exit;   
    etch_thread* threadobj;
    if(!params) return (void*)(-1);
    on_exit   = params->on_exit;
    threadobj = params->etchobj;

    if (threadobj && threadobj->waiter)  /* wait for signal to start */
    {
        threadobj->waiter->wait (threadobj->waiter);

        /* fyi we can't destroy the waiter here since etchwait_signal  
         * holds etchwait.mutex and will try to release it.  
         * destroy_etchwait (threadobj->waiter);  no!  
         * threadobj->waiter = NULL;  
         */              
    }

    params->threadstate = ETCH_THREADSTATE_STARTED;

    if (params->on_start)    /* call threadproc start hook */
        params->on_start(threadobj);

    if (params->threadproc)  /* run user threadproc */
        params->threadproc(params);  

    params->threadstate = ETCH_THREADSTATE_STOPPED;
	    
    if (on_exit)             /* call threadproc exit hook */
        on_exit(threadobj);    

    return 0;
}


/**
 * new_thread()
 * etch_thread constructor
 */
etch_thread* new_thread (etch_threadproc proc, void* tdata)
{
    etch_threadparams* tp  = NULL;
    apr_pool_t* newsubpool = NULL;
    etch_thread* newthread = NULL;
    /* size_t poolsize = apr_pool_num_bytes(get_etch_aprpool(), FALSE); */  

    if (-1 == apr_pool_create(&newsubpool, get_etch_aprpool())) 
        return NULL;

    newthread = (etch_thread*) new_object
        (sizeof(etch_thread), ETCHTYPEB_THREAD, CLASSID_THREAD);

    newthread->destroy = destroy_thread;
    newthread->clone   = clone_null;
 
    /* allocating an apr memory pool per thread is not ideal, however it was
     * not immediately clear how to free apr memory for an apr_thread_t and
     * for the apr_threadattr_t, apr documentation being nonexistent in that 
     * respect. once we find out, we should change this code to not use a
     * subpool, and change the destructor to free those two data structures.
     */
    newthread->aprdata.mempool = newsubpool;  
    newthread->aprdata.is_dedicated_mempool = TRUE; 

    tp = &newthread->params;
    etch_init_threadparams(tp);
    tp->etchobj  = newthread;
    tp->libdata  = &newthread->aprdata;
    tp->on_start = default_on_threadstart;
    tp->on_exit  = default_on_threadexit;
    tp->etch_thread_id = next_etch_threadid();
    tp->threadproc = proc;
    tp->data = tdata;

    newthread->start = etch_thread_start;
    newthread->stop  = etch_thread_stop; 

    if (0 == etch_createthread(tp)) 
        newthread->waiter = new_wait(newthread->aprdata.mempool);
    else
    {   etch_free(newthread);
        newthread = NULL;
        apr_pool_destroy(newsubpool);
    } 

    return newthread;
} 


/**
 * etch_createthread()
 * to invoke: 1) initalize an etch_apr_threaddata with the mempool; 
 * 2) initialize an etch_threadparams with threadproc plus the etch_apr_threaddata,
 *    plus the thread user data.
 * 2) call with the threaddata, and the userdata.
 */
int etch_createthread(etch_threadparams* params)
{    
    int result = 0;
    apr_pool_t* mempool = params->libdata->mempool;
    if (!params->on_start) params->on_start = default_on_threadstart;
    if (!params->on_exit)  params->on_exit  = default_on_threadexit;

    /* fyi: this sets thread.aprdata.threadattr */
    result = (APR_SUCCESS == apr_threadattr_create
        (&params->libdata->threadattr, mempool))? 0: -1;

    if (result == 0)  /* fyi: this sets thread.aprdata.thread */
        result = (APR_SUCCESS == apr_thread_create
           (&params->libdata->thread, params->libdata->threadattr, 
            etch_apr_threadproc, params, mempool))? 0: -1;  

    return result; 
}


/**
 * destroy_thread()
 * etch_thread destructor
 */
int destroy_thread(etch_thread* threadx)
{
    if (threadx->refcount > 0 && --threadx->refcount > 0) return -1; 

    if (!is_etchobj_static_content(threadx))
    {
        if (threadx->waiter)
            destroy_etchwait(threadx->waiter);

        if (threadx->aprdata.is_dedicated_mempool)
            apr_pool_destroy(threadx->aprdata.mempool);
    }

    return destroy_objectex((objmask*)threadx);   
}


/**
 * destroy_threadparams()
 * free memory for any heap parameters passed to an etch_thread.
 */
int destroy_threadparams (etch_thread* thisx)
{
    etch_threadparams* params = &thisx->params;
    void* userdata = params->data;

    if (params->waitobj) 
        params->waitobj->destroy(params->waitobj);          

    if (params->is_own_data)
        if (params->is_data_etchobject)
           ((objmask*)userdata)->destroy(userdata);
        else etch_free(userdata);

    return 0;
}


/**
 * etch_init_threadparams()
 */
void etch_init_threadparams (etch_threadparams* p)
{
    if (NULL == p) return;
    memset(p, 0, sizeof(etch_threadparams));
    p->signature = ETCH_THREADPARAMS_SIGNATURE;
}


/**
 * etch_thread_start()
 * default thread start method for etch_threads which wait for a start signal
 */
int etch_thread_start (etch_thread* threadx)
{
    int result = -1, threadstate = 0;
    const int thread_id = threadx->params.etch_thread_id;
    etchlog(ETCHTHRE, ETCHLOG_DEBUG, "thread %d starting ...\n", thread_id); 

    if (threadx->waiter)
        result = threadx->waiter->signal(threadx->waiter);

    /* etch_tcpsvr_acceptproc: while(lxr->is_started) was not seeing the started 
     * flag set, because the thread had not received control after the above signal.
     * so this sleep forces it to run, the visual indication of the accept thread 
     * running being the "accepting ..." log message. TODO lose the sleep().
     * NOTE that by the time this thread regains control, its connection may
     * have been closed, or the started thread may possibly even have exited. 
     */
    etch_sleep(30);  /* see comments above */

    /* there is currently no pressing need to log this info, 
     * and since it may indeed be stale, we'll not present it for now. */
    #if(0) 
    threadstate = is_etch_thread(threadx)? 
        threadx->params.threadstate: ETCH_THREADSTATE_DEFUNCT;

    switch(threadstate)
    {   case ETCH_THREADSTATE_STARTED:
             etchlog(ETCHTHRE, ETCHLOG_DEBUG, "thread %d started\n", thread_id); 
             break;
        case ETCH_THREADSTATE_STOPPED:
             etchlog(ETCHTHRE, ETCHLOG_DEBUG, "thread %d stopped\n", thread_id); 
             break;
        case ETCH_THREADSTATE_DEFUNCT:
             etchlog(ETCHTHRE, ETCHLOG_DEBUG, "thread %d was started now destroyed\n", thread_id); 
             break;
        default: etchlog(ETCHTHRE, ETCHLOG_DEBUG, "thread %d state %d\n", thread_id, threadstate);
    }
    #endif

    return result;
}


/**
 * etch_thread_stop()
 * default etch_thread thread stop method.
 * currently never invoked. causes serious grief.
 */
int etch_thread_stop (etch_thread* thread)
{
    apr_status_t tresult = 0;   
    etch_threadparams* p = thread? &thread->params: NULL;
    if (NULL == p) return -1;

    tresult = apr_thread_detach(p->libdata->thread);

    tresult = apr_thread_exit(p->libdata->thread, 0);

    return APR_SUCCESS == tresult? 0: -1;
}


/**
 * etch_threadpool_run_freethread()
 * instantiate and possibly run a free thread.
 */
etch_thread* etch_threadpool_run_freethread (etch_threadpool* pool, 
    etch_threadproc threadproc, void* threaddata)
{
    etch_thread* newthread = NULL;
    if (!pool || pool->is_defunct) return NULL;

    /* create thread in a wait state. it may be started below. */
    if (NULL == (newthread = new_thread (threadproc, threaddata)))
        return NULL;  

    newthread->params.etchpool = pool;
    newthread->params.is_own_data = pool->is_free_data;
    newthread->params.is_data_etchobject = pool->is_data_etchobject;

    arraylist_add(pool->threadlist, newthread);  
    /* an etch_thread object in the threadpool gets freed when a thread exits, 
     * finds its threadpool, and removes itself from that pool's threadlist. 
     */
    etchlog(ETCHTHRE, ETCHLOG_DEBUG, "thread %d created on pool %d\n", 
        newthread->params.etch_thread_id, pool->threadpool_id); 

    if (!pool->is_manual_start) /* start thread unless requested otherwise */
        newthread->start(newthread);  

    return newthread;
}


/**
 * etch_threadpool_run_poolthread()
 * run a thread from a queued pool.
 */
etch_thread* etch_threadpool_run_poolthread (etch_threadpool* pool, 
    etch_threadproc threadproc, void* threaddata)
{
    if (!pool || pool->is_defunct) return NULL;
    return NULL; /* we have not yet implemented or integrated a queued thread pool */
}


/**
 * thread_cancel()
 * cancel a running thread
 */
int etch_thread_cancel(etch_thread* thread)
{
    thread->params.threadstate = ETCH_THREADSTATE_STOPPING;
    etch_join(thread); 
    return 0;       
}


/**
 * etch_join()
 * block until specified thread exits
 */
int etch_join(etch_thread* thread)
{
   int  result = 0;
   etch_threadparams* tp = &thread->params;
   apr_status_t tresult = 0;   /* result returned from dead thread */
   thread->is_joined = TRUE;  /* mark to avoid on_exit destruction */
   result = apr_thread_join (&tresult, tp->libdata->thread); 
   return result;
}


/**
 * etch_thread_join()
 * block until specified thread exits
 */
int etch_thread_join(etch_threadparams* params)
{
   apr_status_t tresult = 0;  /* result returned from dead thread */
   const int result = apr_thread_join(&tresult, params->libdata->thread); 
   return result;
}


/**
 * etch_thread_yield()
 */
void etch_thread_yield()
{
   apr_thread_yield();
}


/**
 * etch_thread_arraylist_comparator()
 * arraylist comparator function to compare a specified thread ID 
 * with the ID of a specified etch_thread
 */
int etch_thread_arraylist_comparator (void* id, void* etchobj)
{
    const int this_id = (int) (size_t) id;
    const int that_id = ((etch_thread*) etchobj)->params.etch_thread_id;
    return this_id < that_id? -1: this_id > that_id? 1: 0;
}


/**
 * threadpool_removeentry()
 * remove a thread from a thread pool, not destroying the etch_thread content.
 * @return the removed etch_thread object.
 */
etch_thread* threadpool_remove_entry (etch_threadpool* pool, const int etch_thread_id)
{
    int result = -1, i = 0;
    etch_thread* outthread = NULL;
    ETCH_ASSERT(is_etch_threadpool(pool));
    if (pool->is_defunct) return NULL;
    pool->pool_lock->acquire(pool->pool_lock);

    i = arraylist_indexof (pool->threadlist, (void*) (size_t) etch_thread_id,   
         0, etch_thread_arraylist_comparator);

    if (i >= 0)  
    {   outthread = arraylist_get (pool->threadlist, i);
        result = arraylist_remove (pool->threadlist, i, FALSE); 
    }

    pool->pool_lock->release(pool->pool_lock);
    return 0 == result? outthread: NULL;
}


/**
 * threadpool_remove()
 * remove a thread from a thread pool, destroying the etch_thread object.
 * this should not be invoked by thread pool destructor, since is_defunct is then true.
 */
int threadpool_remove (etch_threadpool* pool, const int etch_thread_id)
{
    int result = 0;
    ETCH_ASSERT(is_etch_threadpool(pool));
    if (pool->is_defunct) return -1;
    pool->pool_lock->acquire(pool->pool_lock);

    result = arraylist_remove_content (pool->threadlist, 
        (void*) (size_t) etch_thread_id, 0, etch_thread_arraylist_comparator); 

    pool->pool_lock->release(pool->pool_lock);
    return result;
}


/**
 * threadpool_waitfor_all()
 * block until all threads in the pool exit.
 * @param is_cancel if true, signal the thread to exit asap, 
 * otherwise wait for normal thread exit.
 */
int threadpool_waitfor_all (etch_threadpool* pool, const int is_cancel)
{
    etch_iterator iterator;
    etch_arraylist* listcopy = NULL;
    int  threadcount = 0, is_iterable = 0;
    if (NULL == pool) return -1;

    pool->pool_lock->acquire(pool->pool_lock);
    if ((threadcount = pool->count(pool)) > 0 && !pool->is_iterating)
        pool->is_iterating = is_iterable = TRUE;
    pool->pool_lock->release(pool->pool_lock);

    if (threadcount == 0) return 0;  /* no active threads to wait for */
    if (!is_iterable) return -1;    /* another thread is iterating this pool */
    
    /* iterate a copy since canceled threads remove themselves from their pool */
    /* note that iterating the actual pool may however be preferable if the
     * locking is done right, since if a thread ahead of the index exits 
     * during this iteration, it would presumably then not show up here. */
    listcopy = new_arraylist_from(pool->threadlist);
    listcopy->is_readonly = TRUE; /* so destructor will not free copied content */
    set_iterator(&iterator, listcopy, &listcopy->iterable);

    /* note that if pool.is_defunct is true, exiting threads which belong to  
     * that pool will not remove themselves from the pool and self-destruct. 
     * so, if is_defunct is true, threads exiting while we are iterating here 
     * will not be destroyed. the threadpool destructor destroy_threadpool() 
     * sets is_defunct. however if threadpool_waitforall() is invoked elsewhere,
     * care must be taken to ensure that pool threads do not exit while we
     * are iterating them here. */

    while(iterator.has_next(&iterator))  /* wait for each pool thread to exit */
    {
        etch_thread* thisthread = (etch_thread*) iterator.current_value;
        etch_thread* removedthread = NULL;

        if (thisthread) 
        {   const int thread_id = thisthread->params.etch_thread_id;
            char x[40]; sprintf(x, "thread %d in pool %d", thread_id, pool->threadpool_id);
          
            if  (is_cancel)
            {    etchlog(ETCHTHRE, ETCHLOG_DEBUG, "canceling %s ...\n", x); 
                 etch_thread_cancel (thisthread); /* BLOCK here */
            }
            else
            {    etchlog(ETCHTHRE, ETCHLOG_XDEBUG, "joining %s ...\n", x);
                 etch_join (thisthread);          /* BLOCK here */
            }

            etchlog(ETCHTHRE, ETCHLOG_DEBUG, "%s ended\n", x);
            
            if (pool->pooltype == ETCH_THREADPOOLTYPE_FREE)
            {   removedthread = threadpool_remove_entry (pool, thread_id);  
                thisthread->destroy (thisthread);   
            } 
        }
         
        iterator.next(&iterator);
    } 

    pool->is_iterating = FALSE;
    listcopy->destroy(listcopy); /* readonly set above so no content destroyed */
    return 0;
}


/**
 * destroy_threadpool()
 * etch_threadpool destructor.
 * @todo add logic for queued pool, or create separate destructor.
 */
int destroy_threadpool(etch_threadpool* pool)
{
    int can_destroy = TRUE;
    if (NULL == pool) return 0;
    if (pool->refcount > 0 && --pool->refcount > 0) return -1; 

    pool->pool_lock->acquire(pool->pool_lock);
    if  (pool->is_defunct)  /* ensure no race */
         can_destroy = FALSE;
    else pool->is_defunct = TRUE; 
    pool->pool_lock->release(pool->pool_lock);
    if (!can_destroy) return -1;

    threadpool_waitfor_all (pool, TRUE);  /* BLOCK until all threads exited */

    /* destroy the threadlist, destroying the etch_thread content in the process.
     * note that each thread's parameter list was destroyed as the thread exited. 
     * threadlist owns the mutex assigned to it and will destroy threadlist_lock.
     */
    if (!is_etchobj_static_content(pool))
    {
        pool->threadlist->destroy(pool->threadlist);
        pool->pool_lock->destroy(pool->pool_lock);
    }

    return destroy_objectex((objmask*)pool);
} 


/**
 * threadpool_count()
 * return count of threads in list 
 */
int threadpool_count(etch_threadpool* pool)
{
    int count = 0;
    ETCH_ASSERT(pool);
    pool->pool_lock->acquire(pool->pool_lock);
    count = pool->threadlist? pool->threadlist->count: 0;
    pool->pool_lock->release(pool->pool_lock);
    return count;
}


/**
 * threadpool_thread()
 * return thread[i] from specified threadpool.
 * @param i the index.
 * @return the etch_thread*, or null if no thread at that index.
 */
etch_thread* threadpool_thread (etch_threadpool* pool, const int i)
{
    etch_thread* thisthread = NULL;
    if (NULL == pool)  return NULL;

    pool->pool_lock->acquire(pool->pool_lock);

    thisthread = i < pool->count(pool)? pool->threadlist->base[i]: NULL;
  
    pool->pool_lock->release(pool->pool_lock);
     
    return thisthread;
}


/**
 * new_threadpool_list()
 * return an arraylist configured appropriately for a thread pool. 
 */
etch_arraylist* new_threadpool_list(const int initsize, etchmutex* mutex)
{
    etch_arraylist* list = new_arraylist(initsize,0);
    /* ETCHARRAYLIST_CONTENT_OBJECT lets list call destroy() on its content */
    list->content_type = ETCHARRAYLIST_CONTENT_OBJECT;
    list->is_readonly  = TRUE; /* list does not own content */
    list->synclock = mutex;    /* list owns this mutex */
    list->synchook = etchmutex_hookproc;
    return list;
} 
   

/**
 * new_threadpool()
 * etch_threadpool constructor
 * @param pooltype ETCH_THREADPOOLTYPE_FREE (default); ETCH_THREADPOOLTYPE_QUEUED 
 * @param initsize initial number of threads, ignored for ETCH_THREADPOOLTYPE_FREE. 
 */
etch_threadpool* new_threadpool(const unsigned pooltype, const int initsize)
{
    apr_pool_t* mempool;

    etch_threadpool* pool = (etch_threadpool*) new_object
        (sizeof(etch_threadpool), ETCHTYPEB_THREADPOOL, CLASSID_THREADPOOL);

    pool->destroy = destroy_threadpool;
    pool->clone   = clone_null;
    pool->count   = threadpool_count;
    pool->run     = pooltype == ETCH_THREADPOOLTYPE_QUEUED?
                    etch_threadpool_run_poolthread: etch_threadpool_run_freethread;

    mempool = etch_apr_mempool;
    pool->pool_lock = new_mutex(mempool, ETCHMUTEX_NESTED);

    pool->threadlist_lock = new_mutex(mempool, ETCHMUTEX_NESTED);
    pool->threadlist = new_threadpool_list(initsize, pool->threadlist_lock);

    pool->is_free_data = TRUE;
    pool->is_data_etchobject = FALSE;
    pool->threadpool_id = ++etch_threadpool_id_farm;
  
    return pool;
} 


/**
 * etch_glopool_exit()
 * wait on all threads in global pool to exit, destroy global pool.
 */
int etch_glopool_exit()
{
    destroy_threadpool(global_free_threadpool);
    global_free_threadpool = NULL;
    return 0;
}


/**
 * global_pool()
 * return singleton global free thread pool
 */
etch_threadpool* etch_glopool()
{
    if (global_free_threadpool == NULL)
        global_free_threadpool = new_threadpool(ETCH_THREADPOOLTYPE_FREE, 0);
    return global_free_threadpool;
}



/**
 * global_pool()
 * return singleton global free thread pool
 * todo destroy this pool somewhere
 */
etch_threadpool* global_pool()
{
    if (global_free_threadpool == NULL)
        global_free_threadpool = new_threadpool(ETCH_THREADPOOLTYPE_FREE, 0);
    return global_free_threadpool;
}


/**
 * getter for APR thread id
 * seems to be wrong - the ID is the same for each thread
 */
size_t etch_get_threadid (void* threadstruct)
{   
    /* can't address apr_thread_t content so we remap part of apr_thread_t */
    struct x { void* mempool; void* threadid; };
    return threadstruct? (size_t) ((struct x*)threadstruct)->threadid: 0;
}


/**
 * get_etch_aprpool()
 * getter for etch global apr memory pool
 */
apr_pool_t* get_etch_aprpool()
{
    return etch_apr_mempool;
}


/**
 * etch_apr_init()
 * runtime initialize apr
 */
int etch_apr_init()
{
    if (etch_apr_mempool) return 0;
    apr_pool_create(&etch_apr_mempool, NULL);
    loglock = &loglockx;
    return etch_init_static_mutex(loglock, TRUE);
}


/**
 * next_etch_threadid()
 * get a sequential ID we use to key threads
 */
unsigned next_etch_threadid()
{
    do 
    { apr_atomic_inc32 ((volatile apr_uint32_t *) &thread_id_farm);
    } while(thread_id_farm == 0);

    return thread_id_farm;
}


/**
 * default_on_threadstart()
 * default thread start callback.
 * the argument passed to start and exit callbacks is the etch_thread*
 */
int default_on_threadstart(void* param)
{
    int  thread_id = 0;
    etch_thread* threadx = (etch_thread*) param;
    etch_threadparams* p = threadx? &threadx->params: 0;
    if (p) thread_id = p->etch_thread_id;
    ETCH_ASSERT(thread_id);

    etchlog(ETCHTHRE, ETCHLOG_DEBUG, "thread %d enter\n", thread_id); 
    return 0;
}


/**
 * default_on_threadexit()
 * default thread exit callback.
 * the argument passed to start and exit callbacks is the etch_thread*
 */
int default_on_threadexit(void* param)
{    
    etch_thread* thisthread = (etch_thread*) param;
    etch_thread* removedthread = NULL;
    etch_threadpool* pool = NULL;
    etch_threadparams* p;
    int  thread_id = 0;
    ETCH_ASSERT(is_etch_thread(thisthread));
    p = &thisthread->params;
    thread_id = p->etch_thread_id;
    pool = p->etchpool;
   
    /* we remove the thread from its threadpool if a free pool, and unless the
     * pool is being destroyed, and unless the pool is otherwise being iterated
     * elswhere - we can't destroy a thread that another thread has joined, for
     * instance, the joiner will do that once unblocked. and we do not remove a
     * pool entry if we are currently iterating the pool.
     */
    if (pool && !pool->is_defunct) 
    {
        const int is_freethread = (pool->pooltype == ETCH_THREADPOOLTYPE_FREE);
        char x[40]; sprintf(x, "thread %d from pool %d", thread_id, pool->threadpool_id);
        etchlog(ETCHTHRE, ETCHLOG_XDEBUG, "removing %s ...\n", x); 
        pool->pool_lock->acquire(pool->pool_lock);
        /* todo dispose of thread elsewhere when is_iterating is true */

        removedthread = threadpool_remove_entry (pool, thread_id); 

        if (NULL == removedthread) /* occasionally observed while in debugger */
            etchlog(ETCHTHRE, ETCHLOG_WARNING, "%s was previously removed\n", x); 
        else    
        if (removedthread != thisthread)  /* condition never observed to date */
            etchlog(ETCHTHRE, ETCHLOG_ERROR, "%s removed unexpectedly\n", x); 
        else
        /* if either the pool is configured to free threads on exit,
         * or this is a free pool that we are not currently iterating over ... */
        if  (!removedthread->is_joined && is_freethread 
         && (pool->is_free_threads || !pool->is_iterating))
        {    removedthread->destroy (removedthread);   
             etchlog(ETCHTHRE, ETCHLOG_XDEBUG, "%s removed and destroyed\n", x);  
        }
        else 
            etchlog(ETCHTHRE, ETCHLOG_XDEBUG, "%s removed\n", x); 

        pool->pool_lock->release(pool->pool_lock);
    }   
    else 
        destroy_threadparams(thisthread);   

    /* we always destroy the thread's caller-allocated parameter memory, but we
     * do not however destroy the etch_thread wrapper when the thread is not in 
     * a threadpool. memory for non-pool threads is managed by caller.
     */
    etchlog(ETCHTHRE, ETCHLOG_DEBUG, "thread %d exit\n", thread_id); 
    return 0;
}


/**
 * etch_init_static_mutex()
 * etchmutex static initializer.
 * uses etch global apr memory pool.
 * defined here in etchthread to avoid circular include
 */
int etch_init_static_mutex(etchmutex* mx, const int is_nestable)
{
    apr_thread_mutex_t* amx = 0;
   if (NULL == etch_apr_mempool) etch_apr_init();
   if (NULL == etch_apr_mempool) return -1;
 
    mx->destroy = NULL; /* since static or on stack */
    mx->clone   = clone_null;
    mx->acquire = etchmutex_acquire;
    mx->release = etchmutex_release;
    mx->tryacquire = etchmutex_tryacquire;

    apr_thread_mutex_create(&amx, 
        is_nestable? APR_THREAD_MUTEX_NESTED: APR_THREAD_MUTEX_UNNESTED, 
        etch_apr_mempool);

    mx->impl = amx;
    return amx? 0: -1;
}


/**
 * etch_sleep()
 * interface to OS sleep in milliseconds
 */
void etch_sleep(const int ms)
{
    apr_sleep(ms * 1000);
}

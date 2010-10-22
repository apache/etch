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
#include "etch_thread.h"
#include "etch_log.h"
#include "etch_objecttypes.h"
#include "etch_mem.h"

const char* LOG_CATEGORY =  "etch_thread";

extern apr_pool_t*         g_etch_main_pool;
extern apr_thread_mutex_t* g_etch_main_pool_mutex;

// TODO: refactoring this stuff

etch_threadpool* global_free_threadpool;

size_t etch_get_threadid(void* threadstruct);
int default_on_threadstart(void* param);
int default_on_threadexit (void* param);
int destroy_thread   (void*);
int etch_thread_start(void*);
int etch_thread_stop(void*);
int destroy_threadparams(etch_thread*);
etch_mutex* loglock; // remove ??

unsigned short etch_threadpool_id_farm;
unsigned thread_id_farm;
etch_mutex* loglock; // remove ??

#ifdef APR_POOL_DEBUG
APR_DECLARE(apr_size_t) apr_pool_num_bytes(apr_pool_t *p, int recurse);
#endif

typedef struct os_thread_and_etch_thread {
  apr_os_thread_t* os_thread;
  etch_thread* etch_thread;
} os_thread_and_etch_thread;

int etch_thread_find_by_os_thread(void* data, void* to_find) {
  apr_os_thread_t* os_thread_to_find = (apr_os_thread_t*)to_find;
  os_thread_and_etch_thread* both = (os_thread_and_etch_thread*)data;

  return both->os_thread == os_thread_to_find;
}

/**
 * etch_apr_threadproc()
 * internal thread proc. this is a wrapper around the user thread proc.
 * @param data expected to point at an etch_thread_params, which contains the
 * thread start amd stop handlers, and the real threadproc and threadproc data.
 */
static void* APR_THREAD_FUNC etch_apr_threadproc(apr_thread_t *thread, void *data)
{
    etch_status_t status = ETCH_SUCCESS;
    etch_thread_params* params = (etch_thread_params*) data;
    etch_thread_callback on_exit;   
    etch_thread* threadobj;
    if(!params) return (void*)(-1);
    on_exit   = params->on_exit;
    threadobj = params->etchobj;

    if (threadobj && threadobj->waiter)  /* wait for signal to start */
      {
        status = etch_wait_wait(threadobj->waiter, 1);
        if(status != ETCH_SUCCESS) {
            // erro
        }

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

    apr_thread_exit(thread, APR_SUCCESS);

    return 0;
}


/**
 * new_thread()
 * etch_thread constructor
 */
etch_thread* new_thread (etch_threadproc proc, void* tdata)
{
    etch_status_t status = ETCH_SUCCESS;
    etch_thread_params* tp  = NULL;
    apr_pool_t* newsubpool = NULL;
    etch_thread* newthread = NULL;
    /* size_t poolsize = apr_pool_num_bytes(get_etch_aprpool(), FALSE); */  

    newthread = (etch_thread*) new_object
        (sizeof(etch_thread), ETCHTYPEB_THREAD, CLASSID_THREAD);

    ((etch_object*)newthread)->destroy = destroy_thread;
    ((etch_object*)newthread)->clone   = clone_null;

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

    // TODO: pool
    status = etch_wait_create(&newthread->waiter, NULL);
    if(status != ETCH_SUCCESS) {
        // error log it
    }
    newthread->startCond  = 0;

    //newthread->waiter->cond_var = &newthread->startCond;

	if (0 != etch_createthread(tp)) 
	{
        etch_wait_destroy(newthread->waiter);
		etch_free(newthread);
        newthread = NULL;
        //printf("2 destroying apr pool %p\n",newsubpool);
        apr_pool_destroy(newsubpool);
        
    } 

    return newthread;
} 


/**
 * etch_createthread()
 * to invoke: 1) initalize an etch_apr_threaddata with the mempool; 
 * 2) initialize an etch_thread_params with threadproc plus the etch_apr_threaddata,
 *    plus the thread user data.
 * 2) call with the threaddata, and the userdata.
 */
int etch_createthread(etch_thread_params* params)
{
    int result = 0;
    apr_status_t aprstatus;

    if (!params->on_start) params->on_start = default_on_threadstart;
    if (!params->on_exit)  params->on_exit  = default_on_threadexit;

    /* fyi: this sets thread.aprdata.threadattr */
    // TODO: pool
    apr_thread_mutex_lock(g_etch_main_pool_mutex);
    result = (APR_SUCCESS == apr_threadattr_create(&params->libdata->threadattr, g_etch_main_pool))? 0: -1;
    apr_thread_mutex_unlock(g_etch_main_pool_mutex);
    if (result == 0) {
        // TODO: pool
        apr_thread_mutex_lock(g_etch_main_pool_mutex);
        aprstatus = apr_thread_create(&params->libdata->thread, params->libdata->threadattr, etch_apr_threadproc, params, g_etch_main_pool);
        apr_thread_mutex_unlock(g_etch_main_pool_mutex);
        if (aprstatus != APR_SUCCESS) {
            char buffer[512];
            apr_strerror(aprstatus, buffer, 512);
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "thread create error error-code: %d error-msg: %s\n", aprstatus, buffer);
        }
    }
    return result;
}

int find_etch_thread_by_os_thread(void* listentry, void* apr_os_thread)
{
    os_thread_and_etch_thread* pair = (os_thread_and_etch_thread*) listentry;
    return pair->os_thread == apr_os_thread;
}   

/**
 * destroy_thread()
 * etch_thread destructor
 */
int destroy_thread(void* data)
{
    etch_thread* threadx = (etch_thread*)data;
    

    if (!is_etchobj_static_content(threadx))
    {
        if (threadx->waiter) {
            etch_wait_destroy(threadx->waiter);
        }
    }

    return destroy_objectex((etch_object*)threadx);   
}


/**
 * destroy_threadparams()
 * free memory for any heap parameters passed to an etch_thread.
 */
int destroy_threadparams(etch_thread* thisx)
{
    etch_thread_params* params = &thisx->params;
    void* userdata = params->data;

    if (params->waitobj)
        etch_wait_destroy(params->waitobj);

    if (params->is_own_data)
        if (params->is_data_etchobject)
           ((etch_object*)userdata)->destroy(userdata);
        else etch_free(userdata);

    return 0;
}


/**
 * etch_init_threadparams()
 */
void etch_init_threadparams(etch_thread_params* p)
{
    if (NULL == p) return;
    memset(p, 0, sizeof(etch_thread_params));
    p->signature = ETCH_THREAD_PARAMS_SIGNATURE;
}


/**
 * etch_thread_start()
 * default thread start method for etch_threads which wait for a start signal
 */
int etch_thread_start(void* data)
{
  etch_thread* threadx = (etch_thread*)data;
    etch_status_t status = ETCH_SUCCESS;
    int result = -1;
    const int thread_id = threadx->params.etch_thread_id;
    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "thread %d starting ...\n", thread_id); 

    if (threadx->waiter) {
        status = etch_wait_set(threadx->waiter, 1);
        // log error
    }

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
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "thread %d started\n", thread_id); 
             break;
        case ETCH_THREADSTATE_STOPPED:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "thread %d stopped\n", thread_id); 
             break;
        case ETCH_THREADSTATE_DEFUNCT:
             ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "thread %d was started now destroyed\n", thread_id); 
             break;
        default: ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "thread %d state %d\n", thread_id, threadstate);
    }
    #endif

    return result;
}


/**
 * etch_thread_stop()
 * default etch_thread thread stop method.
 * currently never invoked. causes serious grief.
 */
int etch_thread_stop(void* data)
{
    etch_thread* thread = (etch_thread*)data;
    apr_status_t tresult = 0;   
    etch_thread_params* p = thread? &thread->params: NULL;
    if (NULL == p) return -1;

    tresult = apr_thread_detach(p->libdata->thread);

    tresult = apr_thread_exit(p->libdata->thread, 0);

    return APR_SUCCESS == tresult? 0: -1;
}

int etch_thread_current_id()
{
#if defined(WIN32)
    return GetCurrentThreadId();
#elif defined(__QNX__) || defined(__LINUX__) || defined(__APPLE__)
    #warning  not implemented for this os
    return 0;
#else
    #error OS no support
#endif

    //int tid = -1;
    //void* p = NULL;
    //apr_os_thread_t thread_t = apr_os_thread_current();
    //apr_os_threadkey_t threadkey_t;
    //apr_os_threadkey_get(&threadkey_t, );
    //p = thread_t;
    ////tid = p;
    //printf("%p", p);
    //tid = *(int*)p;
    
    return 0;
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

    etch_arraylist_add(pool->threadlist, newthread);  
    /* an etch_thread object in the threadpool gets freed when a thread exits, 
     * finds its threadpool, and removes itself from that pool's threadlist. 
     */
    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "thread %d created on pool %d\n", 
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
   etch_thread_params* tp = NULL;
   apr_status_t tresult = 0;   /* result returned from dead thread */

   if(thread == NULL)
       return -1;
   tp = &thread->params;
   thread->is_joined = TRUE;  /* mark to avoid on_exit destruction */
   result = apr_thread_join (&tresult, tp->libdata->thread); 
   return result;
}


/**
 * etch_thread_join()
 * block until specified thread exits
 */
int etch_thread_join(etch_thread_params* params)
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
    etch_status_t status = ETCH_SUCCESS;
    int result = -1, i = 0;
    etch_thread* outthread = NULL;
    ETCH_ASSERT(is_etch_threadpool(pool));
    if (pool->is_defunct) return NULL;

    status = etch_mutex_lock(pool->pool_lock);
    if(status != ETCH_SUCCESS) {
        // error
    }

    i = etch_arraylist_indexof (pool->threadlist, (void*) (size_t) etch_thread_id,   
         0, etch_thread_arraylist_comparator);

    if (i >= 0)  
    {   outthread = etch_arraylist_get (pool->threadlist, i);
        result = etch_arraylist_remove (pool->threadlist, i, FALSE); 
    }

    status = etch_mutex_unlock(pool->pool_lock);
    if(status != ETCH_SUCCESS) {
        // error
    }

    return 0 == result? outthread: NULL;
}


/**
 * threadpool_remove()
 * remove a thread from a thread pool, destroying the etch_thread object.
 * this should not be invoked by thread pool destructor, since is_defunct is then true.
 */
int threadpool_remove(etch_threadpool* pool, const int etch_thread_id)
{
    etch_status_t status = ETCH_SUCCESS;
    int result = 0;
    ETCH_ASSERT(is_etch_threadpool(pool));
    if (pool->is_defunct) return -1;

    status = etch_mutex_lock(pool->pool_lock);
    if(status != ETCH_SUCCESS) {
        // error
    }

    result = etch_arraylist_remove_content (pool->threadlist, 
        (void*) (size_t) etch_thread_id, 0, etch_thread_arraylist_comparator); 

    status = etch_mutex_unlock(pool->pool_lock);
    if(status != ETCH_SUCCESS) {
        // error
    }

    return result;
}


/**
 * threadpool_waitfor_all()
 * block until all threads in the pool exit.
 * @param is_cancel if true, signal the thread to exit asap, 
 * otherwise wait for normal thread exit.
 */
int threadpool_waitfor_all(etch_threadpool* pool, const int is_cancel)
{
    etch_status_t status = ETCH_SUCCESS;
    etch_iterator iterator;
    etch_arraylist* listcopy = NULL;
    int  threadcount = 0, is_iterable = 0;
    if (NULL == pool) return -1;

    status = etch_mutex_lock(pool->pool_lock);
    if(status != ETCH_SUCCESS) {
        // error
    }
    if ((threadcount = pool->count(pool)) > 0 && !pool->is_iterating)
        pool->is_iterating = is_iterable = TRUE;
    status = etch_mutex_unlock(pool->pool_lock);
    if(status != ETCH_SUCCESS) {
        // error
    }

    if (threadcount == 0) return 0;  /* no active threads to wait for */
    if (!is_iterable) return -1;    /* another thread is iterating this pool */
    
    /* iterate a copy since canceled threads remove themselves from their pool */
    /* note that iterating the actual pool may however be preferable if the
     * locking is done right, since if a thread ahead of the index exits 
     * during this iteration, it would presumably then not show up here. */
    status = etch_mutex_lock(pool->pool_lock);
    if(status != ETCH_SUCCESS) {
        // error
    }
    listcopy = new_etch_arraylist_from(pool->threadlist);
    status = etch_mutex_unlock(pool->pool_lock);
    if(status != ETCH_SUCCESS) {
        // error
    }
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
            char x[60];
            apr_snprintf(x, sizeof(x), "thread %d in pool %d", thread_id, pool->threadpool_id);
          
            if  (is_cancel)
            {    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "canceling %s ...\n", x); 
                 etch_thread_cancel (thisthread); /* BLOCK here */
            }
            else
            {    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_XDEBUG, "joining %s ...\n", x);
                 etch_join (thisthread);          /* BLOCK here */
            }

            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s ended\n", x);
            
            status = etch_mutex_lock(pool->pool_lock);
            if(status != ETCH_SUCCESS) {
                // error
            }
            if (pool->pooltype == ETCH_THREADPOOLTYPE_FREE)
            {   removedthread = threadpool_remove_entry (pool, thread_id);  
                etch_object_destroy (thisthread);   
            }
            status = etch_mutex_unlock(pool->pool_lock);
            if(status != ETCH_SUCCESS) {
                // error
            }
        }
         
        iterator.next(&iterator);
    } 

    pool->is_iterating = FALSE;
    etch_object_destroy(listcopy); /* readonly set above so no content destroyed */
    return 0;
}


/**
 * destroy_threadpool()
 * etch_threadpool destructor.
 * @todo add logic for queued pool, or create separate destructor.
 */
int destroy_threadpool(void* data)
{
    etch_threadpool* pool = (etch_threadpool*)data;
    etch_status_t status = ETCH_SUCCESS;
    int can_destroy = TRUE;
    if (NULL == pool) return 0;

    status = etch_mutex_lock(pool->pool_lock);
    if(status != ETCH_SUCCESS) {
        // error
    }
    if  (pool->is_defunct)  /* ensure no race */
         can_destroy = FALSE;
    else pool->is_defunct = TRUE; 
    status = etch_mutex_unlock(pool->pool_lock);
    if(status != ETCH_SUCCESS) {
        // error
    }
    if (!can_destroy) return -1;

    threadpool_waitfor_all (pool, TRUE);  /* BLOCK until all threads exited */

    /* destroy the threadlist, destroying the etch_thread content in the process.
     * note that each thread's parameter list was destroyed as the thread exited. 
     * threadlist owns the mutex assigned to it and will destroy threadlist_lock.
     */
    if (!is_etchobj_static_content(pool))
    {
        etch_object_destroy(pool->threadlist);
        status = etch_mutex_destroy(pool->pool_lock);
        if(status != ETCH_SUCCESS) {
            // error
        }
    }

    return destroy_objectex((etch_object*)pool);
} 


/**
 * threadpool_count()
 * return count of threads in list 
 */
int threadpool_count(etch_threadpool* pool)
{
    etch_status_t status = ETCH_SUCCESS;
    int count = 0;
    ETCH_ASSERT(pool);
    status = etch_mutex_lock(pool->pool_lock);
    if(status != ETCH_SUCCESS) {
        // error
    }
    count = pool->threadlist? pool->threadlist->count: 0;
    status = etch_mutex_unlock(pool->pool_lock);
    if(status != ETCH_SUCCESS) {
        // error
    }
    return count;
}


/**
 * threadpool_thread()
 * return thread[i] from specified threadpool.
 * @param i the index.
 * @return the etch_thread*, or null if no thread at that index.
 */
etch_thread* threadpool_thread(etch_threadpool* pool, const int i)
{
    etch_status_t status = ETCH_SUCCESS;
    etch_thread* thisthread = NULL;
    if (NULL == pool)  return NULL;

    status = etch_mutex_lock(pool->pool_lock);
    if(status != ETCH_SUCCESS) {
        // error
    }
    thisthread = i < pool->count(pool)? pool->threadlist->base[i]: NULL;
    status = etch_mutex_unlock(pool->pool_lock);
    if(status != ETCH_SUCCESS) {
        // error
    }
    return thisthread;
}


/**
 * new_threadpool_list()
 * return an arraylist configured appropriately for a thread pool. 
 */
etch_arraylist* new_threadpool_list(const int initsize, etch_mutex* mutex)
{
    //TODO: Check if arraylist has to be synchronized
    etch_arraylist* list = new_etch_arraylist(initsize,0);
    /* ETCHARRAYLIST_CONTENT_OBJECT lets list call destroy() on its content */
    list->content_type = ETCHARRAYLIST_CONTENT_OBJECT;
    list->is_readonly  = TRUE; /* list does not own content */
    ((etch_object*)list)->synclock = mutex;    /* list owns this mutex */
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
    etch_status_t status = ETCH_SUCCESS;

    etch_threadpool* pool = (etch_threadpool*) new_object
        (sizeof(etch_threadpool), ETCHTYPEB_THREADPOOL, CLASSID_THREADPOOL);

    ((etch_object*)pool)->destroy = destroy_threadpool;
    ((etch_object*)pool)->clone   = clone_null;
    pool->count   = threadpool_count;
    pool->run     = pooltype == ETCH_THREADPOOLTYPE_QUEUED?
                    etch_threadpool_run_poolthread : etch_threadpool_run_freethread;

    // TODO: pool
    status = etch_mutex_create(&pool->pool_lock, ETCH_MUTEX_NESTED, NULL);
    if(status != ETCH_SUCCESS) {
        // error
    }

    // TODO: pool
    status = etch_mutex_create(&pool->threadlist_lock, ETCH_MUTEX_NESTED, NULL);
    if(status != ETCH_SUCCESS) {
        // error
    }

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
 * next_etch_threadid()
 * get a sequential ID we use to key threads
 */
unsigned int next_etch_threadid()
{
    do 
    {
        apr_atomic_inc32((volatile apr_uint32_t*) &thread_id_farm);
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
    etch_thread_params* p = threadx? &threadx->params: 0;
    if (p) thread_id = p->etch_thread_id;
    ETCH_ASSERT(thread_id);

    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "thread %d enter\n", thread_id); 
    return 0;
}


/**
 * default_on_threadexit()
 * default thread exit callback.
 * the argument passed to start and exit callbacks is the etch_thread*
 */
int default_on_threadexit(void* param)
{
    etch_status_t status = ETCH_SUCCESS;
    etch_thread* thisthread = (etch_thread*) param;
    etch_thread* removedthread = NULL;
    etch_threadpool* pool = NULL;
    etch_thread_params* p;
    int  thread_id = 0;
    ETCH_ASSERT(thisthread);
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
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_XDEBUG, "removing %s ...\n", x); 

        status = etch_mutex_lock(pool->pool_lock);
        if(status != ETCH_SUCCESS) {
            // error
        }
        /* todo dispose of thread elsewhere when is_iterating is true */

        removedthread = threadpool_remove_entry (pool, thread_id); 

        if (NULL == removedthread) /* occasionally observed while in debugger */
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_WARN, "%s was previously removed\n", x); 
        else    
        if (removedthread != thisthread)  /* condition never observed to date */
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s removed unexpectedly\n", x); 
        else {
            /* if either the pool is configured to free threads on exit,
             * or this is a free pool that we are not currently iterating over ... */
            if  (!removedthread->is_joined && is_freethread 
             && (pool->is_free_threads || !pool->is_iterating))
            {    
                etch_object_destroy(removedthread);   
                ETCH_LOG(LOG_CATEGORY, ETCH_LOG_XDEBUG, "%s removed and destroyed\n", x);  
            }
            else 
                ETCH_LOG(LOG_CATEGORY, ETCH_LOG_XDEBUG, "%s removed\n", x); 
        }

        status = etch_mutex_unlock(pool->pool_lock);
        if(status != ETCH_SUCCESS) {
            // error
        }
    }   
    else 
        destroy_threadparams(thisthread);   

    /* we always destroy the thread's caller-allocated parameter memory, but we
     * do not however destroy the etch_thread wrapper when the thread is not in 
     * a threadpool. memory for non-pool threads is managed by caller.
     */
    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "thread %d exit\n", thread_id); 
    return 0;
}


/**
 * etch_sleep()
 * interface to OS sleep in milliseconds
 */
void etch_sleep(const int ms)
{
    apr_sleep(ms * 1000);
}

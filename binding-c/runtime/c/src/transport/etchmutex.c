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
 * etchmutex.c
 * currently wraps apr mutex. our wrapper carries with it the benefit of 
 * tracking memory for our mutex, and thus for the underlying apr mutex.
 */

#include "apr_network_io.h"
#include "apr_errno.h"
#include "apr_general.h"
#include "apr_thread_mutex.h"
#include "apr_thread_cond.h"
#include "apr_portable.h"  /* apr_os_thread_current() */

#include "etchmutex.h"
#include "etch_global.h"
#include "etchlog.h"


/**
 * etchmutex_acquire()
 */
int etchmutex_acquire(etchmutex* mx) 
{
    int result = -1;
    apr_thread_mutex_t* amx =(apr_thread_mutex_t*) mx->impl;
    /* void* thisthread = apr_os_thread_current(); */
    
    /* if (mx->lockcount == 0 || apr_os_thread_equal(mx->owning_thread, thisthread)) */
    result = apr_thread_mutex_lock(amx);

    if (0 == result)
    {   mx->lockcount++;
        /* mx->owning_thread = thisthread; */
    }
    else result = -1;

    return result;
}


/**
 * etchmutex_tryacquire()
 */
int etchmutex_tryacquire(etchmutex* mx) 
{
    int result = -1;
    apr_thread_mutex_t* amx =(apr_thread_mutex_t*) mx->impl;
    /* void* thisthread = apr_os_thread_current(); */

    /* if (mx->lockcount == 0 || apr_os_thread_equal(mx->owning_thread, thisthread)) */
    result = apr_thread_mutex_trylock(amx);

    if (0 == result)
    {   mx->lockcount++;
        /* mx->owning_thread = thisthread; */
    }
    else result = -1;

    return result;
}


/**
 * etchmutex_release()
 */
int etchmutex_release(etchmutex* mx) 
{
    apr_thread_mutex_t* amx =(apr_thread_mutex_t*) mx->impl;

    int result = apr_thread_mutex_unlock(amx);

    if (0 == result)
    {   if (mx->lockcount > 0)
            mx->lockcount--;
    }       
    else result = -1;
    return result;
}


/**
 * destroy_etchmutex()
 */
int destroy_etchmutex(etchmutex* mx) 
{
    apr_thread_mutex_t* amx = NULL;
    int result = 0;
    if (NULL == mx) return -1;
    if (mx->refcount > 0 && --mx->refcount > 0) return -1;  

    amx = (apr_thread_mutex_t*) mx->impl;

    if (!is_etchobj_static_content(mx))
        result = apr_thread_mutex_destroy(amx); 
 
    if (!is_etchobj_static_shell(mx))
        etch_free(mx);  

    return result == 0? 0: -1;
}
    
    
/**
 * new_mutex()
 * etchmutex constructor.
 * currently accepts apr memory pool as parameter.
 */
etchmutex* new_mutex(void* params, const int is_nestable)
{
    apr_thread_mutex_t* amx = 0;
    apr_status_t xresult = 0;
    apr_pool_t*  aprpool = 0;

    etchmutex* mx = (etchmutex*) new_object
        (sizeof(etchmutex), ETCHTYPEB_MUTEX, CLASSID_MUTEX);

    mx->destroy = destroy_etchmutex;
    mx->clone   = clone_null;
    mx->acquire = etchmutex_acquire;
    mx->release = etchmutex_release;
    mx->tryacquire = etchmutex_tryacquire;

    aprpool = (apr_pool_t*) params;
    xresult = apr_thread_mutex_create(&amx, 
        is_nestable? APR_THREAD_MUTEX_NESTED: APR_THREAD_MUTEX_UNNESTED, aprpool);

    mx->impl = amx;
    return amx? mx: NULL;
}


/**
 * etchmutex_hookproc()
 * callback for mutex control, used to avoid defining concrete mutex objects.
 * for example a synchronized list calls a hook to set and release its lock,
 * however the lock appears to the list as a void*.
 */
int etchmutex_hookproc(int action, void* mx)
{
    int result = -1;
    etchmutex* mutex = (etchmutex*) mx;
     
    if (NULL == mx);
    else switch(action)
    {
        case ETCH_SYNC_SET:
             result = mutex->acquire(mutex);
             break;
        case ETCH_SYNC_TRY:
             result = mutex->tryacquire(mutex);
             break;
        case ETCH_SYNC_REL:
             result = mutex->release(mutex);
             break;
        case ETCH_SYNC_DEL:
             result = mutex->destroy(mutex);
             break;
    }

    return result;
}


/* - - - - - -
 * etchwait
 * - - - - - -
 */

/**
 * destroy_etchwait()
 * etchwait destructor
 */
int destroy_etchwait(etchwait* waiter) 
{
    apr_thread_cond_t* tc = 0;
    int xresult  = 0;
    if (!waiter || (waiter->refcount > 0 && --waiter->refcount > 0)) return -1; 

    tc =(apr_thread_cond_t*) waiter->impl;

    if (!is_etchobj_static_content(waiter))
    {   xresult = apr_thread_cond_destroy(tc);  
        waiter->mutex->destroy(waiter->mutex); /* free mutex + wrapper */
    }
   
    if (!is_etchobj_static_shell(waiter))
        etch_free(waiter);  

    return xresult == 0? 0: -1;
}


/**
 * etchwait_waitex()
 * private method blocks until signaled or specified condition set or timeout.
 * if a specified wait condition preexists, no blocking occurs.
 * @return 0 signaled or preexisted, ETCH_TIMEOUT timeout,  
 * or -1 if unblocked not due to timeout and condition was not met, or error.
 * @return 0 OK (equality signaled or condition preexisted), ETCH_TIMEOUT timeout, -1 error.
 */
int etchwait_waitex (etchwait* waiter, int64* cond_var, int64 cond_waitfor, int timeoutms)
{
    apr_thread_cond_t*  tc =(apr_thread_cond_t*)  waiter->impl;
    apr_thread_mutex_t* tm =(apr_thread_mutex_t*) waiter->mutex->impl;
    int result = 0;
     
    waiter->mutex->acquire(waiter->mutex);

    do
    {   const int64* waitvar   = waiter->cond_var;
        const int64  waitvalue = waiter->cond_waitfor;
        const unsigned char is_same_monitortype 
            = (waiter->cond_var && cond_var) || (!waiter->cond_var && !cond_var);
        const unsigned char is_requesting_same_waitcond = waiter->cond_var 
           && (waiter->cond_var == cond_var && waiter->cond_waitfor == cond_waitfor);
        const unsigned char is_unblock_required = waiter->threadcount > 0  
           && (!is_same_monitortype || !is_requesting_same_waitcond);

        /* when new condition differs, unblock threads waiting on prior condition.
         * such threads recognize that the unblocking condition is not the monitored ,
         * condition, and return -1 from this method accordingly, see below. */
        if (is_unblock_required) 
            apr_thread_cond_broadcast(tc);

        waiter->cond_var = cond_var;
        waiter->cond_waitfor = cond_waitfor;

        waiter->threadcount++;

        if (waiter->cond_var && *(waiter->cond_var) == waiter->cond_waitfor)
            result = 0;    /* wait condition preexists */
        else                     
        if (timeoutms > 0) /* BLOCK here with timeout */
            result = apr_thread_cond_timedwait(tc, tm, timeoutms * 1000);
        else               /* BLOCK here until signaled */
            result = 0 == apr_thread_cond_wait(tc, tm)? 0: -1;

        /* thread is now unblocked. result will be APR_TIMEUP if timed out. */

        /* check if we were previously waiting for something other condition*/
        if((result == 0) && waitvar && waitvalue && (*waitvar != waitvalue))
            result = -2;    /* current state is not the monitored condition */

        switch(result)
        { case 0:  break;   /* 0 = APR_SUCCESS */
          case APR_TIMEUP: result = ETCH_TIMEOUT; break;
          default: result = -1; /* condition not met or error */
        }
        
        waiter->cond_var = NULL; /* reset */
        waiter->threadcount--;

    } while(0);

    waiter->mutex->release(waiter->mutex);
    return result;
}


/**
 * etchwait_wait_until_signaled()
 * block until signaled or specified condition set.
 * if a specified wait condition preexists, no blocking occurs.
 * @return 0 signaled or preexisted, -1 error.
 */
int etchwait_wait_until_signaled (etchwait* waiter)
{
    return etchwait_waitex (waiter, NULL, 0, 0);
}


/**
 * etchwait_timed_wait_until_signaled()
 * block until signaled or timout.
 * if a specified wait condition preexists, no blocking occurs.
 * @return 0 OK (signaled), ETCH_TIMEOUT timeout, -1 error.
 */
int etchwait_timed_wait_until_signaled (etchwait* waiter, int timeoutms)
{
    return etchwait_waitex (waiter, NULL, 0, timeoutms);
}


/**
 * etchwait_wait_until_equal()
 * block until signaled or specified condition set.
 * if a specified wait condition preexists, no blocking occurs.
 * @param cond_var a pointer to the monitored long integer. if null, no monitoring
 * occurs, and threads must be unblocked with signal().
 * @param cond_waitfor the value for which a set() will be compared for equality.
 * @return 0 signaled or preexisted, -1 error.
 */
int etchwait_wait_until_equal (etchwait* waiter, int64* cond_var, int64 cond_waitfor)
{
    return etchwait_waitex (waiter, cond_var, cond_waitfor, 0);
}


/**
 * etchwait_timed_wait_until_equal()
 * block until signaled or specified condition set.
 * if a specified wait condition preexists, no blocking occurs.
 * @param cond_var a pointer to the monitored long integer. if null, no monitoring
 * occurs, and threads must be unblocked with signal().
 * @param cond_waitfor the value for which a set() will be compared for equality.
 * @param timeoutms length of time to wait for the condition. if the equality condition
 * already exists, no blocking is performed and zero is returned.
 * @return 0 OK (equality signaled or condition preexisted), ETCH_TIMEOUT timeout, -1 error.
 */
int etchwait_timed_wait_until_equal (etchwait* waiter, int64* cond_var, int64 cond_waitfor, int timeoutms)
{
    return etchwait_waitex (waiter, cond_var, cond_waitfor, timeoutms);
}


/**
 * etchwait_signal()
 * unblock first waiting thread 
 */
int etchwait_signal (etchwait* waiter) 
{
    apr_thread_cond_t*  tc =(apr_thread_cond_t*)  waiter->impl;
    waiter->mutex->acquire(waiter->mutex);

    apr_thread_cond_signal(tc);
    
    waiter->mutex->release(waiter->mutex);
    return 0;
}


/**
 * etchwait_broadcast()
 * unblock all waiting threads
 */
int etchwait_broadcast (etchwait* waiter) 
{
    apr_thread_cond_t*  tc =(apr_thread_cond_t*)  waiter->impl;
    waiter->mutex->acquire(waiter->mutex);

    apr_thread_cond_broadcast(tc);
    
    waiter->mutex->release(waiter->mutex);
    return 0;
}


/**
 * etchwait_setvalue()
 * set condition variable and signal if wait condition met
 */
int etchwait_setvalue(etchwait* waiter, int64 newval)
{
    int result = -1;

    if (waiter->cond_var)  /* if waiter is monitoring a condition ... */
    {
        result = 0;
        *(waiter->cond_var) = newval;

        if (newval == waiter->cond_waitfor) /* ... if condition met ... */
            etchwait_broadcast(waiter); /* ... unblock waiting threads */
    }
     
    return result;
}


/**
 * new_wait()
 * etchwait constructor.
 * etchwait is a thread wait condition that blocks until signaled, or a condition is met, or timeout.
 * currently wraps an APR condition variable and mutex pair.
 * @param params the apr memory pool  
 */
etchwait* new_wait(void* params)
{
    apr_thread_cond_t* tcx = 0;
    apr_status_t xresult = 0;
    apr_pool_t* aprpool = 0;
    etchwait* newwait = 0;
    aprpool = (apr_pool_t*) params;

    xresult = apr_thread_cond_create(&tcx, aprpool);
    if (xresult != 0) return NULL;

    newwait = (etchwait*) new_object(sizeof(etchwait), ETCHTYPEB_WAIT, CLASSID_WAIT);
    if (NULL == newwait) return NULL;

    newwait->mutex = new_mutex(aprpool, ETCHMUTEX_UNNESTED);  
    newwait->impl  = tcx;  

    newwait->destroy         = destroy_etchwait;
    newwait->clone           = clone_null;
    newwait->wait            = etchwait_wait_until_signaled;
    newwait->timed_wait      = etchwait_timed_wait_until_signaled;
    newwait->waitequal       = etchwait_wait_until_equal;
    newwait->timed_waitequal = etchwait_timed_wait_until_equal;
    newwait->signal          = etchwait_signal;
    newwait->broadcast       = etchwait_broadcast;
    newwait->set             = etchwait_setvalue;

    return newwait;
}



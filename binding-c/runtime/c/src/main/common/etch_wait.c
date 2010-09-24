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
 * etch_wait.c -- thread wait implementation
 */

#include "etch_wait.h"
#include "etch_object.h"
#include "etch_objecttypes.h"
#include "etch_mutex.h"
#include "etch_log.h"

static const char* LOG_CATEGORY = "etch_wait";

extern apr_pool_t*         g_etch_main_pool;
extern apr_thread_mutex_t* g_etch_main_pool_mutex;

struct etch_wait_t
{
    etch_object object;

    void*  mutex;         /* associated mutex - owned */
    void*  cond;          /* wait implementation - owned */
    int64  cond_var;      /* wait condition var */
    int32  threadcount;   /* count of threads waiting */

};

etch_status_t etch_wait_create(etch_wait_t** waiter, etch_pool_t* pool)
{
    etch_wait_t*        newwaiter  = NULL;
    apr_status_t        apr_status = APR_SUCCESS;
    apr_thread_cond_t*  apr_cond   = NULL;
    apr_thread_mutex_t* apr_mutex  = NULL;

    if(waiter == NULL) {
        return ETCH_EINVAL;
    }

    /* if pool is null use global pool */
    if(pool == NULL) {
        pool = g_etch_main_pool;
    }

    newwaiter = (etch_wait_t*)new_object(sizeof(etch_wait_t), ETCHTYPEB_WAIT, CLASSID_WAIT);
    ETCH_ASSERT(newwaiter != NULL);

    apr_thread_mutex_lock(g_etch_main_pool_mutex);
    apr_status = apr_thread_mutex_create(&apr_mutex, APR_THREAD_MUTEX_UNNESTED, g_etch_main_pool);
    apr_thread_mutex_unlock(g_etch_main_pool_mutex);
    if(apr_status != APR_SUCCESS) {
        char temp[1024];
        apr_strerror(apr_status, temp, sizeof(temp));
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s\n", temp);
        etch_free(newwaiter);
        *waiter = NULL;
        return ETCH_ERROR;
    }

    apr_thread_mutex_lock(g_etch_main_pool_mutex);
    apr_status = apr_thread_cond_create(&apr_cond, g_etch_main_pool);
    apr_thread_mutex_unlock(g_etch_main_pool_mutex);
    if(apr_status != APR_SUCCESS) {
        char temp[1024];
        apr_strerror(apr_status, temp, sizeof(temp));
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s\n", temp);
        apr_thread_mutex_destroy(apr_mutex);
        etch_free(newwaiter);
        *waiter = NULL;
        return ETCH_ERROR;
    }

    newwaiter->mutex     = apr_mutex;
    newwaiter->cond      = apr_cond;
    newwaiter->cond_var  = 0;
    *waiter = newwaiter;
    return ETCH_SUCCESS;
}

etch_status_t etch_wait_set(etch_wait_t* waiter, int64 cond_value)
{
    etch_status_t       status = ETCH_SUCCESS;
    apr_status_t        apr_status = APR_SUCCESS;
    apr_thread_mutex_t* apr_mutex  = NULL;
    apr_thread_cond_t*  apr_cond   = NULL;

    if(waiter == NULL) {
        return ETCH_EINVAL;
    }

    apr_mutex = waiter->mutex;
    ETCH_ASSERT(apr_mutex != NULL);
    apr_cond  = waiter->cond;
    ETCH_ASSERT(apr_cond != NULL);

    apr_thread_mutex_lock(apr_mutex);

    waiter->cond_var = cond_value;
    apr_status = apr_thread_cond_broadcast(apr_cond);
    if(apr_status != APR_SUCCESS) {
        char temp[1024];
        apr_strerror(apr_status, temp, sizeof(temp));
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s\n", temp);
        status = ETCH_ERROR;
    }

    apr_thread_mutex_unlock(apr_mutex);

    return status;
}

etch_status_t etch_wait_reset(etch_wait_t* waiter, int64 cond_value)
{
    etch_status_t       status = ETCH_SUCCESS;
    apr_thread_mutex_t* apr_mutex  = NULL;

    if(waiter == NULL) {
        return ETCH_EINVAL;
    }

    apr_mutex = waiter->mutex;
    ETCH_ASSERT(apr_mutex != NULL);

    apr_thread_mutex_lock(apr_mutex);
    waiter->cond_var = cond_value;
    apr_thread_mutex_unlock(apr_mutex);

    return status;
}

etch_status_t etch_wait_wait_helper(etch_wait_t* waiter, int64 cond_value, int64* new_cond_value)
{
    etch_status_t       status     = ETCH_SUCCESS;
    apr_status_t        apr_status = APR_SUCCESS;
    apr_thread_mutex_t* apr_mutex  = NULL;
    apr_thread_cond_t*  apr_cond   = NULL;

    if(waiter == NULL) {
        return ETCH_EINVAL;
    }

    apr_mutex = waiter->mutex;
    ETCH_ASSERT(apr_mutex != NULL);
    apr_cond  = waiter->cond;
    ETCH_ASSERT(apr_cond != NULL);

    apr_thread_mutex_lock(apr_mutex);

    while(waiter->cond_var != cond_value) {
        apr_status = apr_thread_cond_wait(apr_cond, apr_mutex);
        if(apr_status != APR_SUCCESS) {
            char temp[1024];
            apr_strerror(apr_status, temp, sizeof(temp));
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s\n", temp);
            status = ETCH_ERROR;
            break;
        }
    }

    if(status == ETCH_SUCCESS && new_cond_value != NULL) {
        waiter->cond_var = *new_cond_value;
        apr_status = apr_thread_cond_broadcast(apr_cond);
        if(apr_status != APR_SUCCESS) {
            char temp[1024];
            apr_strerror(apr_status, temp, sizeof(temp));
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s\n", temp);
        }
    }

    apr_thread_mutex_unlock(apr_mutex);

    return status;
}

etch_status_t etch_wait_wait(etch_wait_t* waiter, int64 cond_value)
{
    etch_status_t status;
    status = etch_wait_wait_helper(waiter, cond_value, NULL);
    return status;
}

etch_status_t etch_wait_wait_and_set(etch_wait_t* waiter, int64 cond_value, int64 new_cond_value)
{
    etch_status_t status;
    status = etch_wait_wait_helper(waiter, cond_value, &new_cond_value);
    return status;
}

static etch_status_t etch_wait_timedwait_helper(etch_wait_t* waiter, int64 cond_value, uint32 timeout, int64* new_cond_value)
{
    etch_status_t       status     = ETCH_SUCCESS;
    apr_status_t        apr_status = APR_SUCCESS;
    apr_thread_mutex_t* apr_mutex  = NULL;
    apr_thread_cond_t*  apr_cond   = NULL;
    apr_time_t          now        = 0;
    apr_time_t          end        = 0;
    apr_time_t          diff       = 0;

    if(waiter == NULL) {
        return ETCH_EINVAL;
    }

    apr_mutex = waiter->mutex;
    ETCH_ASSERT(apr_mutex != NULL);
    apr_cond  = waiter->cond;
    ETCH_ASSERT(apr_cond != NULL);

    now = apr_time_now();
    end = now + timeout * 1000;

    apr_thread_mutex_lock(apr_mutex);

    diff = end - now;
    while(waiter->cond_var != cond_value && diff > 0) {
        apr_status = apr_thread_cond_timedwait(apr_cond, apr_mutex, diff);
        if(apr_status != APR_TIMEUP && apr_status != APR_SUCCESS) {
            char temp[1024];
            apr_strerror(apr_status, temp, sizeof(temp));
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s\n", temp);
            status = ETCH_ERROR;
            break;
        }

        now = apr_time_now();
        diff = end - now;
    }

    if(status != ETCH_ERROR && waiter->cond_var != cond_value) {
        status = ETCH_ETIMEOUT;
    }

    if(status == ETCH_SUCCESS && new_cond_value != NULL) {
        waiter->cond_var = *new_cond_value;
        apr_status = apr_thread_cond_broadcast(apr_cond);
        if(apr_status != APR_SUCCESS) {
            char temp[1024];
            apr_strerror(apr_status, temp, sizeof(temp));
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s\n", temp);
        }
    }

    apr_thread_mutex_unlock(apr_mutex);

    return status;
}

etch_status_t etch_wait_timedwait(etch_wait_t* waiter, int64 cond_value, uint32 timeout)
{
    etch_status_t status;
    status = etch_wait_timedwait_helper(waiter, cond_value, timeout, NULL);
    return status;
}

etch_status_t etch_wait_timedwait_and_set(etch_wait_t* waiter, int64 cond_value, uint32 timeout, int64 new_cond_value)
{
    etch_status_t status;
    status = etch_wait_timedwait_helper(waiter, cond_value, timeout, &new_cond_value);
    return status;
}

etch_status_t etch_wait_destroy(etch_wait_t* waiter)
{
    etch_status_t status = ETCH_SUCCESS;
    apr_status_t        apr_status = APR_SUCCESS;
    apr_thread_mutex_t* apr_mutex  = NULL;
    apr_thread_cond_t*  apr_cond   = NULL;

    if(waiter == NULL) {
        return ETCH_EINVAL;
    }

    apr_mutex = waiter->mutex;
    ETCH_ASSERT(apr_mutex != NULL);
    apr_cond  = waiter->cond;
    ETCH_ASSERT(apr_cond != NULL);


    if (!is_etchobj_static_content(waiter)) {
        apr_thread_mutex_lock(g_etch_main_pool_mutex);
        apr_status = apr_thread_cond_destroy(apr_cond);
        apr_thread_mutex_unlock(g_etch_main_pool_mutex);
        if(apr_status != APR_SUCCESS) {
            char temp[1024];
            apr_strerror(apr_status, temp, sizeof(temp));
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s\n", temp);
            status = ETCH_ERROR;
        }

        apr_thread_mutex_lock(g_etch_main_pool_mutex);
        apr_status = apr_thread_mutex_destroy(apr_mutex);
        apr_thread_mutex_unlock(g_etch_main_pool_mutex);
        if(apr_status != APR_SUCCESS) {
            char temp[1024];
            apr_strerror(apr_status, temp, sizeof(temp));
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s\n", temp);
            status = ETCH_ERROR;
        }

        if(!is_etchobj_static_shell(waiter)) {
            etch_free(waiter);
        }
    }
    return status;
}

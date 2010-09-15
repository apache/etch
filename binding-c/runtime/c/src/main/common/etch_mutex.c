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
 * etch_mutex.c
 * currently wraps apr mutex. our wrapper carries with it the benefit of 
 * tracking memory for our mutex, and thus for the underlying apr mutex.
 */

#include "etch_mutex.h"
#include "etch_object.h"
#include "etch_objecttypes.h"
#include "etch_log.h"
#include "etch_mem.h"
#include "etch_thread.h"

static const char* LOG_CATEGORY = "etch_mutex";

extern apr_pool_t*         g_etch_main_pool;
extern apr_thread_mutex_t* g_etch_main_pool_mutex;

/** 
 *  etchmutex: mutex object
 */
struct etch_mutex_t
{
    etch_object object;

    void* impl;
    int32 lockcount;
};

etch_status_t etch_mutex_create(etch_mutex_t** mutex, unsigned int flags, etch_pool_t* pool)
{
    etch_status_t       status      = ETCH_SUCCESS;
    etch_mutex_t*       newmutex    = NULL;
    apr_status_t        apr_status  = APR_SUCCESS;
    apr_thread_mutex_t* apr_mutex   = NULL;

    if(mutex == NULL) {
        return ETCH_EINVAL;
    }

    if(pool == NULL) {
        pool = g_etch_main_pool;
    }

    if(flags != ETCH_MUTEX_NESTED && flags != ETCH_MUTEX_UNNESTED) {
        return ETCH_EINVAL;
    }

    newmutex = (etch_mutex_t*)new_object(sizeof(etch_mutex_t), ETCHTYPEB_MUTEX, CLASSID_MUTEX);
    ETCH_ASSERT(newmutex != NULL);

    apr_thread_mutex_lock(g_etch_main_pool_mutex);
    apr_status = apr_thread_mutex_create(&apr_mutex, flags, g_etch_main_pool);
    apr_thread_mutex_unlock(g_etch_main_pool_mutex);
    if(apr_status != APR_SUCCESS) {
        char temp[1024];
        apr_strerror(apr_status, temp, sizeof(temp));
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s\n", temp);
        etch_free(newmutex);
        *mutex = NULL;
        return ETCH_ERROR;
    }
    newmutex->impl      = apr_mutex;
    newmutex->lockcount = 0;
    *mutex = newmutex;
    return status;
}

etch_status_t etch_mutex_lock(etch_mutex_t* mutex)
{
    etch_status_t       status      = ETCH_SUCCESS;
    apr_status_t        apr_status  = APR_SUCCESS;
    apr_thread_mutex_t* apr_mutex   = NULL;

    if(mutex == NULL) {
        return ETCH_EINVAL;
    }

    apr_mutex = mutex->impl;
    ETCH_ASSERT(apr_mutex);
    apr_status = apr_thread_mutex_lock(apr_mutex);
    switch(apr_status) {
        case APR_SUCCESS:
            mutex->lockcount++;
            status = ETCH_SUCCESS;
            break;
        default: {
            char temp[1024];
            apr_strerror(apr_status, temp, sizeof(temp));
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s\n", temp);
            status = ETCH_ERROR;
            break;
        }
    }
    return status;
}

etch_status_t etch_mutex_trylock(etch_mutex_t* mutex)
{
    etch_status_t       status     = ETCH_SUCCESS;
    apr_status_t        apr_status = APR_SUCCESS;
    apr_thread_mutex_t* apr_mutex  = NULL;

    if(mutex == NULL) {
        return ETCH_EINVAL;
    }

    apr_mutex = mutex->impl;
    ETCH_ASSERT(apr_mutex);
    apr_status = apr_thread_mutex_trylock(apr_mutex);
    switch(apr_status) {
        case APR_SUCCESS:
            mutex->lockcount++;
            status = ETCH_SUCCESS;
            break;
        case APR_EBUSY:
            status = ETCH_EBUSY;
            break;
        default: {
            char temp[1024];
            apr_strerror(apr_status, temp, sizeof(temp));
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "%s\n", temp);
            status = ETCH_EBUSY;
            break;
        }
    }
    return status;
}

etch_status_t etch_mutex_unlock(etch_mutex_t* mutex)
{
    etch_status_t       status     = ETCH_SUCCESS;
    apr_status_t        apr_status = APR_SUCCESS;
    apr_thread_mutex_t* apr_mutex  = NULL;
    
    if(mutex == NULL) {
        return ETCH_EINVAL;
    }

    if(mutex->lockcount > 0) {
        mutex->lockcount--;
    }

    apr_mutex = mutex->impl;
    ETCH_ASSERT(apr_mutex);
    apr_status = apr_thread_mutex_unlock(apr_mutex);
    switch(apr_status) {
        case APR_SUCCESS:
            status = ETCH_SUCCESS;
            break;
        default: {
            char temp[1024];
            apr_strerror(apr_status, temp, sizeof(temp));
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s\n", temp);
            status = ETCH_ERROR;
            break;
        }
    }
    return status;
}

etch_status_t etch_mutex_destroy(etch_mutex_t* mutex)
{
    etch_status_t       status     = ETCH_SUCCESS;
    apr_status_t        apr_status = APR_SUCCESS;
    apr_thread_mutex_t* apr_mutex  = NULL;
    
    if(mutex == NULL) {
        return ETCH_EINVAL;
    }

    if(!is_etchobj_static_content(mutex)) {
        apr_mutex  = mutex->impl;
        apr_thread_mutex_lock(g_etch_main_pool_mutex);
        apr_status = apr_thread_mutex_destroy(apr_mutex);
        apr_thread_mutex_unlock(g_etch_main_pool_mutex);
        if(apr_status != APR_SUCCESS) {
            char temp[1024];
            apr_strerror(apr_status, temp, sizeof(temp));
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "%s\n", temp);
            status = ETCH_ERROR;
        }
    }

    if (!is_etchobj_static_shell(mutex)) {
        etch_free(mutex);
    }
    return status;
}

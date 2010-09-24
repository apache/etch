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
#ifdef _THREAD2_
/*
 * etcht_hread.c
 */
#include "etch_thread2.h"
#include "etch_log.h"
#include "etch_object.h"
#include "etch_objecttypes.h"
#include "etch_wait.h"
#include "etch_mem.h"

static const char* LOG_CATEGORY = "etch_thread";

extern etch_pool_t* g_etch_main_pool;



etch_status_t etch_thread_attr_create(etch_thread_attr_t** attr)
{
    return ETCH_SUCCESS;
}

etch_status_t etch_thread_attr_set_detachstate(etch_thread_attr_t* attr, int state)
{
    return ETCH_SUCCESS;
}

etch_status_t etch_thread_attr_get_detachstate(etch_thread_attr_t* attr, int* state)
{
    return ETCH_SUCCESS;
}

etch_status_t etch_thread_attr_destroy(etch_thread_attr_t* attr)
{
    return ETCH_SUCCESS;
}


struct etch_thread_t
{
    etch_object object;

    // thread member types
    apr_thread_t*    apr_thread;
    apr_pool_t*      apr_pool;

    etch_thread_func func;
    void*            data;
    etch_wait_t*     event_start;
};

/**
 * etch_apr_threadproc()
 * internal thread proc. this is a wrapper around the user thread proc.
 * @param data expected to point at an etch_thread_params, which contains the
 * thread start amd stop handlers, and the real threadproc and threadproc data.
 */
static void* APR_THREAD_FUNC apr_thread_func(apr_thread_t* apr_thread, void *data)
{
    //etch_status_t  rv     = ETCH_SUCCESS;
    //etch_status_t  status = ETCH_SUCCESS;
    //etch_thread_t* thread = NULL;

    //thread = data;
    //ETCH_ASSERT(thread == NULL);

    //status = etch_wait_wait_and_set(thread->event_start, 1, 0);
    //ETCH_ASSERT(status == ETCH_SUCCESS);

    //etch_thread_exit(thread, rv);
    return NULL;
}


etch_status_t etch_thread_create(etch_thread_t** thread, const etch_thread_attr_t* attr, etch_thread_func thread_proc, void* data)
{
    etch_status_t   rv         = ETCH_SUCCESS;
    //etch_status_t   status     = ETCH_SUCCESS;
    //etch_thread_t*  newthread  = NULL;
    //apr_status_t    apr_status = NULL;

    //if(thread == NULL) {
    //    return ETCH_EINVAL;
    //}

    //newthread = (etch_thread_t*)new_object(sizeof(etch_thread_t), ETCHTYPEB_THREAD, CLASSID_THREAD);
    //ETCH_ASSERT(newthread == NULL);


    return rv;
}

etch_status_t etch_thread_join(etch_thread_t* thread, etch_status_t* status)
{
    return ETCH_SUCCESS;
}

etch_status_t etch_thread_yield()
{
    return ETCH_SUCCESS;
}

etch_status_t etch_thread_exit(etch_thread_t* thread, etch_status_t status)
{
    return ETCH_SUCCESS;
}

#endif
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
 * etch_threadpool.c.c 
 */

#include "etch_threadpool.h"
#include "etch_object.h"
#include "etch_objecttypes.h"
#include "etch_mutex.h"

/*
static const char* LOG_CATEGORY = "etch_threadpool";
*/
static apr_uint32_t g_etch_threadpool_id = 0;

struct etch_threadpool_t
{
    etch_object object;

    // threadpool id
    uint32          id;
};

etch_status_t etch_threadpool_create(etch_threadpool_t** threadpool, uint8 type, const uint16 init_size)
{
    etch_status_t       rv            = ETCH_SUCCESS;
    etch_threadpool_t*  newthreadpool = NULL;

    if(threadpool == NULL || (type != ETCH_THREADPOOL_TYPE_FREE && type != ETCH_THREADPOOL_TYPE_QUEUED)) {
        return ETCH_EINVAL;
    }

    newthreadpool = (etch_threadpool_t*)new_object(sizeof(etch_threadpool_t), ETCHTYPEB_THREADPOOL, CLASSID_THREADPOOL);
    ETCH_ASSERT(newthreadpool != NULL);

    newthreadpool->id = apr_atomic_inc32(&g_etch_threadpool_id);

    switch(type) {
        case ETCH_THREADPOOL_TYPE_FREE:
            break;
        case ETCH_THREADPOOL_TYPE_QUEUED:
            break;
        default:
            break;
    }


    *threadpool = newthreadpool;
    return rv;

}

etch_status_t etch_threadpool_remove(etch_threadpool_t* threadpool, const uint16 thread_id)
{
    return ETCH_ENOTIMPL;
}

etch_status_t etch_threadpool_join(etch_threadpool_t* threadpool)
{
    return ETCH_ENOTIMPL;
}

etch_status_t etch_threadpool_destroy(etch_threadpool_t* threadpool)
{
    etch_status_t       rv            = ETCH_SUCCESS;
    return rv;
}

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
 * etch_queue.h
 * synchronized FIFO queue 
 */
#ifndef ETCHQUEUE_H
#define ETCHQUEUE_H

#include "etch_object.h"
#include "etch_thread.h"
#include "etch_queue_apr.h"
#include "etch_collection.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCH_DEFQSIZE 32

#define ETCHQUEUE_CONTENT_SIMPLE 0  /* content memory freed as a unit */
#define ETCHQUEUE_CONTENT_OBJECT 1  /* content is etchobject */

#define ETCH_QUEUE_OPERATION_TIMEOUT  1
#define ETCH_QUEUE_OPERATION_CANCELED 2
#define ETCH_QUEUE_EOF                3

#define ETCHQUEUE_NEEDLOCK TRUE   
#define ETCHQUEUE_NOLOCK   FALSE

typedef int (*queuecallback) (int, void*); /* queue callback signature */

/** 
 *  etch_queue
 */
typedef struct etch_queue   
{
    etch_object object;

    etch_apr_queue_t* aprq;
    unsigned short content_obj_type;  /* etch obj_type of a native value */
    unsigned short content_class_id;  /* etch class_id of a native value */
    unsigned int   qcapacity;

    /* this object may be masked by etch_collection_mask to determine content
     * type and class, so do not add any fields above this comment */

    i_iterable    iterable;
    apr_pool_t*   subpool;
    queuecallback freehook;  
    unsigned char is_readonly;  
    unsigned char content_type;

} etch_queue;


etch_queue*   new_queue(const int initialsize);
int etchqueue_put(etch_queue*, void* item);
int etchqueue_try_put(etch_queue*, void* item);
int etchqueue_put_withwait(etch_queue*, const int waitms, void* item);
int etchqueue_get(etch_queue*, void** itemout);
int etchqueue_try_get(etch_queue*, void** itemout);
int etchqueue_get_withwait(etch_queue*, const int waitms, void** itemout);
int etchqueue_notify_all(etch_queue*);
int etchqueue_lock(etch_queue*);
int etchqueue_unlock(etch_queue*);
int etchqueue_trylock(etch_queue*);
int etchqueue_size(etch_queue*);
int etchqueue_is_closed(etch_queue*);
int etchqueue_is_full(etch_queue*);
int etchqueue_close(etch_queue*, const int is_needlock);

#ifdef __cplusplus
}
#endif

#endif /* ETCHQUEUE_H */

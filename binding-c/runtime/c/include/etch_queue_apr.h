/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * etch_queue_apr.h
 * based on apr_queue, with timeouts added on push and pop waits.
 */

#ifndef ETCHQUEUEAPR_H
#define ETCHQUEUEAPR_H

#define ETCHQUEUE_CLEARING_CLOSED_QUEUE (-2)

#include "etch_object.h"
#include "apr_thread_cond.h"


#ifdef __cplusplus
extern "C" {
#endif

/** 
 * etch_apr_queue_t
 * same as the private apr_queue_t 
 */
typedef struct etch_apr_queue_t 
{
    void              **data;
    unsigned int        nelts; /**< # elements */
    unsigned int        in;    /**< next empty location */
    unsigned int        out;   /**< next filled location */
    unsigned int        bounds;/**< max size of queue */
    unsigned int        full_waiters;
    unsigned int        empty_waiters;
    apr_thread_mutex_t *one_big_mutex;
    apr_thread_cond_t  *not_empty;
    apr_thread_cond_t  *not_full;
    int                 terminated;

} etch_apr_queue_t;


apr_status_t etch_apr_queue_create(etch_apr_queue_t **queue, 
    unsigned int queue_capacity, apr_pool_t *a);

apr_status_t etch_apr_queue_push(etch_apr_queue_t *queue, 
    apr_interval_time_t timeout, void *data);

apr_status_t etch_apr_queue_pop(etch_apr_queue_t *queue, 
    apr_interval_time_t timeout, void **data);

apr_status_t etch_apr_queue_trypush(etch_apr_queue_t *queue, void *data);

apr_status_t etch_apr_queue_trypop(etch_apr_queue_t *queue, void **data);

unsigned int etch_apr_queue_size(etch_apr_queue_t*);

apr_status_t etch_apr_queue_interrupt_all(etch_apr_queue_t*);

apr_status_t etch_apr_queue_unsafe_interrupt_all(etch_apr_queue_t*);

apr_status_t etch_apr_queue_term(etch_apr_queue_t *queue);

apr_status_t etch_apr_queue_unsafeclose(etch_apr_queue_t*);

apr_status_t etch_apr_queue_trylock(etch_apr_queue_t*);

apr_status_t etch_apr_queue_lock(etch_apr_queue_t*);

apr_status_t etch_apr_queue_unlock(etch_apr_queue_t*);

#ifdef __cplusplus
}
#endif

#endif /* ETCHQUEUEAPR_H */

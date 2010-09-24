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
 * etch_simpletimer.h  
 * thread-per-use one-shot relative timer
 * this should be replaced with a timer pool implementation when development resources permit 
 */

#ifndef ETCH_SIMPLETIMER_H
#define ETCH_SIMPLETIMER_H

#include "apr_thread_proc.h"  /* must be included first */
#include "etch_thread.h"       /* must be included second */

#ifdef __cplusplus
extern "C" {
#endif

#define ETCH_TIMER_REASON_TIMEOUT     ETCH_TIMEOUT
#define ETCH_TIMER_REASON_INTERRUPTED ETCH_INTERRUPTED
#define ETCH_TIMER_REASON_ERROR       (-1)


typedef etch_thread etch_timer;


/**
 * etch_timer_callback 
 * signature of timer expiration callback.
 * @param passthrudata user object passed to timer constructor.
 * @param reason expiration reason 1 timeout, 0 signaled, -1 error.
 */
typedef void (*etch_timer_callback) (void* passthrudata, const int reason); 


/**
 * new_timer() 
 * etch_timer constructor
 * @param durationms timer duration in milliseconds.
 * @param passthrudata user data to be returned in expiration callback.
 * caller retains ownership of this memory.
 * @param callback expiration callback void (*callback) (void*, int);
 * in which creator's passthrudata is passed as the first parameter, and the timeout
 * reason passed in parameter 2 (reason value 0 is signaled, 1 is timeout, -1 error).
 * @return the timer object. caller owns this object but must not destroy it until 
 * after the expiration callback has completed and returned.
 */
etch_timer* new_timer(const int durationms, void* passthrudata, etch_timer_callback);


/**
 * etch_timer_start() 
 * start the specified timer. a timer must always be explicitly started.
 */
int etch_timer_start(etch_timer*);


/**
 * etch_timer_stop()
 * signal specified timer to stop waiting and exit its thread. 
 */
int etch_timer_stop (etch_timer*);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCH_SIMPLETIMER_H */

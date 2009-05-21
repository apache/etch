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
 * etch_apr.h -- common apr and service initialization and teardown
 */

#ifndef ETCHAPR_H
#define ETCHAPR_H

#include "apr_thread_proc.h"
#include "apr_time.h"
#include "apr_atomic.h"
#define INIT_ETCH_CLIENT TRUE
#define INIT_ETCH_SERVER FALSE

apr_pool_t* g_apr_pool;
const char* apr_pooltag;

typedef struct etch_runtime_exit_params
{
    unsigned char is_show_leak_detail;
    unsigned char unused_a;
    unsigned char unused_b;
    unsigned char unused_c;

} etch_runtime_exit_params;


int etch_runtime_init_all(const int is_client);
int etch_runtime_exit(etch_runtime_exit_params* args);
int check_etch_heap_bytes (const int is_show_detail, const int is_log_result);


#endif /* #ifndef ETCHPOOL_H */
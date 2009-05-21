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
 * etch_apr.c
 * common apr and service initialization and teardown
 */

#include "etch_apr.h"
#include "etch_global.h"
#include "etchthread.h"
#include "etchlog.h"

apr_pool_t* g_apr_mempool;
const char* apr_pooltag = "etchpool";
int etch_apr_setup();
int etch_apr_teardown();


/*
 * etch_runtime_init_all()
 * initialize system resources. 
 * must be called early in any client or server executable.
 * @param args reserved for future use.
 */
int etch_runtime_init_all (const int is_client)
{
    int result;
    if (0 == (result = etch_apr_setup()))
        result = etch_runtime_init(is_client);

    return result;
}


/**
 * etch_runtime_exit()
 * frees system resources. 
 * must be called at close of any client or server executable.
 * @param args etch_runtime_exit_params*, may be null.
 */
int etch_runtime_exit (etch_runtime_exit_params* args)
{
    int result = 0;
    const int is_show_leak_detail = args? args->is_show_leak_detail: FALSE;

    check_etch_heap_bytes (is_show_leak_detail, TRUE);

    etch_runtime_cleanup(0, 0);
    etch_apr_teardown();
    etchlog_close();
    return result;
}


/**
 * check_etch_heap_bytes()  
 * check how many bytes remain allocated on etch heap.
 * @param is_show_detail whether to log a line for each outsatnding allocation.
 * @params is_log_result TRUE to log a line displaying number of bytes still allocated.
 * @return number of bytes remaining allocated.
 */
int check_etch_heap_bytes (const int is_show_detail, const int is_log_result)
{
    int result = 0, leakedbytes = 0;

    #if IS_TRACKING_ETCHHEAP
    leakedbytes = etch_showmem (0, is_show_detail);
    #endif

    if (leakedbytes && is_log_result)   
        etchlog("ETCH", ETCHLOG_ERROR, "%d bytes remain allocated\n", leakedbytes);

    return leakedbytes;
}


/*
 * etch_apr_setup()
 * establish apache portable runtime environment
 */
int etch_apr_setup()
{
    int result;
    if (0 == (result = apr_initialize()))
    {   result = etch_apr_init();
        g_apr_pool = etch_apr_mempool;
    }
    if  (g_apr_pool)
         apr_pool_tag(g_apr_pool, apr_pooltag);
    else result = -1;
    return result;
}


/*
 * etch_apr_teardown()
 * free apache portable runtime environment
 */
int etch_apr_teardown()
{
    if (g_apr_pool)
        apr_pool_destroy(g_apr_pool);
    g_apr_pool = NULL;

    apr_terminate();

    return 0;
}

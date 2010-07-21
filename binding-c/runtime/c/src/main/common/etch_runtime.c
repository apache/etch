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
 * etch_runtime.h -- runtime methods.
 */

#include "etch_runtime.h"
#include "etch_cache.h"
#include "etch_default_value_factory.h"
#include "etch_mem.h"
#include "etch_mutex.h"
#include "etchinternal_single_linked_list.h"
#include "apr_pools.h"
#include "apr_thread_mutex.h"
#include "etch_encoding.h"

#define MAX_SHUTDOWN_HOOKS_SIZE 5

// global data
apr_pool_t*         g_etch_main_pool       = NULL;
apr_thread_mutex_t* g_etch_main_pool_mutex = NULL;

static etch_runtime_shutdown_hook_func shutdown_hooks[5] = 
    {NULL,NULL,NULL,NULL,NULL};

static uint16 etch_runtime_version_major    = 1;
static uint16 etch_runtime_version_minor    = 1;
static uint16 etch_runtime_version_revision = 2;

//TODO
etch_global_constants etchgc;

/**
 * etch_instantiate_global_constants()
 * instantiate global constants. these are constants which can't be statically intialized.
 * some of these may need to be freed in etch_free_global_constants in order that tests
 * can show all memory freed. any etch objects instantiated here may need to have their  
 * is_static marker set in order that they remain instantiated throughout execution.
 */
void etch_instantiate_global_constants()
{
    etchgc.etch_charsetname_us_ascii = L"us-ascii";
    etchgc.etch_charsetname_utf8     = L"utf-8";
    etchgc.etch_charsetname_utf16    = L"utf-16";
    etchgc.pctd = "%d";
}

/* runtime data */
struct etch_runtime_t
{
    uint32          count;          /* ref counting value */
    unsigned char   state;          /* state or runtime*/
    etch_config_t*  config;
    unsigned char   config_owned;
    etch_log_t*     logger;
};

/* init runtime data */
static struct etch_runtime_t etch_runtime = {
    0,
    0,
    NULL,
    0,
    NULL
};

static etch_status_t etch_statics_create()
{
    etch_instantiate_global_constants();

    return ETCH_SUCCESS;
}

int etch_runtime_mem_abort(int retcode)
{
   printf("*** APR MEMORY ERROR! Code=%d***\n", retcode);
   // sleep 1 s
   apr_sleep(1000 * 1000);
   return APR_SUCCESS;
}

etch_status_t etch_runtime_initialize(etch_config_t* config)
{
    etch_status_t  etch_status  = ETCH_SUCCESS;
    apr_status_t   apr_status   = APR_SUCCESS;

    if(etch_runtime.count++ > 0) {
        return ETCH_SUCCESS;
    }

    if(etch_runtime.state == 0) {
        // initialize apr
        apr_status = apr_initialize();
        if(apr_status != APR_SUCCESS) {
            // log error
            return -1;
        }
        // create apr memory pool
        
        apr_status = apr_pool_create(&g_etch_main_pool, NULL);
        //printf("1 creating apr pool %p\n",g_etch_main_pool);
        if(apr_status != APR_SUCCESS) {
            // log error
            // shutdown apr
            apr_terminate();
            return ETCH_ERROR;
        }
        /* set pool abort function */
        apr_pool_abort_set(etch_runtime_mem_abort, g_etch_main_pool);
        /* set apr pool tag */
        apr_pool_tag(g_etch_main_pool, "etch_apr_pool");

        apr_status = apr_thread_mutex_create(&g_etch_main_pool_mutex, APR_THREAD_MUTEX_NESTED, g_etch_main_pool);
        if(apr_status != APR_SUCCESS) {
            // log error
            // shutdown apr
            apr_terminate();
            return ETCH_ERROR;
        }

        etch_status = etch_encoding_initialize();
        if(etch_status != ETCH_SUCCESS) {
            apr_terminate();
            return ETCH_ERROR;
        }

        // set configuration
        if(config == NULL) {
            etch_status = etch_config_create(&config);
            if(etch_status != ETCH_SUCCESS) {
                // error
            }
            etch_runtime.config_owned = 1;
        }
        etch_runtime.config = config;

        // create logger
        etch_status = etch_log_create(&etch_runtime.logger, etch_runtime.config);
        if(etch_status != ETCH_SUCCESS) {
            // error opening logger
        }

        // create etch cache
        etch_status = etch_cache_create();
        if(etch_status != ETCH_SUCCESS) {
            // error opening logger
        }

        // create etch statics
        etch_status = etch_statics_create();
        if(etch_status != ETCH_SUCCESS) {
            // error opening logger
        }

        // set initialized flag
        etch_runtime.state = 1;
    }
    return ETCH_SUCCESS;
}

etch_status_t etch_runtime_get_version(uint16* major, uint16* minor, uint16* revision)
{
    etch_status_t rv = ETCH_SUCCESS;

    if(major == NULL || minor == NULL || revision == NULL) {
        return ETCH_EINVAL;
    }
    *major    = etch_runtime_version_major;
    *minor    = etch_runtime_version_minor;
    *revision = etch_runtime_version_revision;
    return rv;
}

etch_status_t etch_runtime_get_config(etch_config_t** config)
{
    if(config == NULL) {
        return ETCH_EINVAL;
    }
    *config = etch_runtime.config;
    return ETCH_SUCCESS;
}

etch_status_t etch_runtime_get_logger(etch_log_t** logger)
{
    if(logger == NULL) {
        return ETCH_EINVAL;
    }
    *logger = etch_runtime.logger;
    return ETCH_SUCCESS;
}

etch_status_t etch_runtime_shutdown()
{
    int i = 0;
    etch_status_t etch_status;

    if(etch_runtime.count > 0) {
        if(etch_runtime.count-- > 1) {
            return ETCH_SUCCESS;
        }
    }

    if(etch_runtime.state == 1) {
        
        // destroy etch statics
        for(; i < MAX_SHUTDOWN_HOOKS_SIZE; i++) {
            if(shutdown_hooks[i]){
                shutdown_hooks[i]();
            }
        }

        etchvf_free_builtins();
        
        // destroy cache
        etch_cache_destroy();
        // destroy logger
        if(etch_runtime.logger != NULL) {
            etch_log_destroy(etch_runtime.logger);
            etch_runtime.logger = NULL;
        }
        // destroy config
        if(etch_runtime.config_owned) {
            etch_config_destroy(etch_runtime.config);
            etch_runtime.config       = NULL;
            etch_runtime.config_owned = 0;
        }

        etch_status = etch_encoding_shutdown();
        if(etch_status != ETCH_SUCCESS) {
            apr_terminate();
            return ETCH_ERROR;
        }

        /* destroy apr memory pool mutex */
        apr_thread_mutex_destroy(g_etch_main_pool_mutex);

        
        /* destroy apr memory pool */
        //printf("1 destroying apr pool %p\n",g_etch_main_pool);
        apr_pool_destroy(g_etch_main_pool);
        

        g_etch_main_pool = NULL;

        /* terminate apr */
        apr_terminate();
        /* set shutdown flag */
        etch_runtime.state = 0;
    }
    return ETCH_SUCCESS;
}

etch_status_t etch_runtime_shutdown_hook_add(etch_runtime_shutdown_hook_func func)
{
    int i = 0;
    for(; i < MAX_SHUTDOWN_HOOKS_SIZE; i++)
    {
        if(shutdown_hooks[i] == NULL)
        {
            shutdown_hooks[i] = func;
            return ETCH_SUCCESS;
        }
    }
    return ETCH_ERROR;
}

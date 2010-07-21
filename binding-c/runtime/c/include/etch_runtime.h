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

#ifndef _ETCH_RUNTIME_H_
#define _ETCH_RUNTIME_H_

#include "etch.h"
#include "etch_config.h"
#include "etch_log.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * malloc'ed global constants 
 */
typedef struct etch_global_constants
{
    wchar_t* etch_charsetname_us_ascii;
    wchar_t* etch_charsetname_utf8;
    wchar_t* etch_charsetname_utf16;

    char* pctd; /* for "%d" sprintf mask */

} etch_global_constants;


// shutdown hook function
typedef etch_status_t (*etch_runtime_shutdown_hook_func)();

/**
 * etch runtime mem abort function
 */
int etch_runtime_mem_abort(int retcode);

/**
 * initialize the etch runtime components.
 */
etch_status_t etch_runtime_initialize(etch_config_t* config);

/**
 * gets the runtime version.
 */
etch_status_t etch_runtime_get_version(uint16* major, uint16* minor, uint16* revision);

/**
 * gets the runtime configuration.
 */
etch_status_t etch_runtime_get_config(etch_config_t** config);

/**
 * gets the runtime logger.
 */
etch_status_t etch_runtime_get_logger(etch_log_t** logger);

/**
 * register callback function, witch will be called when
 * the runtime shutdown.
 */
etch_status_t etch_runtime_shutdown_hook_add(etch_runtime_shutdown_hook_func);

/**
 * shutdown the etch runtime components
 */
etch_status_t etch_runtime_shutdown();

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCH_RUNTIME_H */

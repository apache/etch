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
 * xmpl_perf_client_impl.c
 * your custom implementation of base perf_client. 
 * add your implementations of messages from the server, if any, here.   
 * this code is generated in stub form, and manually augmented. 
 */

#include "apr_time.h" /* some apr must be included first */
#include "xmpl_perf_client_impl.h"
#include "etch_global.h"
#include "etch_url.h"  

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

/* generated signatures */
int destroy_perf_client_implex (perf_client_impl*);
perf_client_impl* init_perf_client_impl (perf_remote_server*, etch_destructor);


/* - - - - - - - -    
 * instantiation
 * - - - - - - - -   
 */

/**
 * new_perf_client_impl()
 * perf_client_impl constructor.
 * add your custom initialization and virtual method overrides here.
 */
perf_client_impl* new_perf_client_impl (perf_remote_server* server) 
{
    perf_client_impl* pc  /* allocate object and assign default virtuals */
        = init_perf_client_impl (server, destroy_perf_client_implex); 

    /* * *  add custom construction/initialization here  * * */
    pc->exampleobj = etch_malloc(100,0);

    /* * *  add virtual method overrides, if any, here  * * */

    return pc;
}


/**
 * destroy_perf_client_implex()
 * destructor for any user allocated memory.
 * this code is invoked by the private perf_client_impl destructor,
 * via perf_client.destroyex(). add code here to destroy any memory you  
 * may have allocated for your custom perf_client implementation.
 */
int destroy_perf_client_implex (perf_client_impl* thisx)
{
    /* * *  add custom destruction here  * * */
    etch_free(thisx->exampleobj);

    return 0;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * implementations of perf_client messages from server, if any 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */


/* - - - - - - - - - - - - - - - - - - -
 * session interface method overrides
 * - - - - - - - - - - - - - - - - - - -
 */

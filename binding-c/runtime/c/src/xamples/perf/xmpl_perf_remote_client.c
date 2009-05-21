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
 * xmpl_perf_remote_client.c
 */

#include "apr_time.h" /* some apr must be included first */
#include "xmpl_perf_remote_client.h"
#include "etch_global.h"
#include "etch_url.h" 
#include "etchlog.h" 

#include <tchar.h>
#include <stdio.h>
#include <conio.h>
char* ETCHREMC = "REMC";

/* generated signatures */
int destroy_perf_remote_client (perf_remote_client*);


/* - - - - - - - -    
 * instantiation
 * - - - - - - - -   
 */

/**
 * new_remote_client()
 * perf_remote_client constructor.
 */
perf_remote_client* new_remote_client (void* thisx, etch_session* session, etch_value_factory* vf)
{
    perf_remote* remote = NULL;
    perf_remote_client* rc = NULL;

    rc = (perf_remote_client*) new_object (sizeof(perf_remote_client), 
       ETCHTYPEB_REMOTECLIENT, get_dynamic_classid_unique(&CLASSID_PERFREMOTECLIENT));

    rc->destroy = destroy_perf_remote_client;

    /* we "implement" the service interface here since it may contain client-directed 
     * items, in which case those methods and data are exposed in this object.  we do  
     * not expose the service's server-directed methods in the remote client object.
     */
    remote = new_perf_remote (thisx, session->ds, vf, NULL);
    remote->remote_type = ETCH_REMOTETYPE_CLIENT;

    rc->perf_remote_base = remote;
    rc->perf_client_base = new_perf_client_base (thisx, remote->iperf);
    rc->session_id = session->session_id;

    /* override client-directed virtuals with implementations here */
    /* no toward-client items are declared by this service */

    return rc;
}


/**
 * destroy_perf_remote_client()
 * perf_remote_client destructor.
 */
int destroy_perf_remote_client (perf_remote_client* thisx)
{
    if (NULL == thisx) return -1;
    if (thisx->refcount > 0 && --thisx->refcount > 0) return -1;  

    if (!is_etchobj_static_content(thisx))
    {    
        ETCHOBJ_DESTROY(thisx->perf_remote_base);
        ETCHOBJ_DESTROY(thisx->perf_client_base);
    }
            
    return destroy_objectex((objmask*)thisx);
}


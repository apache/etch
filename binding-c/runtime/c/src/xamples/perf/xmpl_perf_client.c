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
 * xmpl_perf_client.c 
 */

#include "apr_time.h" /* some apr must be included first */
#include "xmpl_perf_client.h"
#include "etch_global.h"
#include "etch_url.h"  

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

int destroy_perf_client_base (i_perf_client*);


/* - - - - - - - - - - - - - -  
 * constructors
 * - - - - - - - - - - - - - -  
 */

/**
 * new_perf_client_base
 * @param iservice service interface -- caller retains   
 */
i_perf_client* new_perf_client_base (perf_client_impl* implobj)
{
    i_perf_client* ipc = (i_perf_client*) new_object (sizeof(i_perf_client), 
        ETCHTYPEB_EXECLIENTBASE, get_dynamic_classid_unique(&CLASSID_PERFCLIENTBASE));

    ipc->thisx   = implobj;  /* perf_client_impl on client, null on server */
    ipc->destroy = destroy_perf_client_base;

    ipc->iperf   = new_perf_service_interface();

    ipc->add     = ipc->iperf->add;
    ipc->sum     = ipc->iperf->sum;
    ipc->report  = ipc->iperf->report;
    ipc->dist    = ipc->iperf->dist;
    ipc->add2    = ipc->iperf->add2;
    ipc->report2 = ipc->iperf->report2;

    ipc->point   = ipc->iperf->point;

    return ipc;
}


/**
 * destroy_perf_client_base()
 * i_perf_client destructor.
 */
int destroy_perf_client_base (i_perf_client* ipc)
{
    if (NULL == ipc) return -1;
    if (ipc->refcount > 0 && --ipc->refcount > 0) return -1;  

    if (!is_etchobj_static_content(ipc))
    {    
        if (ipc->thisx)  /* thisx is null on server (i.e. this is a remote client) */
        {   /* destroy the perf_client_impl object */
            ETCH_ASSERT(is_etch_client_impl((objmask*)ipc->thisx));
            ETCHOBJ_DESTROY(((objmask*)ipc->thisx));
        }

        ETCHOBJ_DESTROY(ipc->iperf);

        etch_free(ipc->iobjsession);
    }
            
    return destroy_objectex((objmask*)ipc);
}


/* - - - - - - - - - - - - - -  
 * client base methods
 * - - - - - - - - - - - - - -  
 */

/* nothing to do - service defines no client-directed items */
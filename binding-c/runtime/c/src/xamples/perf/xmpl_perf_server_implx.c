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
 * xmpl_perf_server_implx.c 
 * perf_server_impl functions which would ordinarily not be subject to edit.
 */

#include "apr_time.h" /* some apr must be included first */
#include "xmpl_perf_server_impl.h"
#include "etch_global.h"
#include "etch_url.h"  

#include <tchar.h>
#include <stdio.h>
#include <conio.h>
int destroy_perf_server_impl (perf_server_impl*);



/* - - - - - - - - - - - - - - - - - - - - - - - -   
 * server_impl private construction / destruction 
 * - - - - - - - - - - - - - - - - - - - - - - - -    
 */

/**
 * init_perf_server_impl()   
 * called by perf_server_impl constructor to instantiate server implementation 
 * object and initialize with default virtuals.
 * @param client the remote client, not owned.
 * @param usermem_dtor destructor for any custom memory allocations.
 */
perf_server_impl* init_perf_server_impl (perf_remote_client* client, 
    etch_destructor usermem_dtor)
{
    perf_server_impl* ps = (perf_server_impl*) new_object (sizeof(perf_server_impl), 
        ETCHTYPEB_EXESERVERIMPL, CLASSID_EXESERVERBASE_IMPL);  

    ps->client  = client;  /* not owned */
    ps->destroy = destroy_perf_server_impl;  /* private destructor */
    ps->destroyex = usermem_dtor;  /* user memory destructor */

    /* instantiate base and copy virtuals to this object */
    ps->perf_server_base = new_perf_server_base (ps, NULL);  /* owned */
    ps->iperf   = ps->perf_server_base->iperf;
    ps->add     = ps->perf_server_base->add;
    ps->sum     = ps->perf_server_base->sum;
    ps->report  = ps->perf_server_base->report;
    ps->dist    = ps->perf_server_base->dist;
    ps->add2    = ps->perf_server_base->add2;
    ps->report2 = ps->perf_server_base->report2;
    ps->point   = ps->perf_server_base->point;

    ps->iobjsession = ps->perf_server_base->iobjsession;
    ps->iobjsession->thisx = ps;  /* set implementor reference */
    ps->_session_control = ps->perf_server_base->_session_control;
    ps->_session_notify  = ps->perf_server_base->_session_notify;
    ps->_session_query   = ps->perf_server_base->_session_query;

    return ps;
}


/**
 * destroy_perf_server_impl()
 * perf_server_impl private destructor.
 * calls back to user destructor to effect cleanup of any perf_server_impl 
 * memory which may have been allocated in custom code added by user.
 */
int destroy_perf_server_impl (perf_server_impl* ps)
{
    if (NULL == ps) return -1;
    if (ps->refcount > 0 && --ps->refcount > 0) return -1;  

    if (!is_etchobj_static_content(ps))
    {    
        if (ps->destroyex)     /* call back to user memory destructor */
            ps->destroyex(ps);

        if (ps->perf_server_base) /* destroy iobjession, service data */
            /* must call dtor directly since virtual dtor is destroy via base */
            destroy_perf_server_base (ps->perf_server_base);
            //ps->perf_server_base->destroy(ps->perf_server_base);
    }
            
    return destroy_objectex((objmask*)ps);
}

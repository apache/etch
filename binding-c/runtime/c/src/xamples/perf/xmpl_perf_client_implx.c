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
 * xmpl_perf_client_implx.c 
 * perf_client_impl functions which would ordinarily not be subject to edit.
 */

#include "apr_time.h" /* some apr must be included first */
#include "xmpl_perf_client_impl.h"
#include "xmpl_perf_remote_server.h"
#include "etch_global.h"
#include "etch_url.h"  

#include <tchar.h>
#include <stdio.h>
#include <conio.h>
int destroy_perf_client_impl (perf_client_impl*);



/* - - - - - - - - - - - - - - - - - - - - - - - -   
 * client_impl private construction / destruction 
 * - - - - - - - - - - - - - - - - - - - - - - - -    
 */

/**
 * init_perf_client_impl()   
 * called by perf_client_impl constructor to instantiate client implementation 
 * object and initialize with default virtuals.
 * @param server the remote server, not owned.
 * @param usermem_dtor destructor for any custom memory allocations.
 */
perf_client_impl* init_perf_client_impl (perf_remote_server* server, 
    etch_destructor usermem_dtor)
{
    perf_client_impl* pc = (perf_client_impl*) new_object (sizeof(perf_client_impl), 
        ETCHTYPEB_EXECLIENTIMPL, get_dynamic_classid_unique(&CLASSID_PERFCLIENTBASE_IMPL));  

    pc->server  = server;  /* not owned */
    pc->destroy = destroy_perf_client_impl;  /* private destructor */
    pc->destroyex = usermem_dtor;  /* user memory destructor */

    /* instantiate base and copy virtuals, if any, to this object */
    pc->perf_client_base = new_perf_client_base (pc);  /* owned */

    // pc->iobjsession = pc->perf_client_base->iobjsession; /* ORIGINALLY THIS LINE */
    pc->iobjsession = server->server_base->iobjsession; /* TRYING THIS REPLACING ABOVE */

    pc->iobjsession->thisx = pc;  /* set implementor reference */
    pc->_session_control = pc->perf_client_base->_session_control;
    pc->_session_notify  = pc->perf_client_base->_session_notify;
    pc->_session_query   = pc->perf_client_base->_session_query;

    return pc;
}


/**
 * destroy_perf_client_impl()
 * perf_client_impl private destructor.
 * calls back to user destructor to effect cleanup of any perf_client_impl 
 * memory which may have been allocated in custom code added by user.
 */
int destroy_perf_client_impl (perf_client_impl* thisx)
{
    if (NULL == thisx) return -1;
    if (thisx->refcount > 0 && --thisx->refcount > 0) return -1;  

    if (!is_etchobj_static_content(thisx))
    {    
        if (thisx->destroyex)     /* call back to user memory destructor */
            thisx->destroyex(thisx);
    }
            
    return destroy_objectex((objmask*)thisx);
}

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
 * xmpl_perf_server.c 
 */

#include "apr_time.h" /* some apr must be included first */
#include "xmpl_perf_server.h"
#include "etch_global.h"
#include "etch_url.h"  

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

int destroy_perf_server_via_base (i_perf_server*);


/* - - - - - - - - - - - - - -  
 * constructors
 * - - - - - - - - - - - - - -  
 */

/**
 * new_perf_server_base()  
 * @param implobj not interpreted
 * @param psi a perf service interface. if supplied, caller retains,
 * otherwise a service interface is instantiated and owned here. 
 */
i_perf_server* new_perf_server_base (void* implobj, i_perf* psi)
{
    i_perf_server* ips = (i_perf_server*) new_object (sizeof(i_perf_server), 
        ETCHTYPEB_EXESERVERBASE, get_dynamic_classid_unique(&CLASSID_EXESERVERBASE_PERF));

    /* the server impl is destroyed via this base object. the virtual destructor we assign
     * here will call the impl object's virtual destructor, which will directly call a 
     * non-virtual destructor for the base object. 
     */
    ips->destroy = destroy_perf_server_via_base;

    ips->thisx   = implobj; /* null passed thru from client main */

    if (psi)
        ips->iperf = psi;
    else
    {   ips->iperf = new_perf_service_interface();
        ips->is_service_interface_owned = TRUE;
    }
     
    ips->add     = ips->iperf->add;
    ips->sum     = ips->iperf->sum;
    ips->report  = ips->iperf->report;
    ips->dist    = ips->iperf->dist;
    ips->add2    = ips->iperf->add2;
    ips->report2 = ips->iperf->report2;
    ips->point   = ips->iperf->point;

    ips->iobjsession = new_default_objsession_interface (ips);
    ips->_session_control = ips->iobjsession->_session_control; 
    ips->_session_notify  = ips->iobjsession->_session_notify; 
    ips->_session_query   = ips->iobjsession->_session_query; 

    ips->server_id = ++perf_server_id_farm;

    return ips;
}


/**
 * new_perf_remote_server_base()  
 * constructor for server base when host is a remote server.
 * the server base destructor in this case destroys only itself.   
 * @param psi a perf service interface, if supplied, caller retains.
 * may be null.
 */
i_perf_server* new_perf_remote_server_base (void* implobj, i_perf* psi)
{
    i_perf_server* ips = new_perf_server_base (implobj, psi);
    ips->destroy = destroy_perf_server_base;
    return ips;
}


/**
 * destroy_perf_server_base()
 * i_perf_server destructor.
 */
int destroy_perf_server_base (i_perf_server* ips)
{
    if (NULL == ips) return -1;
    if (ips->refcount > 0 && --ips->refcount > 0) return -1;  

    if (!is_etchobj_static_content(ips))
    {    
        if (ips->is_service_interface_owned)
            ETCHOBJ_DESTROY(ips->iperf);

        etch_free(ips->iobjsession);
    }
            
    return destroy_objectex((objmask*)ips);
}


/**
 * destroy_perf_server_via_base()
 * destructor for perf_server_impl via i_perf_server.
 */
int destroy_perf_server_via_base (i_perf_server* ips)
{
    if (NULL == ips) return -1;
    if (ips->refcount > 0 && --ips->refcount > 0) return -1;  

    if (!is_etchobj_static_content(ips))
    {    
        /* serverimpl dtor will call base dtor (destroy_perf_server_base) */
        objmask* serverimpl = (objmask*) ips->thisx;
        ETCH_ASSERT(is_etch_serverimpl(serverimpl));
        ETCHOBJ_DESTROY(serverimpl);
    }

    return 0;
}


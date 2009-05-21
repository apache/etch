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
 * etch_sessionlisten.c
 * i_sessionlistener interface
 */

#include "etch_sessionlisten.h"
#include "etch_global.h"
#include "etchlog.h"

int destroy_sessionlistener(i_sessionlistener*);
int etchseslxr_def_session_accepted (void* thisx, void*);


/**
 * new_sessionlistener_interface()
 * i_sessionlistener constructor
 * @params sa server on accept callback confirming to typedef etch_session_accepted.
 * @param isession session interface virtual function overrides, 
 * caller relinquishes ownership of this memory.
 */
i_sessionlistener* new_sessionlistener_interface (void* thisx, etch_session_accepted sa, i_session* isession)  
{
    i_sessionlistener* newi = (i_sessionlistener*) new_object
        (sizeof(i_sessionlistener), ETCHTYPEB_SESSIONLXR, CLASSID_SESSIONLXR);

    newi->thisx   = thisx;
    newi->clone   = clone_null;
    newi->destroy = destroy_sessionlistener;  

    newi->session_accepted = sa? sa: etchseslxr_def_session_accepted;

    newi->isession = isession? isession: new_default_session_interface(thisx);
    newi->isession->thisx  = newi;
    newi->session_control  = newi->isession->session_control;
    newi->session_notify   = newi->isession->session_notify;
    newi->session_query    = newi->isession->session_query;
    newi->is_session_owned = TRUE;

    newi->itransport = new_default_transport_interface(thisx);
    newi->is_transport_owned = TRUE;

    return newi;
}


/*
 * destroy_sessionlistener()
 * i_sessionlistener destructor
 */
int destroy_sessionlistener(i_sessionlistener* sa)
{
    if (NULL == sa) return -1;
    if (sa->refcount > 0 && --sa->refcount > 0) return -1;  

    if (!is_etchobj_static_content(sa))
    {   
        if (sa->is_session_owned)
            etch_free(sa->isession);

        if (sa->is_transport_owned)
            etch_free(sa->itransport);

        ETCHOBJ_DESTROY(((objmask*)sa->url));
    }
            
    return destroy_objectex((objmask*)sa);
}


/*
 * etchseslxr_def_session_accepted()
 * @param socket caller retains
 */
int etchseslxr_def_session_accepted (void* thisx, void* socket)
{
    return -1;
}

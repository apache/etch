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

#include "etch_session_listener.h"
#include "etch_objecttypes.h"
#include "etch_log.h"
#include "etch_mem.h"
/*
static const char* LOG_CATEGORY = "etch_session_listener";
*/
int etchseslxr_def_session_accepted (void* thisx, void*);


/*
 * destroy_sessionlistener()
 * i_sessionlistener destructor
 */
int destroy_sessionlistener(void* data)
{
    i_sessionlistener* sa = (i_sessionlistener*)data;
    if (NULL == sa) return -1;

    if (!is_etchobj_static_content(sa))
    {   
        if (sa->is_session_owned)
            etch_free(sa->isession);

        if (sa->is_transport_owned)
            etch_free(sa->itransport);

	//        ETCHOBJ_DESTROY();
	if(((etch_object*)sa->url))
	  ((etch_object*)sa->url)->destroy(((etch_object*)sa->url));
	sa->url = NULL;

    }
            
    return destroy_objectex((etch_object*)sa);
}


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
    ((etch_object*)newi)->clone   = clone_null;
    ((etch_object*)newi)->destroy = destroy_sessionlistener;  

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
 * etchseslxr_def_session_accepted()
 * @param socket caller retains
 */
int etchseslxr_def_session_accepted (void* thisx, void* socket)
{
    return -1;
}

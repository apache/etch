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
 * etch_transportint.c
 * transport interface
 */

#include "etch_transportint.h"
#include "etch_sessionint.h"
#include "etch_mem.h"
#include "etch_session_message.h"

int etchtransport_def_transport_control    (void*, etch_event*, etch_object*);
int etchtransport_def_transport_notify     (void*, etch_event*);
etch_object* etchtransport_def_transport_query (void*, etch_query*);

i_session* etchtransport_def_get_session(void* thisx);
void  etchtransport_def_set_session(void* thisx, void* session);


/**
 * new_default_transport_interface
 * return a transport interface populated with defaults for virtuals.
 * caller owns returned object, not an etch object, use etch_free() to destroy.
 */
i_transport* new_default_transport_interface(void* thisx)
{
    i_transport* newi = etch_malloc(sizeof(i_transport), ETCHTYPEB_RAWOBJECT);

    newi->transport_control = etchtransport_def_transport_control;
    newi->transport_notify  = etchtransport_def_transport_notify;
    newi->transport_query   = etchtransport_def_transport_query;
    newi->get_session = etchtransport_def_get_session;
    newi->set_session = etchtransport_def_set_session;
    newi->thisx = thisx;
    return newi;    
}


/**
 * new_transport_interface
 * return a transport interface populated with specified virtuals.
 * caller owns returned object, not an etch object, use etch_free() to destroy.
 */
i_transport* new_transport_interface(void* thisx,
  etch_transport_control sc, etch_transport_notify sn, etch_transport_query sq)
{
    i_transport* newi = new_default_transport_interface(thisx);

    if (sc) newi->transport_control = sc;
    if (sn) newi->transport_notify  = sn;
    if (sq) newi->transport_query   = sq;
    return newi;    
}


/**
 * new_transport_interface_ex
 * return a transport interface populated with specified virtuals.
 * caller owns returned object, not an etch object, use etch_free() to destroy.
 */
i_transport* new_transport_interface_ex(void* thisx, 
  etch_transport_control sc, etch_transport_notify sn, etch_transport_query sq,
  etch_transport_get_session gs, etch_transport_set_session ss)
{   
    i_transport* newi = new_transport_interface(thisx, sc, sn, sq);

    if (gs) newi->get_session = gs;
    if (ss) newi->set_session = ss;
    return newi;    
}


/**
 * clone_transport() 
 */
i_transport* clone_transport(void* thisx, const i_transport* thattransport) 
{
    i_transport* newtransport 
        = thattransport? new_default_transport_interface(thisx): NULL;

    if (newtransport)
        memcpy(newtransport, thattransport, sizeof(i_transport));
     
    return newtransport;
}


int etchtransport_def_transport_control (void* obj, etch_event* evt, etch_object* v)
{
    return -1;
}

int etchtransport_def_transport_notify  (void* obj, etch_event* evt)
{
    return -1;
}


etch_object* etchtransport_def_transport_query (void* obj, etch_query* query) 
{
    return NULL;
}


i_session* etchtransport_def_get_session(void* thisx) 
{
    return NULL;
}


void etchtransport_def_set_session(void* thisx, void* session)
{
}

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
 * etch_sessionpkt.c
 * i_sessionpacket interface
 */

#include "etch_session_packet.h"
#include "etch_message.h"
#include "etch_flexbuffer.h"
#include "etch_objecttypes.h"
#include "etch_log.h"
#include "etch_mem.h"

/*
static const char* LOG_CATEGORY = "etch_session_package";
*/

/*
 * destroy_sessionpacket()
 * i_sessionpacket destructor
 */
int destroy_sessionpacket(void* data)
{
  i_sessionpacket* sm = (i_sessionpacket*)data;
    if (NULL == sm) return -1;

    if (!is_etchobj_static_content(sm))
    {   etch_free(sm->isession);
    }
            
    return destroy_objectex((etch_object*)sm);
}


/*
 * etchsessionpkt_def_sessionpacket()
 * default virtual implementation   
 * @param sender caller retains
 * @param buf caller retains
 */
int etchsessionpkt_def_sessionpacket (void* data, void* whoData, void* bufferData)
{
      return -1;
}


/**
 * new_sessionpkt_interface()
 * i_sessionpacket constructor
 * @param sp session_packetvirtual function overrides, 
 * @param itransport transport interface virtual function overrides, 
 * caller relinquishes ownership of this memory
 */
i_sessionpacket* new_sessionpkt_interface(void* thisx, etch_session_packet sp, i_session* isession)  
{
    i_sessionpacket* newi = (i_sessionpacket*) new_object (sizeof(i_sessionpacket), ETCHTYPEB_SESSIONPKT, CLASSID_SESSIONPKT);

    newi->thisx   = thisx;
    ((etch_object*)newi)->clone   = clone_null;
    ((etch_object*)newi)->destroy = destroy_sessionpacket;  

    newi->session_packet = sp ? sp: etchsessionpkt_def_sessionpacket;

    newi->isession = isession? isession: new_default_session_interface(thisx);
    newi->session_control = newi->isession->session_control;
    newi->session_notify  = newi->isession->session_notify;
    newi->session_query   = newi->isession->session_query;

    return newi;
}




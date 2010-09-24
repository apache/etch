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
 * etch_sessionmsg.c
 * i_sessionmessage interface
 */

#include "etch_session_message.h"
#include "etch_message.h"
#include "etch_objecttypes.h"
#include "etch_log.h"
#include "etch_mem.h"
/*
static const char* LOG_CATEGORY = "etch_session_message";
*/
/*
 * destroy_sessionmessage()
 * i_sessionmessage destructor
 */
int destroy_sessionmessage(void* data)
{
    i_sessionmessage* sm = (i_sessionmessage*)data;
    if (NULL == sm) return -1;

    if (!is_etchobj_static_content(sm))
    {   etch_free(sm->isession);
    }

    return destroy_objectex((etch_object*)sm);
}


/*
 * etch_msghandler_defmessage()
 * default virtual implementation  
 * @param sender caller retains
 * @param msg caller retains 
 * memory management rules are: if session_message handles the message, it owns
 * msg memory. otherwise, if not handled, msg is retained by the caller for 
 * redirection via session_notify destination.
 */
int etchsessionmsg_def_sessionmessage (void* sm, etch_who* sender, etch_message* msg)
{
      return -1;
}


/**
 * new_sessionmsg_interface()
 * i_sessionmessage constructor
 * @param isession session interface virtual function overrides, 
 * caller relinquishes ownership of this memory
 * @param itransport transport interface virtual function overrides, 
 * caller relinquishes ownership of this memory
 */
i_sessionmessage* new_sessionmsg_interface(void* thisx, etch_session_message sm, i_session* isession)  
{
    i_sessionmessage* newi = (i_sessionmessage*) new_object
        (sizeof(i_sessionmessage), ETCHTYPEB_SESSIONMSG, CLASSID_SESSIONMSG);

    newi->thisx   = thisx;
    ((etch_object*)newi)->clone   = clone_null;
    ((etch_object*)newi)->destroy = destroy_sessionmessage;  

    if (sm) {
      newi->session_message = sm;
    } else {
      newi->session_message = etchsessionmsg_def_sessionmessage;
    }

    newi->isession = isession? isession: new_default_session_interface(thisx);
    newi->session_control = newi->isession->session_control;
    newi->session_notify  = newi->isession->session_notify;
    newi->session_query   = newi->isession->session_query;

    return newi;
}


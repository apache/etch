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
 * etch_sessiondata.c
 * i_sessiondata interface
 */

#include "etch_sessiondata.h"
#include "etchflexbuf.h"
#include "etch_global.h"
#include "etchlog.h"

int destroy_sessiondata(i_sessiondata*);
int etchsessiondata_def_sessiondata (void* thisx, etch_who*, etch_flexbuffer*);


/**
 * new_sessiondata_interface()
 * i_sessiondata constructor
 * @param sm i_sessiondata::session_data() virtual function override. 
 * caller relinquishes ownership of this memory
 * @param isession session interface virtual function overrides. 
 * caller relinquishes ownership of this memory
 */
i_sessiondata* new_sessiondata_interface(void* thisx, etch_session_data sm, i_session* isession)  
{
    i_sessiondata* newi = (i_sessiondata*) new_object
        (sizeof(i_sessiondata), ETCHTYPEB_SESSIONDATA, CLASSID_SESSIONDATA);

    newi->thisx   = thisx;
    newi->clone   = clone_null;
    newi->destroy = destroy_sessiondata;  

    newi->session_data = sm? sm: etchsessiondata_def_sessiondata;

    newi->isession = isession? isession: new_default_session_interface(thisx);
    newi->session_control = newi->isession->session_control;
    newi->session_notify  = newi->isession->session_notify;
    newi->session_query   = newi->isession->session_query;

    return newi;
}


/*
 * destroy_sessiondata()
 * i_sessiondata destructor
 */
int destroy_sessiondata(i_sessiondata* sm)
{
    if (NULL == sm) return -1;
    if (sm->refcount > 0 && --sm->refcount > 0) return -1;  

    if (!is_etchobj_static_content(sm))
    {   etch_free(sm->isession);
    }
            
    return destroy_objectex((objmask*)sm);
}


/*
 * etch_msghandler_defmessage()
 * default virtual implementation   
 * @param sender caller retains
 * @param buf caller retains
 */
int etchsessiondata_def_sessiondata (i_sessiondata* sm, etch_who* sender, etch_flexbuffer* buf)
{
      return -1;
}

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
 * etch_sessiondata.h
 * i_sessiondata interface
 * interface used to deliver packets to the session from the transport
 */
#ifndef ETCHISESSIONDATA_H
#define ETCHISESSIONDATA_H

#if(0)

 SESSIONDATA
 |  int sessionData(Who from, buffer)  
  - SESSION
       sessionQuery(); sessionControl(); sessionNotify();
 
#endif


#include "etch_sessionint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*etch_session_data) (void* thisx, void* whofrom, void* buffer);


/**
 * i_sessiondata
 * sessiondata interface
 * a message handler delivers messages from a message source
 */
typedef struct i_sessiondata
{
    etch_object object;

    void* thisx; /* object which is the interface implementor */

    /* session interface */
    i_session* isession;
    etch_session_control session_control;
    etch_session_notify  session_notify;
    etch_session_query   session_query;

    /**
     * session_data()
     * delivers packets to the session from the transport
     * @param from whofrom who sent the message.
     * caller relinquishes this object on success, retains on failure.
     * @param buf the packet buffer from the packet source. 
     * caller relinquishes this object on success, retains on failure.
     * @return 0 success, -1 error
     */
    etch_session_data session_data;

} i_sessiondata;


i_sessiondata* new_sessiondata_interface(void* thisx, etch_session_data, i_session*); 

#ifdef __cplusplus
}
#endif

#endif /* ETCHISESSIONDATA_H */

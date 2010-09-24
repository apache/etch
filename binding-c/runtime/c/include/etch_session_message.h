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
 * etch_sessionmsg.h
 * i_sessionmessage interface
 * interface used to deliver messages to the session from the transport
 */
#ifndef ETCHISESSIONMSG_H
#define ETCHISESSIONMSG_H

#if(0)

 SESSIONMESSAGE
 |  int sessionMessage(Who from, message)  
  - SESSION
       sessionQuery(); sessionControl(); sessionNotify();
 
#endif


#include "etch_sessionint.h"
#include "etch_object.h"
#include "etch_message.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*etch_session_message) (void* , etch_who* , etch_message* );


/**
 * i_sessionmessage
 * sessionmessage interface
 * a message handler delivers messages from a message source
 */
typedef struct i_sessionmessage
{
    etch_object object;

    void* thisx; /* object which is the interface implementor */

    /* session interface */
    i_session* isession;
    etch_session_control session_control;
    etch_session_notify  session_notify;
    etch_session_query   session_query;

   /**
     * session_message()
     * delivers data to the session from the transport
     * @param from whofrom who sent the message.
     * caller relinquishes this object on success, retains on failure.
     * @param message the message from the message source. 
     * caller relinquishes this object on success, retains on failure.
     * @return 0 success, -1 error
     */
    etch_session_message session_message;

} i_sessionmessage;

i_sessionmessage* new_sessionmsg_interface(void* thisx, etch_session_message sm, i_session* session); 
 
#ifdef __cplusplus
}
#endif

#endif /* ETCHISESSIONMSG_H */

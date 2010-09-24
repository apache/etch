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
 * etch_transportmsg.h
 * i_transportmessage interface
 * interface used to deliver messages to the transport from the transport
 */

#ifndef ETCHITRANSPORTMSG_H
#define ETCHITRANSPORTMSG_H

#if(0)

 TRANSPORTMESSAGE
 |  void transportMessage(Who to, message)  
  - TRANSPORT<SessionMessage>
      transportQuery(); transportl(); transport();
      getSession(); setSession();
 
#endif

#include "etch_transportint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*etch_transport_message) (void* thisx, void* whofrom, void* message);


/**
 * i_transportmessage
 * transportmessage interface
 * a message handler delivers messages from a message source
 */
typedef struct i_transportmessage
{
    etch_object object;

    void* thisx; /* object which is the interface implementor */

    /* transport interface */
    i_transport* itransport;
    etch_transport_control transport_control;  
    etch_transport_notify  transport_notify;   
    etch_transport_query   transport_query;   
    etch_transport_get_session  get_session;   
    etch_transport_set_session  set_session;  

   /**
     * transport_message()
     * delivers data to transport from ...
     * @param to recipient
     * @param message the message  
     * @return 0 success, -1 error
     */
    etch_transport_message transport_message;

} i_transportmessage;


i_transportmessage* new_transportmsg_interface(void* thisx, etch_transport_message, i_transport*); 

#ifdef __cplusplus
}
#endif

#endif /* ETCHITRANSPORTMSG_H */

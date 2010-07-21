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
 * etch_sessionpkt.h
 * i_sessionpacket interface
 * interface used to deliver packets to the session from the transport
 */
#ifndef ETCHISESSIONPACKET_H
#define ETCHISESSIONPACKET_H

#if(0)

 SESSIONPACKET
 |  int sessionPacket(Who from, packet)  
  - SESSION
       sessionQuery(); sessionControl(); sessionNotify();
 
#endif


#include "etch_sessionint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*etch_session_packet) (void* thisx, void* whofrom, void* buffer);


/**
 * i_sessionpacket
 * sessionpacket interface
 */
typedef struct i_sessionpacket
{
    etch_object object;

    void* thisx; /* object which is the interface implementor */

   /* session interface */
    i_session* isession;
    etch_session_control session_control;
    etch_session_notify  session_notify;
    etch_session_query   session_query;

    /**
     * session_packet()
     * delivers data to the session from the transport.
     * @param from whofrom who sent the packet
     * @param packet the packet from the packet source 
     * @return 0 OK, -1 buffer contents invalid.
     */
    etch_session_packet  session_packet;


} i_sessionpacket;


i_sessionpacket* new_sessionpkt_interface(void* thisx, etch_session_packet, i_session*); 

#ifdef __cplusplus
}
#endif

#endif /* ETCHISESSIONPACKET_H */

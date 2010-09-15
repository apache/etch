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
 * etch_msgizer.h
 * messagizer accepts packets and translates them to messages,
 * and it accepts messages and translates them to packets.
 */

#ifndef ETCHMSGIZER_H
#define ETCHMSGIZER_H

#include "apr_thread_proc.h"
#include "etch_session_message.h"
#include "etch_session_packet.h"
#include "etch_transport_message.h"
#include "etch_transport_packet.h"

#include "etch_tagdata_inp.h"
#include "etch_tagdata_out.h"

#include "etch_flexbuffer.h"
#include "etch_resources.h"  
#include "etch_mutex.h"
#include "etch_url.h"

#ifdef __cplusplus
extern "C" {
#endif

#if(0)

 MESSAGIZER(TRANSPORTPACKET)
 |  tagdata_input*  tdi;
 |  tagdata_output* tdo;
 |  transportPacket* transport;
 |  sessionPacket(from, buf);      // override  i_sessionpacket
 |  transportMessage(to, Message); // implement i_transportmsg
  - SESSIONPACKET
 |  |  int (*etch_session_packet) (void* thisx, void* whofrom, void* buffer);
 |   - SESSION
 |        sessionQuery(); sessionControl(); sessionNotify();
  - TRANSPORTMESSAGE  
    |  int transportMessage(to, Message);
     - TRANSPORT
          transportQuery(); transportControl(); transportNotify();
#endif


/*
 * etch_messagizer
 */
typedef struct etch_messagizer
{
    etch_object object;

    /* i_transportpacket of next lower layer (packetizer) */
    i_transportpacket*   transport; /* not owned */

    /* i_sessionmessage of next higher layer (mailbox manager) */
    i_sessionmessage*    session;   /* not owned */

    etch_flexbuffer*     msgbuf;        /* owned */
    tagged_data_input*   tdi;           /* owned */
    tagged_data_output*  tdo;           /* owned */

    /* - - - - - - - - - - - - - - -
     * i_transportmessage
     * - - - - - - - - - - - - - - -
     */

    /**
     * i_transportmessage::transport_message()
     * delivers message to transport from session
     * @param to recipient
     * @return 0 success, -1 error
     */
    etch_transport_message transport_message;
    i_transportmessage*    transportmsg;      /* owned */
    etch_transport_control transport_control; /* i_transportmessage::itransport */
    etch_transport_notify  transport_notify;  /* i_transportmessage::itransport */
    etch_transport_query   transport_query;   /* i_transportmessage::itransport */
    etch_transport_get_session  get_session;  /* i_transportmessage::itransport */
    etch_transport_set_session  set_session;  /* i_transportmessage::itransport */

    /* - - - - - - - - - - - - - - -
     * i_sessionpacket
     * - - - - - - - - - - - - - - -
     */

    /**
     * i_sessionpacket::session_packet()
     * delivers data to the session from the transport
     * @param from from who sent the packet
     * @param buffer the packet from the packet source 
     * @return 0 success, -1 error
     */    
    etch_session_packet  session_packet;
    i_sessionpacket*     sessionpkt;       /* owned */
    etch_session_control session_control;  /* i_sessionpacket::isession */
    etch_session_notify  session_notify;   /* i_sessionpacket::isession */
    etch_session_query   session_query;    /* i_sessionpacket::isession */

    etch_mutex* msglock;

} etch_messagizer;


etch_messagizer* new_messagizer  (i_transportpacket*, wchar_t* uri, etch_resources* resxmap);
etch_messagizer* new_messagizer_a(i_transportpacket*, etch_url*, etch_resources*);
i_session* etch_msgizer_get_session(void*); 
void etch_msgizer_set_session(void*, void*);

#ifdef __cplusplus
}
#endif

#endif  /* ETCHMSGIZER_H */

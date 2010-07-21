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
 * etch_transportpkt.h
 * i_transportpacket interface
 * interface used to deliver packets to transport from session
 */

#ifndef ETCHITRANSPORTPACKET_H
#define ETCHITRANSPORTPACKET_H


#if(0)

 TRANSPORTPACKET
 |  int transportPacket(Who from, packetbuffer)  
  - TRANSPORT
       transportQuery(); transportControl(); transportNotify();
 
#endif


#include "etch_transportint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*etch_transport_packet) (void* thisx, void* whoto, void* buffer);
typedef int (*etch_transport_packet_headersize) (void* thisx);


/**
 * i_transportpacket
 * transportpacket interface
 */
typedef struct i_transportpacket
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
     * transport_packet()
     * delivers packet to transport after adding packet header.
     * @param to recipient
     * @param buffer buffer positioned at the packet data, with space for header
     * @return 0 success, -1 error
     */
    etch_transport_packet  transport_packet; 

    /**
     * etch_transport_packet_headersize()
     * @return size of packet header in bytes
     */
    etch_transport_packet_headersize  get_headersize;

    int   header_size;

} i_transportpacket;


i_transportpacket* new_transportpkt_interface(void* thisx, etch_transport_packet, i_transport*);

#ifdef __cplusplus
}
#endif

#endif /* ETCHITRANSPORTPACKET_H */

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
 * etch_packetizer.h
 * packetizes a stream data source. Reads a packet header: 32-bit flag 
 * and 32-bit length, both big-endian, verifies the flag, using length from
 * header, reads packet data and forwards it to the packet handler.
 * as a packet source, accepts a packet and prepends a packet header to it 
 * prior to delivering it to a data source.
 */

#ifndef ETCHPACKETIZER_H
#define ETCHPACKETIZER_H

#include "apr_thread_proc.h"
#include "etch_sessiondata.h"
#include "etch_transportpkt.h"
#include "etch_transportdata.h"
#include "etch_sessionpkt.h"
#include "etch_arraylist.h"
#include "etch_resources.h"
#include "etchflexbuf.h"
#include "etchthread.h"
#include "etch_url.h"
#define ETCHPZR_HAS_MUTEX FALSE

#if(0)

 PACKETIZER(TRANSPORTDATA)
 |  transportData* transport;
 |  sessionData(from, buf);      // override  i_sessiondata
 |  transportPacket(to, buf);    // implement i_transportpacket
  - SESSIONDATA
 |  |  int sessionData (whofrom, buffer);
 |   - SESSION
 |        sessionQuery(); sessionControl(); sessionNotify();
  - TRANSPORTPACKET  
    |  int transportPacket(to, buffer);
    |  int headerSize;
     - TRANSPORT
          transportQuery(); transportControl(); transportNotify();
#endif

const wchar_t* ETCH_PKTIZER_MAX_PKT_SIZE_TERM;
const int ETCH_PKTIZER_DEFMAXPKTSIZE;
const int ETCH_PKTIZER_HEADERSIZE;
const int ETCH_PKTIZER_SIG;

typedef struct etch_packetizer  etch_packetizer;


/*
 * etch_packetizer
 */
typedef struct etch_packetizer
{
    unsigned int    hashkey;  
    unsigned short  obj_type; 
    unsigned short  class_id;
    struct objmask* vtab;  
    int  (*destroy)(void*);
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;
    struct objmask* parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;

    /* transport of next lower layer of the stack (connection) */
    i_transportdata*   transport;   /* not owned */

   /* session of next higher layer of the stack (messagizer) */
    i_sessionpacket*   session;     /* not owned */

    /* - - - - - - - - - - - - - - -
     * i_transportpacket
     * - - - - - - - - - - - - - - -
     */

    /**
     * i_transportpacket::transport_packet()
     * delivers packet to the transport after adding packet header.
     * @param to recipient
     * @param buf flexbuffer positioned on the packet and including space for header.
     * @return 0 success, -1 error
     */
    etch_transport_packet  transport_packet;  /* transport_packet() */
    i_transportpacket* transportpkt;          /* owned */
    etch_transport_control transport_control; /* i_transportmessage::itransport */
    etch_transport_notify  transport_notify;  /* i_transportmessage::itransport */
    etch_transport_query   transport_query;   /* i_transportmessage::itransport */
    etch_transport_get_session  get_session;  /* i_transportmessage::itransport */
    etch_transport_set_session  set_session;  /* i_transportmessage::itransport */

    /* - - - - - - - - - - - - - - -
     * i_sessiondata
     * - - - - - - - - - - - - - - -
     */

    /**
     * i_sessiondata::session_data()
     * delivers data to the session from the transport
     * @param from from who sent the packet data
     * @param buffer the packet from the packet source positioned at the data
     * @return 0 success, -1 error
     */ 
    etch_session_data    session_data;     /* session_data() */  
    i_sessiondata*       sessiondata;      /* owned */ 
    etch_session_control session_control;  /* i_sessionpacket::isession */
    etch_session_notify  session_notify;   /* i_sessionpacket::isession */
    etch_session_query   session_query;    /* i_sessionpacket::isession */

    int (*process_header) (etch_packetizer*, etch_flexbuffer*, const int is_reset);

    etchmutex* datalock;

    etch_flexbuffer* savebuf;  /* owned */

    size_t headersize;
    size_t bodylength;
    size_t maxpacketsize;
    unsigned char is_wantheader; /* logic state flag: set true in ctor */

} etch_packetizer;


etch_packetizer* new_packetizer  (i_transportdata*, wchar_t* uri, etch_resources*);
etch_packetizer* new_packetizer_a(i_transportdata*, etch_url*, etch_resources*);

i_sessionpacket*  etch_pktizer_get_session(etch_packetizer*); 
void etch_pktizer_set_session(etch_packetizer*, i_sessionpacket*);
int  etch_pktizer_process_header (etch_packetizer*, etch_flexbuffer*, const int);


#endif  /* ETCHPACKETIZER_H */
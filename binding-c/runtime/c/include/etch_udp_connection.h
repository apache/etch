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
 * etch_connection.h
 * connection client and server classes - udp
 */

#ifndef _ETCH_UDP_CONNECTION_H_
#define _ETCH_UDP_CONNECTION_H_

#include "apr_thread_proc.h"
#include "etch_connection.h"
#include "etch_transport_packet.h"
#include "etch_transportint.h"
#include "etch_session_packet.h"
#include "etch_url.h"

#ifdef __cplusplus
extern "C" {
#endif

struct etch_udp_client;

/*
 * udp specific connection type
 */
typedef struct etch_udp_connection
{
    ETCH_TRANSPORT_CONNECTION_COMMON_TYPES;

    struct etch_udp_client* rcvlxr;  /* owned */
    i_transportpacket* itp;     /* owned */
    i_sessionpacket*   session; /* not owned */

    boolean is_server;
    apr_sockaddr_t *remote_addr;

    unsigned char is_reuseaddr;
    unsigned char is_broadcast;
    unsigned char is_session_owned;

} etch_udp_connection;


/**
 * etch_udp_client 
 * udp client listener class
 */
typedef struct etch_udp_client
{
    ETCH_CLIENT_COMMON_TYPES;
 
    etch_udp_connection* cxlisten; /* not owned */
} etch_udp_client;


//extern unsigned connection_id_farm;

int  is_good_udp_params(etch_url*, void* resources, etch_rawsocket*);
etch_udp_connection* new_udp_connection(etch_url*, void* resources, etch_rawsocket*);
int  init_etch_udpconx_interfaces (etch_udp_connection*);
int  new_udpsocket (apr_socket_t**, apr_pool_t*);
int  destroy_etch_udp_connection(void* thisx);
int  etch_udpconx_start(etch_udp_connection*);
int  etch_udpclient_start_listener (etch_udp_connection*);
int  etch_udpconx_open (etch_udp_connection*, const int is_reconnect);
int  etch_udpconx_close(etch_udp_connection*);
int  etch_udpclient_send(etch_udp_connection *conx, etch_who* whoto, unsigned char* buf, const size_t totallen, int* rc);
int  etch_udpclient_sendex (etch_udp_connection *conx, etch_who* whoto, unsigned char* buf, const size_t totallen, const int timeout_ms, int* rc);
int  etch_udpclient_receive (etch_udp_connection*, unsigned char*, const size_t, int*);
int  etch_udpclient_receivex(etch_udp_connection*, unsigned char*, const size_t, const int, int*);
int  etch_udpclient_stop_listener (etch_udp_connection*);


#ifdef __cplusplus
}
#endif

#endif  /* _ETCH_UDP_CONNECTION_H_ */

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
 * connection client and server classes - tcp, udp
 */

#ifndef ETCHTCPSERVER_H
#define ETCHTCPSERVER_H

#include "apr_thread_proc.h"
#include "etch_tcp_connection.h"
#include "etch_resources.h"
#include "etch_session_listener.h"
#include "etch_mutex.h"
#include "etch_linked_list.h"

#ifdef __cplusplus
extern "C" {
#endif

struct etch_tcp_server;
typedef int (*etch_listener_event_handler) (struct etch_tcp_server*, struct etch_tcp_connection*, const int, int, void*);

#define ETCH_TCPSERVER_STATE_CLOSED   0
#define ETCH_TCPSERVER_STATE_CLOSING  1
#define ETCH_TCPSERVER_STATE_STOPPED  2
#define ETCH_TCPSERVER_STATE_STOPPING 3
#define ETCH_TCPSERVER_STATE_STARTING 4
#define ETCH_TCPSERVER_STATE_STARTED  5


/**
 * etch_tcp_server 
 * tcp listener class
 */
typedef struct etch_tcp_server
{
    etch_object object;

    etch_tcp_connection* cxlisten; /* owned */
    etch_threadpool* threadpool;   /* not owned */
    etch_threadpool* subpool;      /* not owned */
    etch_listener_event_handler   on_event;
    etch_connection_event_handler on_data;
    
    /* - - - - - - - - - - - - - - - - - -
     * i_sessionlistener - set externally
     * - - - - - - - - - - - - - - - - - -
     */
    i_session*            isession;  /* not owned */
    i_sessionlistener*    session;   /* owned or not */
    etch_session_control  session_control;
    etch_session_notify   session_notify;
    etch_session_query    session_query; 
    etch_session_accepted on_session_accepted;

    /* - - - - - - - - - - - - - - - - - -
     * i_transport
     * - - - - - - - - - - - - - - - - - -
     */
    i_transport* itransport;  /* owned */
    etch_transport_control transport_control;
    etch_transport_notify  transport_notify;
    etch_transport_query   transport_query;
    etch_transport_get_session get_session;
    etch_transport_set_session set_session;

    /* - - - - - - - - - - - - - - - - - 
     * instance data 
     * - - - - - - - - - - - - - - - - - 
     */
    unsigned listener_id;
    int  backlog;
    int  connections;
    etch_url* url;
    etch_wait_t* waiter;     /* not owned */
    etch_resources* resxmap; /* not owned */

    unsigned char state;
    unsigned char is_started;
    unsigned char is_session_owned;
    unsigned char is_threadpool_owned;

    etch_linked_list_t* client_connections;
    etch_mutex*         client_connections_mutex;
    etch_thread*        thread;

} etch_tcp_server;

unsigned tcpserver_id_farm;

etch_tcp_server* new_tcp_server(etch_url*, etch_threadpool*, etch_threadpool*, etch_resources*, i_sessionlistener*);
int  etch_tcpsvr_open (etch_tcp_server*, const int is_reconnect);
int  etch_tcpsvr_start(etch_tcp_server*);
int  etch_tcpsvr_stop (etch_tcp_server*);
int  etch_tcpsvr_close(etch_tcp_server*);
int  etch_deftcplistener_on_event(etch_tcp_server*, etch_tcp_connection*, const int e, int p1, void* p2);

#ifdef __cplusplus
}
#endif

#endif  /* ETCHTCPSERVER_H */

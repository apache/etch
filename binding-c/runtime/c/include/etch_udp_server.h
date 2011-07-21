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
 * etch_udp_server.h
 */

#ifndef ETCHUDPSERVER_H
#define ETCHUDPSERVER_H

#include "apr_thread_proc.h"
#include "etch_udp_connection.h"
#include "etch_resources.h"
#include "etch_session_listener.h"
#include "etch_mutex.h"
#include "etch_linked_list.h"
#include "etch_server.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCH_UDPSERVER_STATE_CLOSED   0
#define ETCH_UDPSERVER_STATE_CLOSING  1
#define ETCH_UDPSERVER_STATE_STOPPED  2
#define ETCH_UDPSERVER_STATE_STOPPING 3
#define ETCH_UDPSERVER_STATE_STARTING 4
#define ETCH_UDPSERVER_STATE_STARTED  5

/**
 * etch_udp_server 
 * udp listener class
 */
typedef struct etch_udp_server
{
    ETCH_SERVER_COMMON_TYPES;

    etch_udp_connection* cxlisten; /* owned */
} etch_udp_server;

unsigned udpserver_id_farm;

etch_udp_server* new_udp_server(etch_url*, etch_threadpool*, etch_threadpool*, etch_resources*, i_sessionlistener*);
int  etch_udpsvr_open (etch_udp_server*, const int is_reconnect);
int  etch_udpsvr_start(etch_udp_server*);
int  etch_udpsvr_stop (etch_udp_server*);
int  etch_udpsvr_close(etch_udp_server*);

#ifdef __cplusplus
}
#endif

#endif  /* ETCHUDPSERVER_H */

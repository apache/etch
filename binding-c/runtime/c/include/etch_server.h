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
 * etch_server.h
 */

#ifndef ETCHSERVER_H
#define ETCHSERVER_H

#include "etch_session_listener.h"
#include "etch_linked_list.h"

#ifdef __cplusplus
extern "C" {
#endif

struct etch_server;
typedef int (*etch_listener_event_handler) (struct etch_server*, struct etch_transport_connection*, const int, int, void*);
typedef struct etch_server *(*new_server_func) (etch_url* url, etch_threadpool* mainpool, etch_threadpool* subpool, etch_resources* resxmap, i_sessionlistener* insession);

/**
 * etch_server
 */
#define ETCH_SERVER_COMMON_TYPES \
    etch_object object;                                      \
                                                             \
    etch_threadpool* threadpool;   /* not owned */           \
    etch_threadpool* subpool;      /* not owned */           \
    etch_listener_event_handler   on_event;                  \
    etch_connection_event_handler on_data;                   \
                                                             \
    /* - - - - - - - - - - - - - - - - - - */                \
    /* i_sessionlistener - set externally  */                \
    /* - - - - - - - - - - - - - - - - - - */                \
    i_session*            isession;  /* not owned */         \
    i_sessionlistener*    session;   /* owned or not */      \
    etch_session_control  session_control;                   \
    etch_session_notify   session_notify;                    \
    etch_session_query    session_query;                     \
    etch_session_accepted on_session_accepted;               \
                                                             \
    /* - - - - - - - - - - - - - - - - - - */                \
    /* i_transport                         */                \
    /* - - - - - - - - - - - - - - - - - - */                \
    i_transport* itransport;  /* owned */                    \
    etch_transport_control transport_control;                \
    etch_transport_notify  transport_notify;                 \
    etch_transport_query   transport_query;                  \
    etch_transport_get_session get_session;                  \
    etch_transport_set_session set_session;                  \
                                                             \
    /* - - - - - - - - - - - - - - - - - */                  \
    /* instance data                     */                  \
    /* - - - - - - - - - - - - - - - - - */                  \
    unsigned listener_id;                                    \
    int  connections;                                        \
    etch_url* url;                                           \
    etch_wait_t* waiter;     /* not owned */                 \
    etch_resources* resxmap; /* not owned */                 \
                                                             \
    unsigned char state;                                     \
    unsigned char is_started;                                \
    unsigned char is_session_owned;                          \
    unsigned char is_threadpool_owned;                       \
                                                             \
    etch_linked_list_t* client_connections;                  \
    etch_mutex*         client_connections_mutex;            \
    etch_thread*        thread

typedef struct etch_server
{
   ETCH_SERVER_COMMON_TYPES;

   etch_transport_connection* cxlisten;
} etch_server;

int destroy_etch_server (etch_server* svr);
void etch_server_set_session(void* data, void* sessionListener);
int etch_server_stub_session_control (void* obj, etch_event* evt, etch_object* v);
int etch_server_stub_session_notify  (void* obj, etch_event* event);
int etch_server_stub_on_session_accepted(void* thisx, void* socket);
etch_object* etch_server_stub_session_query (void* obj, etch_query* query);

#ifdef __cplusplus
}
#endif

#endif  /* ETCHTCPSERVER_H */

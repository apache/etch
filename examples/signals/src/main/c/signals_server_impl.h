/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */


#ifndef SIGNALS_SERVER_IMPL_H
#define SIGNALS_SERVER_IMPL_H

#include "signals_server.h"
#include "etch_transport.h"

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned short CLASSID_SIGNALS_SERVER_IMPL;

//typedef struct signals_remote_client signals_remote_client;

/**
 * signals_server_impl
 */
typedef struct signals_server_impl
{
    etch_object object;  

    i_signals_server* signals_server_base; /* owned */
    i_signals* isignals; /* not owned */
    struct signals_remote_client* client; /* not owned */

    int (*destroyex) (void*);  /* user memory destructor */

    /* - - - - - - - - - - - -
     * objsession
     * - - - - - - - - - - - -
     */
    i_objsession* iobjsession;  /* owned by base */
    /* note that iobjsession->thisx is set to this signals_server_impl* */
    etch_session_control _session_control;
    etch_session_notify  _session_notify;
    etch_session_query   _session_query;

} signals_server_impl;

/* constructor */
signals_server_impl* new_signals_server_impl (struct signals_remote_client*);

#ifdef __cplusplus
} //extern "C"
#endif

#endif /* SIGNALS_SERVER_IMPL_H */

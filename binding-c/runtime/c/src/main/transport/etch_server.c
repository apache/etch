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
 * etch_server.c
 */

#include "etch_thread.h"
#include "etch_url.h"
#include "etch_resources.h"
#include "etch_connection_event.h"
#include "etch_server.h"
#include "etch_objecttypes.h"

int destroy_etch_server (etch_server* svr)
{
    int result = 0;
    if (NULL == svr) return -1;

    if (svr->session && svr->is_session_owned)
       ((etch_object*)svr->session)->destroy (svr->session);

    if (svr->itransport)
        etch_free(svr->itransport);

    if (svr->threadpool && svr->is_threadpool_owned)
        etch_object_destroy(svr->threadpool);

    etch_object_destroy(svr->thread);
    svr->thread = NULL;

    etch_mutex_destroy(svr->client_connections_mutex);
    svr->client_connections_mutex = NULL;

    etch_linked_list_destroy(svr->client_connections);
    svr->client_connections = NULL;

    svr->on_event((etch_server *)svr, 0, ETCH_CONXEVT_DESTROYED, 0, 0);

    result = destroy_objectex((etch_object*)svr);
    return result;
}

/**
 * etch_svr_set_session()
 * i_transport::set_session() override
 * @param session an i_sessionlistener*. caller owns this object.
 */
void etch_server_set_session(void* data, void* sessionListener)
{
    etch_server* thisx = (etch_server*)data;
    i_sessionlistener* session = (i_sessionlistener*)sessionListener;
    ETCH_ASSERT(is_etch_udpserver(thisx) || is_etch_tcpserver(thisx));
    ETCH_ASSERT(is_etch_sessionlxr(session));

    thisx->is_session_owned = FALSE;  /* internal caller will re-set */

    if (thisx->session)  /* replacing? */
    {   ETCH_ASSERT(is_etch_sessionlxr(thisx->session));
        etch_object_destroy(thisx->session);
    }

    thisx->session  = session;
    thisx->isession = session->isession;
    thisx->session_control = session->session_control;
    thisx->session_notify  = session->session_notify;
    thisx->session_query   = session->session_query;
    thisx->on_session_accepted = session->session_accepted;    
}

int etch_server_stub_session_control (void* obj, etch_event* evt, etch_object* v)
{
    return -1;
}

int etch_server_stub_session_notify  (void* obj, etch_event* event)
{
    return -1;
}

int etch_server_stub_on_session_accepted(void* thisx, void* socket)
{

    return -1;
}

etch_object* etch_server_stub_session_query (void* obj, etch_query* query) 
{
    return NULL;
}
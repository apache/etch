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
 * etch_connection.c
 * connection client and server classes - tcp, udp
 */
#include "etch.h"
#include "etch_runtime.h"
#include "etch_thread.h"
#include "etch_connection.h"
#include "etch_encoding.h"
#include "etch_flexbuffer.h"
#include "etch_objecttypes.h"
#include "etch_log.h"
#include "etch_mem.h"

static const char* LOG_CATEGORY = "etch_connection";

// extern types
extern etch_pool_t*        g_etch_main_pool;
extern apr_thread_mutex_t* g_etch_main_pool_mutex;

const wchar_t* ETCH_CONNECTION_RECONDELAY   = L"TcpConnection.reconnect_delay";
const wchar_t* ETCH_CONNECTION_AUTOFLUSH    = L"TcpConnection.autoFlush";
const wchar_t* ETCH_CONNECTION_KEEPALIVE    = L"TcpConnection.keepAlive";
const wchar_t* ETCH_CONNECTION_LINGERTIME   = L"TcpConnection.lingerTime";
const wchar_t* ETCH_CONNECTION_NODELAY      = L"TcpConnection.noDelay";
const wchar_t* ETCH_CONNECTION_TRAFCLASS    = L"TcpConnection.trafficClass";
const wchar_t* ETCH_CONNECTION_BUFSIZE      = L"TcpConnection.bufferSize";
const wchar_t* ETCH_TCPLISTENER_BACKLOG     = L"TcpListener.backlog";

unsigned next_etch_connection_id();

unsigned connection_id_farm;

/*
 * is_good_tcp_params()
 */
int is_good_tcp_params(etch_url* url, void* resources, etch_rawsocket* socket)
{
    int whicherr = 0;

    if (NULL == socket)
    {
        if (NULL == url->host) whicherr  = 0x1; 
        if (url->port <= 0 || url->port > 0xffff) whicherr |= 0x2; 
    }

    return whicherr == 0;
}


/*
 * etch_socket_set_timeout
 * set timeout for specified socket
 */
int etch_socket_set_timeout(etch_rawsocket* socket, const unsigned ms)
{
    const int result = apr_socket_timeout_set(socket, ms * 1000);
    return 0 == result? 0: -1;
}


/*
 * etch_socket_get_timeout
 * get timeout set on specified socket
 */
int etch_socket_get_timeout(etch_rawsocket* socket, unsigned* retms)
{
    int64 val = 0;
    if (0 != apr_socket_timeout_get(socket, &val)) return -1;
    if (retms) *retms = (unsigned) (val / 1000);
    return 0;  
}


/**
 * etchconx_set_socket_options()
 */
int etchconx_set_socket_options(void *c)
{
    return 0;
}


/**
 * next_etch_connection_id()
 * return a unique ID used to identify a connection instance
 */
unsigned next_etch_connection_id()
{
    do { apr_atomic_inc32 ((volatile apr_uint32_t*) &connection_id_farm);
       } while(connection_id_farm == 0); 

    return connection_id_farm;
}


/**
 * etch_defconx_on_data()
 * default receive data handler  
 */
int etch_defconx_on_data (void* conx, const int unused, int length, void* voidData)
{
    char* msgmask = "connxn %d no data handler provided for %d bytes\n";
    etch_connection* cx = (etch_connection*) conx;
    ETCH_ASSERT(is_etch_connection(cx));
    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_WARN, msgmask, cx->conxid, length);
    return 0;
}


/*
 * etch_defconx_on_event()
 * default handler for connection events
 */
int etch_defconx_on_event (void* c, const int e, int p1, void* p2)
{
    return 0;
}

/*
 * etchconx_wait_up() 
 * block until connection comes up or timeout
 */
int etchconx_wait_up(etch_connection* cx, int timeoutms)
{
    etch_status_t status = ETCH_SUCCESS;
    status = etch_wait_timedwait(cx->wait_up, ETCH_CONXEVT_UP, timeoutms);
    if(status != ETCH_SUCCESS) {
        return -1;
    }
    return 0;
}

/*
 * etchconx_wait_down() 
 * block until connection is closed or timeout
 */
int etchconx_wait_down(etch_connection* cx, int timeoutms)
{
    etch_status_t status = ETCH_SUCCESS;
    status = etch_wait_timedwait(cx->wait_down, ETCH_CONXEVT_DOWN, timeoutms);
    if(status != ETCH_SUCCESS) {
        return -1;
    }
    return 0;
}

/*
 * etch_destroy_connection() 
 * free memory owned by connection.
 */
int etch_destroy_connection(etch_connection* cx)
{
    if (cx)
    {
        etch_wait_destroy(cx->wait_up);
        etch_wait_destroy(cx->wait_down);
        etch_mutex_destroy(cx->mutex);
        etch_free(cx->hostname);
        /* free apr subpool */
        //printf("5 destroying apr pool %p\n",cx->aprpool);
        apr_pool_destroy(cx->aprpool);
        
    }
    return 0;
}

/*
 * etch_init_connection()  
 * initialize an etch_connection (abstract base)
 */
int etch_init_connection (etch_connection* cx, etch_rawsocket* socket, void* owner)
{
    etch_status_t status = ETCH_SUCCESS;
    memset(cx, 0, sizeof(etch_connection));

    cx->conxid = next_etch_connection_id();
    cx->owner  = owner;

    // TODO: pool handling

    /* set memory pool used for APR objects associated with this connection.
     * if the connection is being created with an existing socket, use the
     * memory pool already assigned to it, otherwise partition a new subpool. 
     * if connection allocates a subpool, connection will free it on destroy.
     */
    if (socket) {
        cx->aprpool = apr_socket_pool_get(socket);
    }

    if (cx->aprpool == NULL) {
        // TODO: pool
        apr_pool_create(&cx->aprpool, g_etch_main_pool);
        //printf("3 creating apr pool %p\n",cx->aprpool);
        /* set pool abort function */
        apr_pool_abort_set(etch_runtime_mem_abort, cx->aprpool);
        
        cx->is_ownpool = TRUE;
    }

    if (cx->aprpool == NULL)
    {   cx->aprpool = g_etch_main_pool;
        cx->is_ownpool = FALSE;
    }

    cx->sig      = ETCH_CONX_SIG;

    // TODO: pool
    status = etch_mutex_create(&cx->mutex, ETCH_MUTEX_NESTED, NULL);
    ETCH_ASSERT(status == ETCH_SUCCESS);
    // TODO: pool
    status = etch_wait_create(&cx->wait_up, NULL);
    ETCH_ASSERT(status == ETCH_SUCCESS);
    // TODO: pool
    status = etch_wait_create(&cx->wait_down, NULL);
    ETCH_ASSERT(status == ETCH_SUCCESS);

    cx->bufsize  = ETCH_CONX_DEFAULT_BUFSIZE;
    cx->on_event = etch_defconx_on_event;
    cx->on_data  = etch_defconx_on_data;
    cx->set_socket_options = etchconx_set_socket_options;
    return 0;
}



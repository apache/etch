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

#include "apr_time.h"
#include "apr_network_io.h"
#include "etchthread.h"
#include "etch_connection.h"
#include "etch_encoding.h"
#include "etchflexbuf.h"
#include "etchlog.h"
#include "etch_global.h"

const wchar_t* ETCH_CONNECTION_RECONDELAY   = L"TcpConnection.reconnect_delay";
const wchar_t* ETCH_CONNECTION_AUTOFLUSH    = L"TcpConnection.autoFlush";
const wchar_t* ETCH_CONNECTION_KEEPALIVE    = L"TcpConnection.keepAlive";
const wchar_t* ETCH_CONNECTION_LINGERTIME   = L"TcpConnection.lingerTime";
const wchar_t* ETCH_CONNECTION_NODELAY      = L"TcpConnection.noDelay";
const wchar_t* ETCH_CONNECTION_TRAFCLASS    = L"TcpConnection.trafficClass";
const wchar_t* ETCH_CONNECTION_BUFSIZE      = L"TcpConnection.bufferSize";
const wchar_t* ETCH_TCPLISTENER_BACKLOG     = L"TcpListener.backlog";
char* ETCHCONX = "CONX";

unsigned next_etch_connection_id();


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
int etch_defconx_on_data (void* conx, const int unused, int length, char* data)
{
    char* msgmask = "connxn %d no data handler provided for %d bytes\n";
    etch_connection* cx = (etch_connection*) conx;
    ETCH_ASSERT(is_etch_connection(cx));
    etchlog(ETCHCONX, ETCHLOG_WARNING, msgmask, cx->conxid, length);
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
 * etchconx_wait_for() 
 * block until condition is met or timeout
 */
int etchconx_wait_for (etch_connection* cx, const int64 waitval, const int timeoutms)
{
    etchwait* waiter = cx->waiter;
    int64* pcondvar  = &cx->waitcond;
    int result = 0;

    result = waiter->timed_waitequal (waiter, pcondvar, waitval, timeoutms);
    return result;
}


/*
 * etchconx_init_waitstate() 
 * pre-set the condition variable target for wait state.
 * this sets wait state to "waiting", and must be invoked prior to waiting up or down,
 * if the up or down might complete before the user calls wait up or down. for example,
 * if we want to establish a connection, then wait for it to come up, the connection
 * may likely complete on another thread prior to the user calling wait up. we do this
 * pre-setting of the condition variable to provide a target for the connection to 
 * mark the state. 
 */
void etchconx_init_waitstate (etch_connection* cx)
{
    ETCH_ASSERT(cx && cx->waiter);
    cx->waiter->cond_var = &cx->waitcond;   
}


/*
 * etchconx_wait_up() 
 * block until connection comes up or timeout
 */
int etchconx_wait_up (etch_connection* cx, int timeoutms)
{
    cx->waiter->cond_waitfor = 0;  /* ETCH_CONXEVT_UP; semikludge */
    return etchconx_wait_for(cx, ETCH_CONXEVT_UP, timeoutms);
}


/*
 * etchconx_wait_down() 
 * block until connection is closed or timeout
 */
int etchconx_wait_down (etch_connection* cx, int timeoutms)
{
    cx->waiter->cond_waitfor = 0;  /* ETCH_CONXEVT_DOWN; semikludge */
    return etchconx_wait_for(cx, ETCH_CONXEVT_DOWN, timeoutms);
}


/*
 * new_etch_socket() 
 * socket object wrapper constructor
 */
etch_socket* new_etch_socket(etch_rawsocket* rawsocket)
{
    etch_socket* newobj = (etch_socket*) new_object (sizeof(etch_socket), 
        ETCHTYPEB_SOCKET, CLASSID_ETCHSOCKET);
    /* fyi this object uses the generic destructor since the rawsocket
     * is allocated from the apr pool and not explicitly freed by etch */
    newobj->value = rawsocket;
    return newobj;
}


/*
 * etch_destroy_connection() 
 * free memory owned by connection.
 */
int etch_destroy_connection(etch_connection* cx)
{
    if (cx)
    {   destroy_etchwait (cx->waiter); 
        destroy_etchmutex(cx->mutex);
        etch_free(cx->hostname);
        if (cx->is_ownpool) /* free apr subpool */
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
    memset(cx, 0, sizeof(etch_connection));

    cx->conxid = next_etch_connection_id();
    cx->owner  = owner;

    /* set memory pool used for APR objects associated with this connection.
     * if the connection is being created with an existing socket, use the
     * memory pool already assigned to it, otherwise partition a new subpool. 
     * if connection allocates a subpool, connection will free it on destroy.
     */
    if (socket) 
        cx->aprpool = apr_socket_pool_get(socket);

    if (cx->aprpool == NULL)
    {   apr_pool_create(&cx->aprpool, etch_apr_mempool);
        cx->is_ownpool = TRUE;
    }

    if (cx->aprpool == NULL)
    {   cx->aprpool = etch_apr_mempool;
        cx->is_ownpool = FALSE;
    }

    cx->sig      = ETCH_CONX_SIG;
    cx->mutex    = new_mutex(cx->aprpool, TRUE);  
    cx->waiter   = new_wait (cx->aprpool); 

    cx->bufsize  = ETCH_CONX_DEFAULT_BUFSIZE;
    cx->on_event = etch_defconx_on_event;
    cx->on_data  = etch_defconx_on_data;
    cx->set_socket_options = etchconx_set_socket_options;
    return 0;
}



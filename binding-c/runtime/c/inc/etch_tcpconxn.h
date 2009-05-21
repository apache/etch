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

#ifndef ETCHTCPCONXN_H
#define ETCHTCPCONXN_H

#include "apr_thread_proc.h"
#include "etch_connection.h"
#include "etch_transportdata.h"
#include "etch_transportint.h"
#include "etch_sessiondata.h"
#include "etch_url.h"

typedef struct etch_tcp_client etch_tcp_client;


/*
 * tcp specific connection type
 */
typedef struct etch_tcp_connection
{
    unsigned int     hashkey;  
    unsigned short   obj_type; 
    unsigned short   class_id;
    struct objmask*  vtab;  
    int  (*destroy) (void*);
    void*(*clone)   (void*); 
    obj_gethashkey   get_hashkey;
    struct objmask*  parent;
    etchresult*      result;
    unsigned int     refcount;
    unsigned int     length;
    unsigned char    is_null;
    unsigned char    is_copy;
    unsigned char    is_static;
    unsigned char    reserved;

    etch_connection  cx;
    etch_tcp_client* rcvlxr;  /* owned */

    i_transportdata* itd;     /* owned */

    i_sessiondata*   session; /* not owned */

    int linger;
    int traffic_class;

    unsigned char is_nodelay;
    unsigned char is_keepalive;
    unsigned char is_autoflush;
    unsigned char is_session_owned;

} etch_tcp_connection;


/*
 * udp connection
 * placeholder - this will go in etch_udpconxn.h when we implement udp transport
 */
typedef struct etch_udp_connection
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
 
    etch_connection cx;

    apr_sockaddr_t *last_from;

} etch_udp_connection;


/**
 * etch_tcp_client 
 * tcp client listener class
 */
typedef struct etch_tcp_client
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
 
    etch_tcp_connection* cxlisten; /* not owned */
    etch_threadpool* threadpool;   /* owned */
    unsigned listener_id;

    unsigned char is_started;
    unsigned char is_threadpool_owned;

} etch_tcp_client;


unsigned connection_id_farm;

etch_tcp_connection* new_tcp_connection(etch_url*, void* resources, etch_rawsocket*);
int  init_etch_tcpconx_interfaces (etch_tcp_connection*);
int  new_tcpsocket (apr_socket_t**, apr_pool_t*);
int  destroy_etch_tcp_connection(etch_tcp_connection*);
int  etch_tcpconx_start(etch_tcp_connection*);
int  etch_tcpclient_start_listener (etch_tcp_connection*);
int  etch_tcpconx_open (etch_tcp_connection*, const int is_reconnect);
int  etch_tcpconx_close(etch_tcp_connection*, const int is_linger);
int  etch_tcpconx_wait_until(etch_tcp_connection*, const int64 waitval, const int timeoutms);
int  etch_tcpconx_on_event(etch_tcp_connection*, const int e, int p1, void* p2);
int  etch_tcpclient_send  (etch_tcp_connection*, char* buf, const size_t, int*);
int  etch_tcpclient_sendex(etch_tcp_connection*, char* buf, const size_t, const int, int*);
int  etch_tcpclient_receive (etch_tcp_connection*, char*, const size_t, int*);
int  etch_tcpclient_receivex(etch_tcp_connection*, char*, const size_t, const int, int*);
int  etch_tcpclient_stop(etch_tcp_connection*);
int  etch_tcpclient_stop_listener (etch_tcp_connection*);
int  etch_tcpclient_listenerproc(etch_threadparams*);
int  is_good_tcp_params(etch_url*, void* resources, etch_rawsocket*);


#endif  /* ETCHTCPCONXN_H */
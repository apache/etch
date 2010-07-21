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

#ifndef ETCHCONNECTION_H
#define ETCHCONNECTION_H

#include "apr_thread_proc.h"
#include <apr_network_io.h>
#include <apr_thread_mutex.h>
#include <apr_thread_cond.h>
#include "etch_transport_data.h"
#include "etch_transportint.h"
#include "etch_sessionint.h"
#include "etch_thread.h"
#include "etch_mutex.h"
#include "etch_wait.h"
#include "etch_url.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef apr_socket_t etch_rawsocket;
 
typedef int (*etch_set_socket_options) (void* conxn);
typedef int (*etch_connection_event_handler) (void* conxn, const int, int, void*);

int  etch_defconx_on_event(void* conxn, const int, int, void*);
int  etch_defconx_on_data (void* conxn, const int, int, void*);

#define IS_ETCH_SOCKET_TIMEOUT(n) (APR_TIMEUP == n || 730060 == n)
#define ETCH_CONX_DEFAULT_BUFSIZE (1024 * 32)
#define ETCH_CONX_NO_LINGER  FALSE
#define ETCH_CONX_USE_LINGER TRUE
#define ETCH_CONX_NOT_RECONNECTING FALSE
#define ETCH_CONX_IS_RECONNECTING  TRUE
#define ETCH_THIS_END_CLOSED   (-2)
#define ETCH_OTHER_END_CLOSED  (-3)
#define ETCH_SHUTDOWN_NOTIFIED (-4)
#define APR_THIS_END_CLOSED  730053
#define APR_OTHER_END_CLOSED 730054


/*
 * etch_connection  
 * abstract base class, never instantiated so not an object
 */
typedef struct etch_connection
{
    unsigned        sig;
    unsigned        conxid;
    void*           owner;
    apr_socket_t*   socket;
    apr_sockaddr_t* sockdata;
    apr_pool_t*     aprpool;  
    etch_mutex*      mutex;
    unsigned        bufsize;
    etch_object*    listener;
    etch_thread*    thread;
    unsigned char   is_ownpool;
    unsigned char   is_started;
    unsigned char   is_closing;
    unsigned char   unused;

    etch_wait_t* wait_up;      /* wait object - owned */
    etch_wait_t* wait_down;    /* wait object - owned */
    etch_object*  session;   /* session data - not owned */
    char*     hostname;  /* ansi string - owned */
    int64     waitcond;  /* up/down wait condition */
    int       port;
    int       delay;
 
    etch_connection_event_handler  on_data;
    etch_connection_event_handler  on_event;
    etch_set_socket_options  set_socket_options;

} etch_connection;



#define ETCH_DEFAULT_SOCKET_FAMILY  APR_INET
extern unsigned connection_id_farm;

#define ETCH_CONNECTION_DEFLINGERTIME 30
#define ETCH_CONNECTION_DEFNODELAY  TRUE
#define ETCH_CONNECTION_DEFRETRYATTEMPTS 0
#define ETCH_CONNECTION_DEFRETRYDELAYMS  1000

extern const wchar_t* ETCH_CONNECTION_RECONDELAY;
extern const wchar_t* ETCH_CONNECTION_AUTOFLUSH;
extern const wchar_t* ETCH_CONNECTION_KEEPALIVE;
extern const wchar_t* ETCH_CONNECTION_LINGERTIME;
extern const wchar_t* ETCH_CONNECTION_NODELAY;
extern const wchar_t* ETCH_CONNECTION_TRAFCLASS;
extern const wchar_t* ETCH_CONNECTION_BUFSIZE;
extern const wchar_t* ETCH_TCPLISTENER_BACKLOG;

#define ETCH_CONXEVT_CREATED            0x1
#define ETCH_CONXEVT_CREATERR           0x2
#define ETCH_CONXEVT_SHUTDOWN           0x3
#define ETCH_CONXEVT_DATA               0x4
#define ETCH_CONXEVT_RECEIVING          0x5
#define ETCH_CONXEVT_RECEIVETIMEOUT     0x6
#define ETCH_CONXEVT_RECEIVERR          0x7
#define ETCH_CONXEVT_RECEIVED           0x8
#define ETCH_CONXEVT_RECEIVEND          0x9
#define ETCH_CONXEVT_SENDING            0xa
#define ETCH_CONXEVT_SENDTIMEOUT        0xb
#define ETCH_CONXEVT_SENDERR            0xc
#define ETCH_CONXEVT_SENT               0xd
#define ETCH_CONXEVT_SENDEND            0xe
#define ETCH_CONXEVT_CONXCLOSED         0xf
#define ETCH_CONXEVT_PEERCLOSED         0x10
#define ETCH_CONXEVT_OPENING            0x11
#define ETCH_CONXEVT_OPENERR            0x12
#define ETCH_CONXEVT_OPENED             0x13
#define ETCH_CONXEVT_STARTING           0x14
#define ETCH_CONXEVT_STARTERR           0x15
#define ETCH_CONXEVT_STARTED            0x16 
#define ETCH_CONXEVT_ACCEPTPUMPEXIT     0x1e    
#define ETCH_CONXEVT_ACCEPTPUMPEXITERR  0x1f  
#define ETCH_CONXEVT_RCVPUMP_START      0x20
#define ETCH_CONXEVT_RCVPUMP_RECEIVING  0x21
#define ETCH_CONXEVT_RCVPUMP_ERR        0x22
#define ETCH_CONXEVT_RCVPUMP_STOP       0x23
#define ETCH_CONXEVT_ACCEPTING          0x25 
#define ETCH_CONXEVT_ACCEPTERR          0x26 
#define ETCH_CONXEVT_ACCEPTED           0x27 
#define ETCH_CONXEVT_LISTENED           0x28 
#define ETCH_CONXEVT_UP                 0x2a 
#define ETCH_CONXEVT_DOWN               0x2b
#define ETCH_CONXEVT_SOCKOPTERR         0x2f
#define ETCH_CONXEVT_STOPPING           0x30
#define ETCH_CONXEVT_STOPERR            0x31
#define ETCH_CONXEVT_STOPPED            0x32
#define ETCH_CONXEVT_CLOSING            0x33
#define ETCH_CONXEVT_CLOSERR            0x34
#define ETCH_CONXEVT_CLOSED             0x35 
#define ETCH_CONXEVT_DESTRUCTOR         0x3a 
#define ETCH_CONXEVT_DESTROYING         0x3b 
#define ETCH_CONXEVT_DESTROYED          0x3c

int etch_init_connection(etch_connection*, etch_rawsocket*, void* owner);
int etch_destroy_connection(etch_connection*);
int etchconx_wait_up  (etch_connection*, int timeoutms);
int etchconx_wait_down(etch_connection*, int timeoutms);

#ifdef __cplusplus
}
#endif

#endif  /* ETCHCONNECTION_H */

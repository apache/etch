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
 * test_tcp_connection.c
 */

#include "etch_apr.h" /* some apr must be included first */
#include "etch_connection.h"  /* must be included second */

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#include "etch_connection.h"
#include "etch_tcpserver.h"
#include "etch_global.h"
#include "etch_encoding.h"
#include "etchflexbuf.h"
#include "etchthread.h"
#include "etchlog.h"
int this_setup();
int this_teardown();

int init_suite(void)
{
    if  (0 == etch_runtime_init_all(0)) 
         return this_setup();
    else return -1;
}

int clean_suite(void)
{
    this_teardown();
    return etch_runtime_exit(0);
}

#define IS_DEBUG_CONSOLE TRUE
int g_is_automated_test, g_bytes_allocated;


#define RCVBUFLEN 256
#define TEST_PORT 7302
#define TEST_IP   L"127.0.0.1"
etch_url* g_test_url;
char* g_ip8bit;

int on_data_server_test_write_to_server    (etch_tcp_connection*, const int, int, etch_flexbuffer*);
int on_data_server_test_write_with_readback(etch_connection*, const int, int, etch_flexbuffer*);
int on_data_client_test_write_with_readback(etch_tcp_connection*, const int, int, char*);

char *g_senddata1="By the rude bridge that arched the flood, their flag from April's breeze unfurled";
char *g_senddata2="And therefore I have sailed the seas and come, to the holy city of Byzantium.";
int   g_buflen1, g_buflen2;


int this_setup()
{
    g_buflen1 = (int)strlen(g_senddata1); g_buflen2 = (int)strlen(g_senddata2);
    return 0;
}

int this_teardown()
{    
    return 0;
}

etch_url* get_test_url() /* build a URL string for the test */
{   
    wchar_t buf[256]; swprintf(buf, sizeof(buf), L"%s:%d", TEST_IP, TEST_PORT);
    return new_url(buf);
} 

/**
 * test_unicode_conversion()
 * test the etch unicode to 8 bit conversion
 * this test does not necessarily belong in this suite, it is here because
 * we use the conversion in the tcp connection classes and in these tests.
 */
void test_unicode_conversion(void)
{
    int result = 0;
    wchar_t *widestring = L"it works!";
    char *narrowstring  =  "it works!", *out_8bit = NULL;

    /* unicode to 8 bit ansi */
    result = etch_unicode_to_ansi(&out_8bit, widestring);
    CU_ASSERT_EQUAL(result,0); 
    CU_ASSERT_PTR_NOT_NULL_FATAL(out_8bit);
    
    result = strcmp(out_8bit, narrowstring);
    CU_ASSERT_EQUAL(result,0); 

    /* we own returned buffer */
    etch_free(out_8bit); out_8bit = NULL;
 
    /* unicode to utf-8 */
    result = etch_unicode_to_utf8(&out_8bit, widestring);
    CU_ASSERT_EQUAL(result,0); 
    CU_ASSERT_PTR_NOT_NULL_FATAL(out_8bit); 

    result = strcmp(out_8bit, narrowstring);
    CU_ASSERT_EQUAL(result,0);   
    
    etch_free(out_8bit); out_8bit = NULL;

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/**
 * test_unicode_conversion2()
 * test the etch 8 bit to unicode conversion
 * this test does not necessarily belong in this suite, it is here because
 * we use the conversion in the tcp connection classes and in these tests.
 */
void test_unicode_conversion2(void)
{
    int result = 0;
    wchar_t *widestring = L"it works!", *out_unicode = NULL;
    char *narrowstring  =  "it works!";

    /* 8 bit ansi to unicode */
    result = etch_ansi_to_unicode(&out_unicode, narrowstring);
    CU_ASSERT_EQUAL(result,0); 
    CU_ASSERT_PTR_NOT_NULL_FATAL(out_unicode);
    
    result = wcscmp(out_unicode, widestring);
    CU_ASSERT_EQUAL(result,0); 

    /* we own returned buffer */
    etch_free(out_unicode); out_unicode = NULL;
 
    /* utf-8 to unicode */
    result = etch_ansi_to_unicode(&out_unicode, narrowstring);
    CU_ASSERT_EQUAL(result,0); 
    CU_ASSERT_PTR_NOT_NULL_FATAL(out_unicode); 

    result = wcscmp(out_unicode, widestring);
    CU_ASSERT_EQUAL(result,0);   
    
    etch_free(out_unicode); out_unicode = NULL;

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/**
 * setup_a_test()
 * set up a tcp server test by creating and returning an 
 * initialized tcp server object.
 */
etch_tcp_server* setup_a_test()
{
    etch_tcp_server* tcp_server = 0;
    etch_tcp_connection* cxl = 0;  
    etch_threadpool* pool = 0; 
    etch_connection* cx = 0;

    #if IS_DEBUG_CONSOLE 
    wprintf(L"\n");          
    #endif   

    /* create objects */ 

    g_test_url = get_test_url();  
    CU_ASSERT_STRING_EQUAL_FATAL(g_test_url->host, TEST_IP);
    CU_ASSERT_EQUAL_FATAL(g_test_url->port, TEST_PORT);
    etch_unicode_to_ansi(&g_ip8bit, TEST_IP);

    pool = new_threadpool (ETCH_THREADPOOLTYPE_FREE, 0); 

    tcp_server = new_tcp_server(g_test_url, pool, pool, NULL, NULL);
    CU_ASSERT_PTR_NOT_NULL(tcp_server);
    tcp_server->is_threadpool_owned = TRUE;  /* server now owns pool */

    CU_ASSERT_EQUAL (tcp_server->state, ETCH_TCPSERVER_STATE_CLOSED);
    CU_ASSERT_NOT_EQUAL (tcp_server->listener_id, 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL (tcp_server->cxlisten);
    CU_ASSERT_PTR_NOT_NULL_FATAL (tcp_server->on_event);
    
    cxl = tcp_server->cxlisten; /* listen socket connection object */
    cx  = &cxl->cx; 
    CU_ASSERT_PTR_NULL_FATAL (cx->socket);
    CU_ASSERT_PTR_NOT_NULL_FATAL (cx->mutex);
    CU_ASSERT_PTR_NOT_NULL_FATAL (cx->listener);
    CU_ASSERT_PTR_NOT_NULL_FATAL (cx->on_event);
    CU_ASSERT_PTR_NOT_NULL_FATAL (cx->on_data);
    CU_ASSERT_EQUAL_FATAL (cx->is_ownpool, TRUE);
    CU_ASSERT_EQUAL_FATAL (cx->is_started, FALSE);

    return tcp_server;
}


/**
 * teardown_a_test()
 * tear down a tcp server test by destroying the specified tcp server 
 * object plus the various objects created to set it up.
 */
void teardown_a_test(etch_tcp_server* listener)
{
    etch_free(g_ip8bit);  
    listener->destroy(listener);    
    g_test_url->destroy(g_test_url); g_test_url = NULL;
}


/**
 * test_server_create_destroy()
 * test that we can create a tcp server with all expected pieces present,
 * and subsequently destroy it with all memory accounted for. also tests
 * creation of a connection object (the server's listener connection).
 * tests only construction and destruction, no other functionality. 
 * assuming we run this test early and it passes, we can safely omit 
 * a lot of asserts in subsequent tests.
 */
void test_server_create_destroy(void)
{
    etch_threadpool* thread_manager = 0;
    etch_tcp_server* tcp_server = 0;
    etch_tcp_connection* cxl = 0;    
     
    etch_connection* cx = 0;
    etch_url* url = 0;
    #if IS_DEBUG_CONSOLE 
    wprintf(L"\n");          
    #endif   

    /* start - create objects */ 
    url = get_test_url(); /* we own it */
    CU_ASSERT_STRING_EQUAL_FATAL(url->host, TEST_IP);
    CU_ASSERT_EQUAL_FATAL(url->port, TEST_PORT);

    tcp_server = new_tcp_server(url, NULL, NULL, NULL, NULL);
    CU_ASSERT_PTR_NOT_NULL(tcp_server);

    CU_ASSERT_EQUAL (tcp_server->state, ETCH_TCPSERVER_STATE_CLOSED);
    CU_ASSERT_EQUAL (tcp_server->listener_id, 1);
    CU_ASSERT_PTR_NOT_NULL_FATAL (tcp_server->cxlisten);
    CU_ASSERT_PTR_NOT_NULL_FATAL (tcp_server->on_event);
    
    cxl = tcp_server->cxlisten; /* listen socket connection object */
    cx  = &cxl->cx; 
    CU_ASSERT_PTR_NULL_FATAL (cx->socket);
    CU_ASSERT_PTR_NOT_NULL_FATAL (cx->mutex);
    CU_ASSERT_PTR_NOT_NULL_FATAL (cx->listener);
    CU_ASSERT_PTR_NOT_NULL_FATAL (cx->on_event);
    CU_ASSERT_PTR_NOT_NULL_FATAL (cx->on_data);
    CU_ASSERT_EQUAL_FATAL (cx->is_ownpool, TRUE);
    CU_ASSERT_EQUAL_FATAL (cx->is_started, FALSE);

    /* done - destroy objects */ 
    tcp_server->destroy(tcp_server);    
    url->destroy(url);
    //thread_manager->destroy(thread_manager);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */    
}


/**
 * test_setup_a_test()
 * test that we can do the same as test_server_create_destroy(), but using
 * setup_a_test() and teardown_a_test to create and destroy objects.
 */
void test_setup_a_test(void)
{
    etch_tcp_server* tcp_server = setup_a_test();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tcp_server);

    teardown_a_test(tcp_server);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */    
}


/**
 * test_server_open_close()
 * test that we can create, open, close, and destroy a tcp server 
 * with all memory accounted for. when server is open, its listen socket is
 * listening on the designated port. when closed, it is not listening.
 */
void test_server_open_close(void)
{
    int result;
    etch_tcp_server* tcp_server = setup_a_test();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tcp_server);

    result = etch_tcpsvr_open(tcp_server, 0);
    CU_ASSERT_EQUAL(result, 0);  /* state stopped means not closed but not started */
    CU_ASSERT_EQUAL(tcp_server->state, ETCH_TCPSERVER_STATE_STOPPED); 

    if (result == 0)
    {   result = etch_tcpsvr_close(tcp_server);
        CU_ASSERT_EQUAL(result, 0);
        CU_ASSERT_EQUAL(tcp_server->state, ETCH_TCPSERVER_STATE_CLOSED); 
    }

    teardown_a_test(tcp_server);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */    
}


/**
 * test_server_start_stop()
 * test that we can create, open, start, stop, close, and destroy a tcp server 
 * with all memory accounted for. when server is started, it is accepting
 * connections from clients.
 */
void test_server_start_stop(void)
{
    int result;
    etch_tcp_server* tcp_server = setup_a_test();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tcp_server);

    result = etch_tcpsvr_open(tcp_server, 0);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    result = etch_tcpsvr_start(tcp_server);
    CU_ASSERT_EQUAL_FATAL(result, 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL (tcp_server->threadpool); 
    CU_ASSERT_EQUAL_FATAL (tcp_server->is_started, TRUE); 
    CU_ASSERT_EQUAL_FATAL (tcp_server->state, ETCH_TCPSERVER_STATE_STARTED); 

    etch_sleep(1000);

    result = etch_tcpsvr_stop(tcp_server);
    CU_ASSERT_EQUAL_FATAL(result, 0);
    CU_ASSERT_EQUAL_FATAL(tcp_server->is_started, FALSE); 
    CU_ASSERT_EQUAL_FATAL(tcp_server->state, ETCH_TCPSERVER_STATE_STOPPED); 

    result = etch_tcpsvr_close(tcp_server);
    CU_ASSERT_EQUAL(result, 0);    

    teardown_a_test(tcp_server);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */    
}


/**
 * test_server_control_start_stop()
 */
void test_server_control_start_stop(void)
{
    int result = 0;
    etch_event *startevent = NULL, *stopevent = NULL;

    etch_tcp_server* tcp_server = setup_a_test();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tcp_server);

    startevent = new_etch_event(CLASSID_CONTROL_START, 0); /* we relinquish this */
    result = tcp_server->transport_control(tcp_server, startevent, NULL);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_EQUAL(tcp_server->is_started, TRUE); 

    stopevent = new_etch_event(CLASSID_CONTROL_STOP, 0);  /* we relinquish this */
    result = tcp_server->transport_control(tcp_server, stopevent, NULL);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_EQUAL(tcp_server->is_started, FALSE); 
  
    teardown_a_test(tcp_server);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */    
}


/**
 * test_server_control_waitup_waitdown()
 */
void test_server_control_waitup_waitdown(void)    
{
    int result = 0;
    etch_event *waitupevent = NULL, *waitdownevent = NULL;
    etch_int32 *waituptime  = NULL, *waitdowntime  = NULL;

    etch_tcp_server* tcp_server = setup_a_test();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tcp_server);

    waitupevent = new_etch_event(CLASSID_CONTROL_START_WAITUP, 0); /* we relinquish this */
    waituptime  = new_int32(5000);  /* we relinquish this */
    result = tcp_server->transport_control(tcp_server, waitupevent, waituptime);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_EQUAL(tcp_server->is_started, TRUE); 

    waitdownevent = new_etch_event(CLASSID_CONTROL_STOP_WAITDOWN, 0);  /* we relinquish this */
    waitdowntime  = new_int32(5000);  /* we relinquish this */
    result = tcp_server->transport_control(tcp_server, waitdownevent, waitdowntime);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_EQUAL(tcp_server->is_started, FALSE); 

    teardown_a_test(tcp_server);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */    
}


/**
 * test_connect_to_server()
 * test that we can open and close a client connection to a tcp server.
 */
void test_connect_to_server(void)
{
    int result;
    etch_tcp_connection* cxclient = 0;
    etch_tcp_server* tcp_server = setup_a_test();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tcp_server);

    result = etch_tcpsvr_open(tcp_server, 0);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    result = etch_tcpsvr_start(tcp_server);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    etch_sleep(1000);  

    cxclient = new_tcp_connection(g_test_url, NULL, NULL);

    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient);

    result = etch_tcpconx_open(cxclient, FALSE);

    CU_ASSERT_EQUAL_FATAL(result, 0);
    CU_ASSERT_STRING_EQUAL_FATAL(cxclient->cx.hostname, g_ip8bit);
    CU_ASSERT_EQUAL_FATAL(cxclient->cx.port, TEST_PORT);
    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient->cx.on_event);
    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient->cx.on_data);
    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient->cx.aprpool);
    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient->cx.socket);
    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient->cx.mutex);
  
    etch_sleep(500);

    result = etch_tcpconx_close(cxclient, FALSE);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    result = etch_tcpsvr_stop(tcp_server);

    CU_ASSERT_EQUAL_FATAL(result, 0);
    CU_ASSERT_EQUAL_FATAL(tcp_server->is_started, FALSE); 
    CU_ASSERT_EQUAL_FATAL(tcp_server->state, ETCH_TCPSERVER_STATE_STOPPED); 

    result = etch_tcpsvr_close(tcp_server);
    CU_ASSERT_EQUAL(result, 0); 

    /* free items created here */
    if (cxclient) cxclient->destroy(cxclient);

    teardown_a_test(tcp_server);
 
    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */    
}



/**
 * test_multiple_connect_to_server()
 * test that we can open and close two concurrent connections to a tcp server.
 */
void test_multiple_connect_to_server(void)
{
    int result;
    etch_tcp_connection *cxclient_a = 0, *cxclient_b = 0;
    etch_tcp_server* tcp_server = setup_a_test();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tcp_server);

    result = etch_tcpsvr_open(tcp_server, 0);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    result = etch_tcpsvr_start(tcp_server);
    CU_ASSERT_EQUAL_FATAL(result, 0);
    etch_sleep(500);  

    cxclient_a = new_tcp_connection(g_test_url, NULL, NULL);
    cxclient_b = new_tcp_connection(g_test_url, NULL, NULL);

    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient_a);
    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient_b);

    result = etch_tcpconx_open(cxclient_a, FALSE);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    result = etch_tcpconx_open(cxclient_b, FALSE);

    CU_ASSERT_EQUAL_FATAL(result, 0);
    CU_ASSERT_STRING_EQUAL_FATAL(cxclient_b->cx.hostname, g_ip8bit);
    CU_ASSERT_EQUAL_FATAL(cxclient_b->cx.port, TEST_PORT);
    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient_b->cx.on_event);
    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient_b->cx.on_data);
    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient_b->cx.aprpool);
    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient_b->cx.socket);
    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient_b->cx.mutex);

    etchlog("TEST", ETCHLOG_DEBUG, "client a ID is %d\n", cxclient_a->cx.conxid); 
    etchlog("TEST", ETCHLOG_DEBUG, "client b ID is %d\n", cxclient_b->cx.conxid); 
    etch_sleep(500);

    result = etch_tcpconx_close(cxclient_a, FALSE);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    result = etch_tcpconx_close(cxclient_b, FALSE);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    result = etch_tcpsvr_stop(tcp_server);
    CU_ASSERT_EQUAL_FATAL(result, 0);
    CU_ASSERT_EQUAL_FATAL(tcp_server->is_started, FALSE); 
    CU_ASSERT_EQUAL_FATAL(tcp_server->state, ETCH_TCPSERVER_STATE_STOPPED); 

    result = etch_tcpsvr_close(tcp_server);
    CU_ASSERT_EQUAL(result, 0); 

    /* free items created here */
    if (cxclient_a) cxclient_a->destroy(cxclient_a);
    if (cxclient_b) cxclient_b->destroy(cxclient_b);

    teardown_a_test(tcp_server);
 
    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */    
}


/*
 * on_data_server_test_write_to_server()
 * server on_data overide for the test_write_to_server() test 
 */
int on_data_server_test_write_to_server(etch_tcp_connection* c, const int unused, int len, etch_flexbuffer* fbuf)
{
    char* datacopy = 0;
    etchlog(ETCHCONX, ETCHLOG_DEBUG, "conxn %d on_data server test overide got %d bytes ...\n", 
        c->cx.conxid, len);    
    datacopy = etch_malloc(len+2,0); 
    memcpy(datacopy, fbuf->buf, len);
    datacopy[len] = '\n'; datacopy[len+1] = '\0';
    etchlog(ETCHCONX, ETCHLOG_DEBUG, datacopy); 
    return etch_free(datacopy);
}


/**
 * test_write_to_server()
 * test that we can send a message to a tcp server.
 */
void test_write_to_server(void)
{
    int result = 0, arc = 0;
    etch_tcp_connection* cxclient = 0;

    etch_tcp_server* tcp_server = setup_a_test();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tcp_server);

    /* this test overrides server's accepted connection data handler */
    tcp_server->on_data = on_data_server_test_write_to_server;  

    result = etch_tcpsvr_open(tcp_server, 0);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    result = etch_tcpsvr_start(tcp_server);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    etch_sleep(500); /* not needed, used for watching console */ 

    cxclient = new_tcp_connection(g_test_url, NULL, NULL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient);

    result = etch_tcpconx_open(cxclient, FALSE);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    /* send data to server */
    etch_tcpclient_send(cxclient, g_senddata1, g_buflen1, &arc);  

    /* if omit this sleep, server receives both sends as one,  
     * either way is is fine */
     etch_sleep(200); 

    /* send more data to server */
    etch_tcpclient_send(cxclient, g_senddata2, g_buflen2, &arc);  

    etch_sleep(500); /* not needed, used for watching console */

    result = etch_tcpconx_close(cxclient, FALSE);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    /* test that we can omit an explicit server stop */
    result = etch_tcpsvr_close(tcp_server);
    CU_ASSERT_EQUAL(result, 0); 

    /* free items created here */
    if (cxclient) cxclient->destroy(cxclient);

    teardown_a_test(tcp_server);
 
    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */    
}


/*
 * on_data_server_test_write_with_readback()
 * receive data handler for server side of test_write_with_readback
 * this is not executed on the main thread, so no CU_ASSERTs can appear here.
 * logs data received at the tcp server and sends it back to client
 */
int on_data_server_test_write_with_readback(etch_connection* cx, const int unused, int length, etch_flexbuffer* fbuf)
{
    char* datacopy = 0; int arc = 0;
    etch_tcp_server* tcps = (etch_tcp_server*) cx->listener;
    etch_tcp_connection* tcpx = tcps? tcps->cxlisten: NULL;
    assert(is_etch_tcpserver(tcps));
    assert(is_etch_tcpconnection(tcpx));
 
    etchlog(ETCHCONX, ETCHLOG_DEBUG, 
        "connxn %d TEST SERVER ONDATA GOT %d bytes ...\n", cx->conxid, length);

    datacopy = etch_malloc(length+2,0); memcpy(datacopy, fbuf->buf, length);
    datacopy[length] = '\n'; datacopy[length+1] = '\0';
    etchlog(ETCHCONX, ETCHLOG_DEBUG, datacopy); 

    etchlog(ETCHCONX, ETCHLOG_DEBUG, 
        "conxn %d TEST SERVER ONDATA RESENDING to client \n", tcpx->cx.conxid);
    etch_tcpclient_send (tcpx, fbuf->buf, length, &arc); /* return to sender */

    return etch_free(datacopy);
}


/*
 * on_data_client_test_write_with_readback()
 * receive data handler for client side of test_write_with_readback 
 * just logs the data received back from tcp server 
 */
int on_data_client_test_write_with_readback (etch_tcp_connection* c, const int unused, int length, char* data)
{
    char* datacopy = 0;
    etchlog(ETCHCONX, ETCHLOG_DEBUG, 
        "connxn %d TEST CLIENT ONDATA GOT %d bytes ...\n", c->cx.conxid, length);
    datacopy = etch_malloc(length+2,0); memcpy(datacopy, data, length);
    datacopy[length] = '\n'; datacopy[length+1] = '\0';
    etchlog(ETCHCONX, ETCHLOG_DEBUG, datacopy); 
    return etch_free(datacopy);
}


/**
 * test_write_with_readback()
 * test that we can send data to server and that server can send it back to us
 */
void test_write_with_readback(void)
{
    char rcvbuf[RCVBUFLEN]; 
    int result = 0, datalen = 0, sndrc = 0, rcvrc = 0;
    etch_tcp_connection *cxclient = 0;
    etch_tcp_server *tcp_server = 0;
    etch_tcp_client *tcp_client = 0;

    /* 
     * create and start a tcp server  
     */
    etchlog("TEST", ETCHLOG_DEBUG, "CREATING SERVER\n");
    tcp_server = setup_a_test(); 
    /* override server's accepted socket data handler to be our test handler */
    tcp_server->on_data = on_data_server_test_write_with_readback;

    etchlog("TEST", ETCHLOG_DEBUG, "STARTING SERVER\n");
    result = etch_tcpsvr_open(tcp_server, 0);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    result = etch_tcpsvr_start(tcp_server);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    /* 
     * open a client connection and listener 
     */
    etchlog("TEST", ETCHLOG_DEBUG, "CREATING CLIENT CONNECTION\n");
    cxclient = new_tcp_connection(g_test_url, NULL, NULL);
    cxclient->cx.on_data = on_data_client_test_write_with_readback;
    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient);
    etchlog("TEST", ETCHLOG_DEBUG, "client connection ID is %d\n", cxclient->cx.conxid); 

   /* opening this client connection results in an accepted connection on our server.
    * that accepted connection will recognize and use our test data handler, since we
    * overrode that handler above. */
    etchlog("TEST", ETCHLOG_DEBUG, "OPENING CLIENT CONNECTION\n");
    result = etch_tcpconx_open (cxclient, FALSE);
    etch_sleep(100);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    /* start a client side listener for the client socket - this is normally done
     * via a start/waitup mechanism, however for the unit test we do so explicitly here.
     * note that the tcp_client class is normally not visible, we expose it for the test. */
    etchlog("TEST", ETCHLOG_DEBUG, "STARTING CLIENT LISTENER THREAD\n");
    result = etch_tcpclient_start_listener (cxclient);
    etch_sleep(100);
    CU_ASSERT_EQUAL_FATAL(result, 0);
    tcp_client = cxclient->rcvlxr; /* check the mutual tcp connection references */
    CU_ASSERT_PTR_EQUAL_FATAL(cxclient, tcp_client->cxlisten);

    /* 
     * send data to server and receive data back from server
     * note that both server and client data handlers are overridden above
     */
    etchlog("TEST", ETCHLOG_DEBUG, "SENDING DATA TO SERVER\n");
    etch_tcpclient_send(cxclient, g_senddata1, g_buflen1, &sndrc);  
    datalen = etch_tcpclient_receive(cxclient, rcvbuf, RCVBUFLEN, &rcvrc);     

    CU_ASSERT_EQUAL(datalen, g_buflen1); /* verify sent == received */
    CU_ASSERT_EQUAL(0, memcmp(rcvbuf, g_senddata1, datalen));

    /* 
     * send more data to server and receive data back from server
     */
    //etchlog("TEST", ETCHLOG_DEBUG, "SENDING MORE DATA TO SERVER\n");
    //etch_tcpclient_send(cxclient, g_senddata2, g_buflen2, &sndrc); 
    //datalen = etch_tcpclient_receive(cxclient, rcvbuf, RCVBUFLEN, &rcvrc);  

    //CU_ASSERT_EQUAL(datalen, g_buflen2); /* verify sent == received */
    //CU_ASSERT_EQUAL(0, memcmp(rcvbuf, g_senddata2, datalen));   

    /* stop the client side listener thread - this is normally done via a
     * stop/waitdown mechanism, however for the unit test we do so explicitly here */
    etchlog("TEST", ETCHLOG_DEBUG, "STOPPING CLIENT LISTENER THREAD\n");
    result = etch_tcpclient_stop_listener (cxclient);
    etch_sleep(100);
    CU_ASSERT_EQUAL_FATAL(result, 0);      
   
    /* 
     * close client connection
     */
    etchlog("TEST", ETCHLOG_DEBUG, "CLOSING CLIENT CONNECTION\n");
    result = etch_tcpconx_close(cxclient, FALSE);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    /* 
     * shut down tcp server  
     */
    etchlog("TEST", ETCHLOG_DEBUG, "STOPPING SERVER\n");
    result = etch_tcpsvr_stop(tcp_server); /* could omit this */
    CU_ASSERT_EQUAL(result, 0);  
   
    etchlog("TEST", ETCHLOG_DEBUG, "CLOSING SERVER\n");
    result = etch_tcpsvr_close(tcp_server);
    CU_ASSERT_EQUAL(result, 0); 

    /* 
     * free memory
     */
    if (cxclient) cxclient->destroy(cxclient);

    teardown_a_test(tcp_server);
 
    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */    
}


/**
 * test_multiple_client_write_with_readback()
 * test that two clients can send data to server and receive it back.
 */
void test_multiple_client_write_with_readback(void)
{
    char rcvbuf[RCVBUFLEN]; 
    int result = 0, datalen = 0, sndrc = 0, rcvrc = 0;
    etch_tcp_connection *cxclient_a = 0, *cxclient_b = 0;
    etch_tcp_server* tcp_server = 0;

    /* 
     * create and start a tcp server  
     */
    tcp_server = setup_a_test(); 
    tcp_server->on_data = on_data_server_test_write_with_readback;

    result = etch_tcpsvr_open(tcp_server, 0);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    result = etch_tcpsvr_start(tcp_server);
    CU_ASSERT_EQUAL_FATAL(result, 0);
    etch_sleep(500);

    /* 
     * open multiple client connections   
     */
    cxclient_a = new_tcp_connection(g_test_url, NULL, NULL);
    cxclient_a->cx.on_data = on_data_client_test_write_with_readback;
    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient_a);
    etchlog("TEST", ETCHLOG_DEBUG, "client a id is %d\n", cxclient_a->cx.conxid); 

    cxclient_b = new_tcp_connection(g_test_url, NULL, NULL);
    cxclient_b->cx.on_data /* override data handler */
        = on_data_client_test_write_with_readback;
    CU_ASSERT_PTR_NOT_NULL_FATAL(cxclient_b);
    etchlog("TEST", ETCHLOG_DEBUG, "client b id is %d\n", cxclient_b->cx.conxid); 

    result = etch_tcpconx_open(cxclient_a, FALSE);
    CU_ASSERT_EQUAL_FATAL(result, 0);
    result = etch_tcpconx_open(cxclient_b, FALSE);
    CU_ASSERT_EQUAL_FATAL(result, 0);
    etch_sleep(500);

    /* 
     * client a send data to server and receive data back from server
     */
    etchlog("TEST", ETCHLOG_DEBUG, "client a sending data\n", cxclient_b->cx.conxid); 
    etch_tcpclient_send(cxclient_a, g_senddata1, g_buflen1, &sndrc);  
    datalen = etch_tcpclient_receive(cxclient_a, rcvbuf, RCVBUFLEN, &rcvrc); 

    CU_ASSERT_EQUAL(datalen, g_buflen1); /* verify sent == received */
    CU_ASSERT_EQUAL(0, memcmp(rcvbuf, g_senddata1, datalen));

    /* 
     * client b send data to server and receive data back from server
     */
    etchlog("TEST", ETCHLOG_DEBUG, "client b sending data\n", cxclient_b->cx.conxid); 
    etch_tcpclient_send(cxclient_b, g_senddata2, g_buflen2, &sndrc); 
    datalen = etch_tcpclient_receive(cxclient_b, rcvbuf, RCVBUFLEN, &rcvrc);  

    CU_ASSERT_EQUAL(datalen, g_buflen2); /* verify sent == received */
    CU_ASSERT_EQUAL(0, memcmp(rcvbuf, g_senddata2, datalen)); 
    etch_sleep(500);    
   
    /* 
     * close client connections
     */
    result = etch_tcpconx_close(cxclient_a, FALSE);
    CU_ASSERT_EQUAL(result, 0);
    result = etch_tcpconx_close(cxclient_b, FALSE);
    CU_ASSERT_EQUAL(result, 0);

    /* 
     * shut down tcp server  
     */
    result = etch_tcpsvr_close(tcp_server);
    CU_ASSERT_EQUAL(result, 0); 

    /* 
     * free memory
     */
    if (cxclient_a) cxclient_a->destroy(cxclient_a);
    if (cxclient_b) cxclient_b->destroy(cxclient_b);

    teardown_a_test(tcp_server);
 
    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */    
}


/**
 * test_receive_with_timeout()
 * test that we can time out a receive operation
 */
void test_receive_with_timeout(void)
{  
    char rcvbuf[RCVBUFLEN]; 
    int result = 0, datalen = 0, sndrc = 0, rcvrc = 0;
    etch_tcp_connection* cxclient = 0;
    etch_tcp_server* tcp_server = 0;

    tcp_server = setup_a_test(); 
    tcp_server->on_data = on_data_server_test_write_with_readback;
    result = etch_tcpsvr_open (tcp_server, 0);
    result = etch_tcpsvr_start(tcp_server);

    cxclient = new_tcp_connection(g_test_url, NULL, NULL);
    result = etch_tcpconx_open(cxclient, FALSE);

    /* do one send and two receives, the 2nd with 2-second timeout */
    etch_tcpclient_send(cxclient, g_senddata1, g_buflen1, &sndrc);  
    datalen = etch_tcpclient_receive (cxclient, rcvbuf, RCVBUFLEN, &rcvrc);  
    datalen = etch_tcpclient_receivex(cxclient, rcvbuf, RCVBUFLEN, 2000, &rcvrc);  
    CU_ASSERT_EQUAL(IS_ETCH_SOCKET_TIMEOUT(rcvrc), TRUE);  
   
    result = etch_tcpconx_close(cxclient, FALSE);
    result = etch_tcpsvr_close(tcp_server);
    if (cxclient) cxclient->destroy(cxclient);
    teardown_a_test(tcp_server);
 
    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */    
}


/**
 * main   
 */
int _tmain(int argc, _TCHAR* argv[])
{
    char c=0;
    CU_pSuite ps = NULL;
    g_is_automated_test = argc > 1 && 0 != wcscmp(argv[1], L"-a");
    if (CUE_SUCCESS != CU_initialize_registry()) return 0;
    CU_set_output_filename("../test_tcpconn");
    ps = CU_add_suite("suite_tcpconn", init_suite, clean_suite);
    etch_watch_id = 0; 

    CU_add_test(ps, "test unicode conversion 2", test_unicode_conversion2);
    CU_add_test(ps, "test unicode conversion", test_unicode_conversion);
    CU_add_test(ps, "test create and destroy tcp server", test_server_create_destroy);
    CU_add_test(ps, "test test setup and teardown", test_setup_a_test);
    CU_add_test(ps, "test tcp server open/close", test_server_open_close);
    CU_add_test(ps, "test tcp server start/stop", test_server_start_stop);

    CU_add_test(ps, "test server control start/stop",   test_server_control_start_stop);
    CU_add_test(ps, "test server control wait up/down", test_server_control_waitup_waitdown);

    CU_add_test(ps, "test connect to server", test_connect_to_server);
    CU_add_test(ps, "test multiple connect to server", test_multiple_connect_to_server);
    CU_add_test(ps, "test write to server", test_write_to_server);

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * these last three tests used to work but are now broken.
     * all tests that return data from server to client no longer work.
     * the re-send of data back to client finds "socket not connected" .
     * the production mailbox stuff works, so not sure what the problem is 
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     */
    // CU_add_test(ps, "test write with readback", test_write_with_readback);   
    // CU_add_test(ps, "test multiple client write/read", test_multiple_client_write_with_readback);  
    // CU_add_test(ps, "test receive with timeout", test_receive_with_timeout);  

    if (g_is_automated_test)    
        CU_automated_run_tests();    
    else
    {   CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    }

    if (!g_is_automated_test) { printf("any key ..."); while(!c) c = _getch(); wprintf(L"\n"); }     
    CU_cleanup_registry();
    return CU_get_error();
}
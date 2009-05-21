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
 * xmpl_perf_helper.c 
 * transport helper for perf service
 */

#include "etch_apr.h"  
#include "xmpl_perf_helper.h"
#include "xmpl_perf_valufact.h"
#include "xmpl_perf_client_impl.h"
#include "etch_svcobj_masks.h"
#include "etch_global.h"
#include "etch_url.h" 
#include "etchlog.h" 

#include <tchar.h>
#include <stdio.h>
#include <conio.h>
char* ETCHETCH = "ETCH";
char* ETCHTHIS = "PERF";
char* ETCHCVER = "etch C v0.5.000";


/**
 * new_remote_server()
 * this is java binding's helper.newServer(uri, rex, implfactory);
 * constructs a client side remote server per specs in the supplied uri.
 * generally invoked from a run thread in the client main .exe.  
 * RemotePerfServer server = PerfHelper.newServer( uri, null, implFactory );
 * @param uri 
 * @param p client parameters including callback to client ctor, 
 * caller reliquishes this memory.
 * @return the new remote server object.
 */
perf_remote_server* new_remote_server (wchar_t* uri, etch_client_factory* p)
{
    i_perf_client* myclient = NULL;
    perf_client_stub* client_stub = NULL;
    i_delivery_service* deliverysvc = NULL;
    perf_remote_server* remote_server = NULL;

    perf_value_factory* vf = new_perf_value_factory();
    ETCH_ASSERT(vf && p);
    p->in_valufact = (etch_value_factory*) vf;
    p->in_resx = etch_transport_resources_init(p->in_resx);
    ETCH_ASSERT(p->in_resx);
    etch_resources_add (p->in_resx, ETCH_RESXKEY_MSGIZER_VALUFACT, (objmask*) vf); 

    /* instantiate a delivery service */
    deliverysvc = new_etch_transport (uri, (etch_factory_params*) p, NULL);   
    ETCH_ASSERT(is_etch_ideliverysvc(deliverysvc));  /* TODO exception not assert */  
    p->dsvc = deliverysvc;                  

    /* instantiate the remote server */
    remote_server = new_perf_remote_server (NULL, deliverysvc, (etch_value_factory*) vf);

    ETCH_ASSERT(is_etch_remote_server(remote_server)); /* TODO return exception or return null */
    p->server_id = remote_server->server_base->server_id;
    p->server = remote_server;   
    remote_server->client_factory = p; /* assume ownership */

    /* here we call back to the client constructor in [main]. the purpose of the 
     * callback is to isolate the editable xxxx_client_impl constructor from the 
     * private constructor pieces. the callback instantiates a client implenentation
     * and returns an interface to it. 
     */   
    myclient = p->new_client? p->new_client (remote_server): NULL;

    ETCH_ASSERT(is_etch_client_base(myclient));  /* TODO return exception or return null */
    p->iclient = myclient;   

    p->fpool = (etch_threadpool*) etch_resources_get (p->in_resx, ETCH_RESXKEY_POOLTYPE_FREE); 
    p->qpool = (etch_threadpool*) etch_resources_get (p->in_resx, ETCH_RESXKEY_POOLTYPE_QUEUED); 
    ETCH_ASSERT(p->fpool && p->qpool);

    /* construct stub */
    client_stub = new_perf_client_stub (p);  

    ETCH_ASSERT(is_etch_client_stub(client_stub));  
    p->stub = client_stub;  

    return remote_server;
}


/**
 * new_perf_listener()  
 * invoked from [main] to construct the session listener.
 * this function is simply a proxy providing additional parameters 
 * to new_etch_listener, in order to simplify the call in [main].
 * @param main_new_server_callback pointer to [main]'s new_xxx_server() 
 * @return i_sessionlistener interface to listener. note that the returned 
 * listener.thisx is the specific server object, e.g. etch_tcp_server. 
 */
i_sessionlistener* new_perf_listener (wchar_t* uri, main_new_server_funcptr main_new_server)
{       
    i_sessionlistener* listener = NULL;
    etchlog (ETCHTHIS, ETCHLOG_DEBUG, "instantiating main listener ...\n");

    listener = new_etch_listener (uri, NULL, new_helper_accepted_server, 
        main_new_server, new_perf_resources);

    if (listener)
        etchlog (ETCHTHIS, ETCHLOG_DEBUG, "main listener instantiated\n");
    else
        etchlog (ETCHTHIS, ETCHLOG_ERROR, "could not instantiate main listener\n");

    return listener;
}


/**
 * start_perf_client()  
 * start the remote server, blocking until it comes up or errors out.
 * @param uri the etch-formatted URI.
 * @param new_client a pointer to a callback in the main client module
 * which constructs the client implementation object and returns an interface
 * to it. 
 * @param waitupms how long to wait for a connection to server to complete.
 * @return the remote server, or null indicating error.
 * todo return exception in remote server rather than null.
 */
perf_remote_server* start_perf_client (wchar_t* uri, 
    new_client_funcptr new_client, const int waitupms)
{
    int result = 0;
    perf_remote* remotebase = NULL;
    perf_remote_server*  remote = NULL;
    etch_client_factory* impl_factory = NULL;
    ETCH_ASSERT(new_client);
    etchlog_open_client();  /* force logger to use client filename */
    etchlog (ETCHTHIS, ETCHLOG_INFO, "creating perf client ...\n");

    impl_factory = new_client_factory (NULL, NULL, new_client);
    ETCH_ASSERT(impl_factory);

    /* instantiate a remote server, which invokes client constructor */
    etchlog (ETCHTHIS, ETCHLOG_DEBUG, "instantiating remote server ...\n");

    remote = new_remote_server (uri, impl_factory);

    ETCH_ASSERT(remote);  /* todo exception rather than assert */
    etchlog (ETCHTHIS, ETCHLOG_DEBUG,"remote server instantiated\n");
    etchlog (ETCHTHIS, ETCHLOG_INFO, "perf client created\n");
    remotebase = remote->remote_base;
    etchlog (ETCHTHIS, ETCHLOG_INFO, "starting perf client ...\n");

    /* fyi call sequence here is as follows:
     * etchremote_start_waitup()
     * etchremote_transport_control()
     * tcpdelsvc_transport_control()
     * pmboxmgr_transport_control()
     * msgzr_transport_control()
     * pktzr_transport_control()
     * tcpconx_transport_control()
     * tcpconx_start()
     * tcpconx_open()
     */

    /* BLOCK here until the connection to server comes up or times out */
    result = remotebase->start_waitup (remotebase, waitupms);  

    if (0 != result)
    {   /* todo return exception with actual error in remote, rather than null */
        etchlog (ETCHTHIS, ETCHLOG_ERROR, "could not start perf client\n");
        remote->destroy(remote);
        remote = NULL;
    } 
    else etchlog (ETCHTHIS, ETCHLOG_INFO, "perf client started\n");

    return remote;
}


/**
 * stop_perf_client()  
 * stop the remote server, blocking until it comes down or errors out.
 * @return 0 success, -1 failure
 * TODO return exception rather than result code.
 */
int stop_perf_client (perf_remote_server* remote, const int waitdownms)
{
    int result = 0;   
    etchlog (ETCHTHIS, ETCHLOG_INFO, "stopping perf client ...\n");

    /* fyi call sequence here is as follows:
     * etchremote_stop_waitdown()
     * etchremote_transport_control()
     * tcpdelsvc_transport_control()
     * pmboxmgr_transport_control()
     * msgizer_transport_control()
     * pktizer_transport_control()
     * tcpconx_transport_control()
     * tcpclient_stop_listener()
     * tcpconx_closex()
     */
    result = remote->remote_base->stop_waitdown (remote->remote_base, waitdownms);

    if  (0 == result)
         etchlog (ETCHTHIS, ETCHLOG_INFO, "perf client stopped\n");
    else etchlog (ETCHTHIS, ETCHLOG_ERROR, "could not stop perf client\n");

    return result;
}


/**
 * run_perf_listener()  
 * start the main (accept) listener, block until listener exit,
 * and finally tear down this server's outstanding client sessions.
 * @return 0 success, -1 failure.
 */
int run_perf_listener (i_sessionlistener* listener, const int waitupms)
{
    int result = 0;
    if (NULL == listener) return -1;
    etchlog (ETCHTHIS, ETCHLOG_INFO, "starting main listener ...\n");

    /* call through to e.g. etch_tcpsvr_transport_control() to start listener */ 
    result = listener->transport_control (listener->thisx, 
             new_etch_event(CLASSID_CONTROL_START_WAITUP, waitupms), NULL);

    if (0 == result)
    {   etchlog (ETCHTHIS, ETCHLOG_INFO, "main listener started on thread %d\n",
                 transport_thread_id (listener));
        etchlog (ETCHTHIS, ETCHLOG_DEBUG, "listening for connect requests ...\n");

        /* BLOCK here until listener thread exits (etch_listener_waitfor_exit) */
        result = listener->wait_exit (listener); 

        etchlog (ETCHTHIS, ETCHLOG_INFO, "main listener ended\n");

        if (transport_session_count (listener) > 0)
        {
            etchlog (ETCHTHIS, ETCHLOG_DEBUG, "begin client sessions teardown\n");

            result = transport_teardown_client_sessions (listener);

            etchlog (ETCHTHIS, ETCHLOG_DEBUG, "end client sessions teardown\n");
        }
    }
    else
        etchlog (ETCHTHIS, ETCHLOG_ERROR, "could not start main listener\n");

    return result;
}


/**
 * new_perf_resources()  
 * callback from new_etch_listener to initialize service-specific resources.
 * @param p the etch_server_factory to be initialized
 * @return 0 success, -1 failure.
 */
int new_perf_resources (etch_server_factory* p)
{
    int result = 0;
    ETCH_ASSERT((NULL != p) && (NULL == p->in_valufact));
    ETCH_ASSERT (p->in_resx && is_etch_hashtable(p->in_resx));

    p->in_valufact = (etch_value_factory*) new_perf_value_factory ();
    ETCH_ASSERT(p->in_valufact);
    if (NULL == p->in_valufact) return -1;

    result = etch_resources_add (p->in_resx, 
        ETCH_RESXKEY_MSGIZER_VALUFACT, (objmask*) p->in_valufact); 

    ETCH_ASSERT(0 == result);
    return result;
}


/**
 * new_helper_accepted_server()  
 * this is java binding's newServer().  
 * constructs the server side listener component of a remote client.
 * invoked from the server's accepted connection handler, such as
 * tcpxfact_session_accepted(), to create the remote client and stub 
 * with which to communicate with the individual client.
 * we see then that each client has its own listener thread on the server,
 * as opposed to a client ID scheme where all clients would filter through
 * a single socket on the server.
 * @param p parameter bundle, including the callback to the to the service-
 * specific new server constructors in [main]. caller relinquishes.
 * @return the client's service-specific server stub.
 */
void* new_helper_accepted_server (etch_server_factory* p, etch_session* session)    
{
    i_perf_server* iserver;
    perf_server_stub* stub;
    perf_remote_client* client;
    ETCH_ASSERT(p && p->helper_new_accepted && p->main_new_server);
    ETCH_ASSERT(p->in_resx && p->in_valufact);   // TODO assert delivery service
    etchlog (ETCHTHIS, ETCHLOG_DEBUG, "instantiating accepted client listener ...\n");

    etchlog (ETCHTHIS, ETCHLOG_DEBUG, "creating remote client...\n");
    client = new_remote_client (NULL, session, p->in_valufact);
    client->session_id = session->session_id;  
    session->client = client;   

    /* here we CALL BACK to the constructor in [main], the purpose of the callback
     * being to isolate the editable constructor from the private constructor. 
     * the callback instantiates a client's server implementation and returns 
     * an interface to it. 
     */ 
    etchlog (ETCHTHIS, ETCHLOG_DEBUG, "creating server implementation ...\n");
    iserver = p->main_new_server (p, session);    
    iserver->session_id = session->session_id; 
    session->server = iserver;  
                   
    /* note that the main listener will use p->mainpool as a thread manager, not these */                          
    p->qpool = (etch_threadpool*) etch_resources_get(p->in_resx, ETCH_RESXKEY_POOLTYPE_QUEUED); 
    p->fpool = (etch_threadpool*) etch_resources_get(p->in_resx, ETCH_RESXKEY_POOLTYPE_FREE);  

    /* eventually new_perf_server_stub() gets to stub_base constructor, which sets
     * the delivery service's session to this, the server stub. so, in the java binding,
     * the server stub is referenced as delivery service.session. we should perhaps also 
     * store the stub opaquely in both the client and listener objects.
     */
    etchlog (ETCHTHIS, ETCHLOG_DEBUG, "creating server stub ...\n");
    stub = new_perf_server_stub (p, session);
    stub->session_id = session->session_id;
    session->server_stub = stub;  

    if (iserver && stub)
        etchlog (ETCHTHIS, ETCHLOG_DEBUG, "accepted client listener instantiated\n");
    else
        etchlog (ETCHTHIS, ETCHLOG_ERROR, "could not instantiate accepted client listener\n");

    return stub;  
}


/**
 * get_perf_client_impl()
 * convenience to extract the client impl from the remote server object.
 */
perf_client_impl* get_perf_client_impl (perf_remote_server* rs)
{
    i_perf_client* iclient = rs->client_factory->iclient;
    perf_client_impl* pci  = iclient? iclient->thisx: NULL;
    return pci;
}


/**
 * get_perf_client_stub()
 * convenience to extract the client stub from the remote server object.
 */
perf_client_stub* get_perf_client_stub (perf_remote_server* rs)
{
    perf_client_stub* stub = (perf_client_stub*) rs->client_factory->stub;
    ETCH_ASSERT(is_perf_client_stub(stub));
    return stub;
}


/**
 * get_perf_client_stubbase()
 * convenience to extract the client stub base from the remote server object.
 */
etch_stub* get_perf_client_stubbase (perf_remote_server* rs)
{
    perf_client_stub* stub = get_perf_client_stub (rs);
    etch_stub* base = stub->stub_base;
    ETCH_ASSERT(is_etch_stub(base));
    return base;
}


/**
 * perf_client_cleanup()
 * destroy instantiations from [main]
 */
void perf_client_cleanup (perf_remote_server* remote)
{
    etchlog (ETCHTHIS, ETCHLOG_DEBUG, "destroying remote server ...\n");
    ETCHOBJ_DESTROY(remote);
    etchlog (ETCHTHIS, ETCHLOG_DEBUG, "remote server destroyed\n");
}

 
/**
 * etch_init()
 * do client-side etch runtime initialization and service-specific initialization. 
 * this is intended as a convenience function to be invoked only by user client code.
 */
int etch_init()
{
    return perf_runtime_init(INIT_ETCH_CLIENT);
}


/**
 * etch_exit()
 * etch runtime teardown plus service-specific teardown 
 * this is intended as a convenience function to be invoked only by user client code.
 */
int etch_exit()
{
    return perf_runtime_exit();
}


/**
 * perf_runtime_exit()
 * do etch runtime teardown plus service-specific teardown. 
 */
int perf_runtime_exit ()
{    
    etchvf_free_builtins(); /* TODO move this somewhere else */
    exitparams.is_show_leak_detail = config.is_log_memory_leak_detail; 
    etch_runtime_exit (&exitparams);
    printf("\n%s exit\n", ETCHCVER);
    return 0;
}

 
/**
 * perf_runtime_init()
 * do etch runtime initialization plus service-specific initialization. 
 */
int perf_runtime_init (const int is_client)
{
    printf("%s %s start\n", ETCHCVER, is_client? "client": "server");

    return etch_runtime_init_all (is_client);
}


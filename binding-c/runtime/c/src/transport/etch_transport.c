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
 * etch_transport.c 
 * common transport functionality
 */

#include "etchthread.h" /* include first */
#include "etch_plainmboxmgr.h"
#include "etch_svcobj_masks.h"
#include "etch_plainmailbox.h"
#include "etch_transport.h"
#include "etch_packetizer.h"
#include "etch_messagizer.h"
#include "etch_tcpserver.h"
#include "etch_tcpconxn.h"
#include "etch_global.h"
#include "etchexcp.h"
#include "etchlog.h"
char* ETCHTRAN = "TRAN";

int tcpdelsvc_init (etch_tcp_delivery_service*);
int destroy_delivery_service_interface (i_delivery_service*);
int destroy_delivery_service_via_interface(i_delivery_service*);
int destroy_tcp_delivery_service(etch_tcp_delivery_service*);
int tcpdelsvc_begincall(i_delivery_service*, etch_message*, void**);
int tcpdelsvc_endcall  (i_delivery_service*, i_mailbox*, etch_type*, void**);
int tcpdelsvc_session_message (i_delivery_service*, etch_who*, etch_message*);
int tcpdelsvc_session_control (i_delivery_service*, etch_event*, objmask*);
int tcpdelsvc_session_notify  (i_delivery_service*, etch_event*);
objmask* tcpdelsvc_session_query (i_delivery_service*, objmask*);
int tcpdelsvc_transport_message(i_delivery_service*, etch_who*, etch_message*);
i_sessionmessage* tcpdelsvc_get_session(i_delivery_service*);
void tcpdelsvc_set_session(i_delivery_service*, i_sessionmessage*) ;
objmask* tcpdelsvc_transport_query (i_delivery_service*, etch_query*); 
int tcpdelsvc_transport_control(i_delivery_service*, etch_event*, objmask*);
int tcpdelsvc_transport_notify (i_delivery_service*, etch_event*);


/* - - - - - - - - - - - - - - - - - - - - - - 
 * delivery service
 * - - - - - - - - - - - - - - - - - - - - - -
 */

/*
 * new_etch_transport()
 * etch_delivery_service constructor.
 * @remarks this is the transport factory, implemented via a switch. 
 * @param uri a uri string, caller relinquishes.
 * @param params a transport parameter bundle, caller retains.
 * @param conximpl optional pre-existing connection implementation, such as tcp_connection*.
 * @return a delivery service interface. note that invoking the destructor on this interface
 * destroys the delivery service implementation, as well as the interface.
 */
i_delivery_service* new_etch_transport (wchar_t* uri, etch_factory_params* params, void* conximpl)
{
    etch_url* url = new_url(uri); 

    i_delivery_service* newds = new_etch_transport_a (url, params, conximpl);   
    
    url->destroy(url);
    return newds;    
}


/*
 * new_etch_transport_a()
 * etch_delivery_service constructor.
 * @remarks this is the transport factory, implemented via a switch. 
 * @param url an etch_url, caller retains.
 * @param resources a resources map, caller retains.
 * @return a delivery service interface. invoking the destructor on this interface
 * destroys the delivery service implementation, as well as the interface.
 */
i_delivery_service* new_etch_transport_a (etch_url* url, etch_factory_params* params, void* conximpl)
{
    i_delivery_service* newds = NULL;

    if (is_url_scheme_udp(url))
    {    
        /* not yet implemented */
    }
    #if(0)
    else   /* handlers for other url schemes follow here eventually */  
    if (is_url_scheme_foo(url))
    {
        /* ... */
    }
    #endif
    else
    {   /* url schemes http, tcp, default */
        etch_tcp_delivery_service* tcpds = new_tcp_delivery_service (url, params, conximpl); 
 
        if (tcpds)
        {   newds = tcpds->ids; 
            newds->thisx = tcpds;
        } 
    }
    
    return newds;    
}


/*
 * new_delivery_service()
 */
objmask* new_delivery_service (const int objsize, const unsigned short class_id)
{
    return new_object (objsize, ETCHTYPEB_DELIVERYSVC_IMPL, class_id);
}    


/*
 * new_tcp_delivery_service()
 * etch_tcp_delivery_service constructor 
 * replaces java TcpTransportFactory.newTransport
 * @param params server parameter bundle, caller retains. 
 * &param tcpx if present, the already accepted client connection. 
 * if present, caller retains.
 */
etch_tcp_delivery_service* new_tcp_delivery_service (etch_url* url, 
    etch_factory_params* params, etch_tcp_connection* tcpconx)
{
    etch_resources* resources = NULL;
    etch_packetizer* packetizer = NULL;
    etch_messagizer* messagizer = NULL;
    etch_mailbox_manager* mboxmgr = NULL;
    etch_tcp_delivery_service* delsvc = NULL;  
    const int is_tcpconx_owned = tcpconx == NULL;  
    ETCH_ASSERT(params && params->in_resx); 
    resources = params->in_resx;

    do
    {   /* as each next higher layer of the delivery service is instantiated, it
         * is passed passed a transport interface to the previously-instantiated
         * layer. in each such case, note that the new layer does not own memory 
         * for the passed transport interface. 
         */
        if (NULL == tcpconx)  
            tcpconx = new_tcp_connection (url, params->in_resx, NULL);  
        
        ETCH_ASSERT(tcpconx);
        if (0 != init_etch_tcpconx_interfaces (tcpconx)) break;  
         
        packetizer = new_packetizer_a (tcpconx->itd, url, resources); 
        if (NULL == packetizer) break;
      
        messagizer = new_messagizer_a (packetizer->transportpkt, url, resources);
        if (NULL == messagizer) break;

        mboxmgr = new_plain_mailbox_manager (messagizer->transportmsg, 
            url->raw, resources, params->mblock);
        if (NULL == mboxmgr) break;

        delsvc = (etch_tcp_delivery_service*) new_delivery_service 
          (sizeof(etch_tcp_delivery_service), CLASSID_TCP_DELIVERYSVC);

        delsvc->destroy = destroy_tcp_delivery_service;

        /* set our transport to that of the next lower layer (mailbox manager) */
        delsvc->transport  = mboxmgr->transportmsg;  
        delsvc->transportx = mboxmgr->imanager;  /* todo can we lose this ref? */     

        delsvc->mailboxmgr = mboxmgr;
        delsvc->tcpconx    = tcpconx;
        delsvc->waiter     = tcpconx->cx.waiter; /* connection up/down monitor */
        delsvc->rwlock     = params->mblock;     /* not owned */
        delsvc->packetizer = packetizer;        /* todo can we lose these refs */
        delsvc->messagizer = messagizer;
        delsvc->resources  = resources;
        delsvc->is_tcpconx_owned = is_tcpconx_owned;

        tcpdelsvc_init (delsvc);  /* initialize the delivery service interface */

    } while(0);

    if (NULL == delsvc)
    {
        ETCHOBJ_DESTROY(tcpconx);
        ETCHOBJ_DESTROY(packetizer);
        ETCHOBJ_DESTROY(messagizer);
        ETCHOBJ_DESTROY(mboxmgr);
    }

    return delsvc;
}


/**
 * tcpdelsvc_init()
 * initialize delivery service interface
 */
int tcpdelsvc_init (etch_tcp_delivery_service* delsvc)
{   
    i_sessionmessage*   ism = NULL;
    i_transportmessage* itm = NULL;
    i_session*   isession   = NULL;
    i_transport* itransport = NULL;

    i_delivery_service* ids = new_delivery_service_interface(NULL, NULL);
    delsvc->ids = ids;

    /* external transport and session */
    ids->transport  = delsvc->transport;
    ids->session    = delsvc->session;

    ids->begin_call = delsvc->begin_call = tcpdelsvc_begincall;
    ids->end_call   = delsvc->end_call   = tcpdelsvc_endcall;


    /* - - - - - - - - - - - - - - -
     * i_transportmessage (native)
     * - - - - - - - - - - - - - - -
     */
    itransport = new_transport_interface (ids,
        tcpdelsvc_transport_control,
        tcpdelsvc_transport_notify,
        tcpdelsvc_transport_query);

    delsvc->transportmsg = new_transportmsg_interface (ids, 
        tcpdelsvc_transport_message, 
        itransport); /* transportmsg now owns itransport */

    delsvc->transportmsg->set_session = tcpdelsvc_set_session;
    delsvc->transportmsg->get_session = tcpdelsvc_get_session;

    /* copy native transport back to interface */
    ids->itm = delsvc->transportmsg;

    /* copy i_transportmsg interface methods up to this object */
    delsvc->transport_message = delsvc->transportmsg->transport_message;
    delsvc->transport_control = itransport->transport_control;
    delsvc->transport_notify  = itransport->transport_notify;
    delsvc->transport_query   = itransport->transport_query;
    delsvc->set_session       = itransport->set_session;
    delsvc->get_session       = itransport->get_session;
  

    /* - - - - - - - - - - - - - - -
     * i_sessionmessage (native)
     * - - - - - - - - - - - - - - -
     */
    isession = new_session_interface (ids, 
        tcpdelsvc_session_control,
        tcpdelsvc_session_notify,
        tcpdelsvc_session_query);

    delsvc->sessionmsg  = new_sessionmsg_interface (ids, 
        tcpdelsvc_session_message, 
        isession); /* sessionmsg now owns isession */

    /* copy native session back to interface */
    ids->ism = delsvc->sessionmsg;

    /* copy i_sessionmessage interface methods up to this object */
    delsvc->session_message = delsvc->sessionmsg->session_message;
    delsvc->session_control = isession->session_control;
    delsvc->session_notify  = isession->session_notify;
    delsvc->session_query   = isession->session_query;

    /* finally set session of next lower layer (messagizer) to our session */
    delsvc->transport->set_session (delsvc->transport->thisx, delsvc->sessionmsg);   

    return 0;
}


/**
 * new_delivery_service_interface()
 * delivery service interface constructor
 */
i_delivery_service* new_delivery_service_interface
    (i_sessionmessage* isessionmsg, i_transportmessage* itransportmsg)
{
    i_delivery_service* ids = (i_delivery_service*) new_object(sizeof(i_delivery_service), 
        ETCHTYPEB_DELIVERYSVCINT, CLASSID_DELIVERYSVC);

    /* this destructor destroys the parent implementation, 
     * which in turn destroys this interface object */
    ids->destroy = destroy_delivery_service_via_interface;

    ids->ism = isessionmsg;
    ids->itm = itransportmsg;

    return ids;
}


/**
 * destroy_delivery_service_stub()
 * invoked by i_deliveryservice destructor to destroy the stub via the session
 * shared by the stub and delivery service, and finally destroy the session.
 */
int destroy_delivery_service_stub (i_delivery_service* ids)
{
    /* 1. the stub constructor created a session interface and set the 
     * delivery service's session to that session, and set the delivery
     * service's is_session_owned true.
     * 2. the stub and delivery service therefore share the session interface.
     * 3. the delivery service implementation object (e.g. tcp delivery service)
     * also references that session, but of course does not own it. 
     * 4. the session interface's thisx pointer is the stub object.
     * 5. the delivery service owns the stub by contract.
     * the delivery service implementation object's destructor invokes the
     * delivery service interface destructor to destroy the session and stub.
     * 6. the delivery service interface destructor destroys the stub by invoking 
     * this method, which finds the stub via the thisx of the shared session. 
     */
    xxxx_either_stub* stubobj = NULL;   
    if (!ids->session || !ids->is_session_owned) return -1;
    
    stubobj = ids->session->thisx;  /* stub is the session interface's thisx */
    if (is_etch_stub(stubobj)) 
        stubobj->destroy(stubobj);
    else  /* is there a use case for this */
    {   assert(is_etch_sessionmsg(ids->session));  
        ids->session->destroy(ids->session);
    }

    ids->session = NULL;  
    return 0;
}


/**
 * destroy_delivery_service_interface()
 * i_delivery_service destructor 1.
 */
int destroy_delivery_service_interface(i_delivery_service* ids)
{
    if (NULL == ids) return -1;
    if (ids->refcount > 0 && --ids->refcount > 0) return -1;  

    if (!is_etchobj_static_content(ids))
    {   
        /* stub owns the ism, ds owns the stub: destroy stub and session */
        destroy_delivery_service_stub(ids);  

        /* stub does not own the itm: itm is that of the mailbox manager */
        if (ids->itm && ids->is_transport_owned)
        {   ids->itm->destroy(ids->itm);
            ids->itm = NULL;
        }
    }
            
    return destroy_objectex((objmask*)ids);
}


/*
 * destroy_delivery_service_via_interface()
 * i_delivery_service destructor 2.
 * this destructor will destroy the parent delivery service implementation, whose
 * destructor will in turn destroy this interface object. this permits the object 
 * creating the delivery service to hold a reference to the interface only, and to
 * destroy the implementation and interface by invoking the interface's destructor.
 */
int destroy_delivery_service_via_interface(i_delivery_service* ids)
{
    objmask* deliveryservice_implobj = ids? ids->thisx: NULL;

    if (deliveryservice_implobj) /* etch_tcp_delivery_service, e.g. */
        deliveryservice_implobj->destroy (deliveryservice_implobj);

    return 0;
}
 

/**
 * destroy_tcp_delivery_service()
 * etch_tcp_delivery_service destructor
 */
int destroy_tcp_delivery_service (etch_tcp_delivery_service* thisx)
{
    const char* thistext = "delsvc dtor";
    if (NULL == thisx) return -1;
    if (thisx->refcount > 0 && --thisx->refcount > 0) return -1;  

    if (!is_etchobj_static_content(thisx))
    {         
        /* ensure any threads referencing mailboxes (see mailbox.message())
         * have run to completion before we start tearing it down. */                 
        etchmbox_get_readlockex (thisx->rwlock, thistext);
        etchmbox_release_readlockex (thisx->rwlock, thistext);

        etchlog (ETCHTRAN, ETCHLOG_DEBUG, "destroying packetizer ...\n");
        ETCHOBJ_DESTROY(((etch_packetizer*) thisx->packetizer));

        etchlog (ETCHTRAN, ETCHLOG_DEBUG, "destroying messagizer ...\n");
        ETCHOBJ_DESTROY(((etch_messagizer*) thisx->messagizer));

        etchlog (ETCHTRAN, ETCHLOG_DEBUG, "destroying mailbox manager ...\n");
        ETCHOBJ_DESTROY(((etch_mailbox_manager*) thisx->mailboxmgr));

        /* on server side, listen thread destroys tcpconx on exit. 
         * on client side, tcpconx is destroyed here. */
        if (thisx->is_tcpconx_owned)
            ETCHOBJ_DESTROY(thisx->tcpconx); 

        etchlog (ETCHTRAN, ETCHLOG_DEBUG, "destroying delivery interface ...\n");
        destroy_delivery_service_interface(thisx->ids);

        ETCHOBJ_DESTROY(thisx->sessionmsg);  
        ETCHOBJ_DESTROY(thisx->transportmsg);
    }
            
    return destroy_objectex((objmask*)thisx);
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * i_deliveryservice (i_sessionmessage, i_transportmessage)
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * get_etch_ds_impl()
 * convenience method to verify i_delivery_service, and from it, 
 * get, verify, and return the delivery service implementation object.
 */
etch_tcp_delivery_service* get_etch_ds_impl (i_delivery_service* ids) 
{
    etch_tcp_delivery_service* tcpds = NULL;
    ETCH_ASSERT(is_etch_ideliverysvc(ids));
    tcpds = ids->thisx;
    ETCH_ASSERT(is_etch_deliverysvc(tcpds));
    return tcpds;
}


 /**
 * tcpdelsvc_begincall()
 * i_deliveryservice :: begincall
 * @param msg caller relinquishes on success, retains on failure
 * @param out mailbox interface returned on success
 * @return 0 success, or -1 failure. new mailbox return in out parameter.
 */
int tcpdelsvc_begincall (i_delivery_service* ids, etch_message* msg, i_mailbox** out)
{
    int result = 0;
    etch_tcp_delivery_service* tcpds = get_etch_ds_impl(ids);
    i_transportmessage* dstransport  = tcpds->transport;
    ETCH_ASSERT(is_etch_transportmsg(dstransport));

    /* transport is mailbox mgr pmboxmgr_transport_call(imbmgr) */
    result = tcpds->transportx->transport_call (tcpds->transportx, NULL, msg, out);

    return result;
}


/**
 * tcpdelsvc_endcall() 
 * read the response message, close its mailbox and return the result object.
 * @param mbox the current mailbox (interface), caller retains.
 * @param response_type type of the response message, caller retains.
 * @param out pointer to caller's location to receive the message response object.
 * @remarks if some exception condition occurred, the result object will not be an
 * object of the expected result type, but rather will be an etch_mailbox_element
 * object containing an exception. therefore, the result object should be tested
 * with is_exception(resultobj) prior to expecting it to be of the expected type. 
 * @return 0 success, -1 failure. a result object is returned via out parameter.
 * the result object is the expected object type of the service function result,
 * or if a response could not be read, the etch_mailbox_element object wrapping
 * both the reply message object, and an exception object detailing the problem. 
 * for example, if the service message is etch_int32* add(etch_int32*, etch_int32*),
 * the result object will be an etch_int32 unless an exception occurred. 
 */
int tcpdelsvc_endcall (i_delivery_service* ids, i_mailbox* ibox, 
    etch_type* response_type, objmask** out)
{
    int  result = 0, timeout = 0;
    objmask* result_obj = NULL;
    etch_mailbox_element* mbe = NULL;
    const char* thistext = "tcpdelsvc_endcall";
    etch_tcp_delivery_service* tcpds = get_etch_ds_impl(ids);
    /* get the response message type's instance data */
    etch_type_impl* typeinfo = response_type? (etch_type_impl*) response_type->impl: NULL;
    ETCH_ASSERT(typeinfo && out);
    /* we do not default to wait forever in order that we can fail gracefully rather
     * than hang the mailbox read. this behavior is essential for comprehensive testing. 
     * if a type specifies a timeout of zero, we do not interpret this as ETCH_INFWAIT, 
     * but rather we substitute a default timeout, which is configurable. 
     * if a message type actually requires an abnormally lengthy wait, it will specify  
     * some presumably large number of milliseconds as its timeout value. similarly,
     * if we actually want to wait "forever" by default, we can configure a very large 
     * timeout value which is effectively "forever".
     */                                                   
    timeout = typeinfo->timeout > 0? typeinfo->timeout: config.default_mailbox_read_waitms;
    etchlog (ETCHTRAN, ETCHLOG_XDEBUG, "reading mailbox (wait %d) ...\n", timeout);

    /* read reply message from mailbox, waiting for message to arrive if necessary */
    result = ibox->read_withwait (ibox, timeout, &mbe);
    // etchlog (ETCHTRAN, ETCHLOG_XDEBUG, "mailbox read result was %d\n", result);
    
    ETCH_ASSERT(mbe); /* mailbox read guarantees a result which may host exception */

    if (is_exception(mbe)) /* mailbox read timed out or otherwise failed */
    {   /* here we are returning an object of a type not expected by the service.
         * this method's contract will specify that the app must test the returned
         * object for an exception prior to "casting" it to the expected result 
         * object type. another possibility is that we could have default  
         * constructors for each response type.  
         */
        *out = (objmask*) mbe;
    }
    else /* find the result object expected in the reply message */  
    if (NULL == (result_obj = message_get (mbe->msg, typeinfo->response_field)))
    {   
        /* see comment above re returning an object of a type not expected by service */
        etch_throw(mbe, EXCPTYPE_ETCHRUNTIME, L"message result missing", ETCHEXCP_COPYTEXT);
        etchlog (ETCHTRAN, ETCHLOG_ERROR, "message result missing\n", timeout);
        *out = (objmask*) mbe;
    }
    else 
    {  /* we found the reply message result object. return this result object, 
        * and destroy the message wrapper and the reply message along with it. 
        * note that we must be careful destroying the message, since the message
        * result object, that we intend to return to the application, is part of 
        * the message and thus destroyed with the message unless steps are taken 
        * to protect it. we could clone the result and return the clone, but
        * that would presuppose that the object is cloneable, so instead we will 
        * protect the object, destroy the message, and finally unprotect the object.
        */
       set_etchobj_static_all(result_obj);   /* protect result object */
       mbe->destroy(mbe);                    /* destroy message and wrapper */
       clear_etchobj_static_all(result_obj); /* unprotect result object */
       *out = result_obj;                    /* return result object */
       // etchlog (ETCHTRAN, ETCHLOG_XDEBUG, "mailbox result object type %d class %d\n", 
       //          result_obj->obj_type, result_obj->class_id);
       result = 0;
    }

    /* acquire mailbox read write lock to switch context back to the receive thread,
     * which will continue at the return from the queue.put in mailbox.message().
     * we have passed in a dedicated mutex in start_xxxx_client, intended to be used
     * by client main thread and receive thread as a read/write lock. there should
     * be a cleaner way to sync mailbox reads and writes from within the mailbox
     * itself, we need to investigate. ideally we would sync all of mailbox.message()
     * using its queue waiter's mutex; however at present all such waiter mutexes are
     * non-recursive, and so we would need to do extensive re-testing if we were to
     * change waiter mutexes to be nestable, assuming they would work at all that way.
     * we should make this change asap however, since the way it is now, we don't wait
     * for the mailbox write to do its fire_notify() before we read the mailbox.
     * however the following lock at least ensures the fire_notify() happens prior to  
     * closing the mailbox, which is the overriding consideration.
     */
    etchmbox_get_readlock (ibox->thisx, thistext);
    etchmbox_release_readlock (ibox->thisx, thistext);

    /* we're now done with the the mailbox so close it. */
    ibox->close_read (ibox);  
    return result;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - 
 * i_deliveryservice :: i_sessionmessage (i_session)
 * - - - - - - - - - - - - - - - - - - - - - - - - - 
 */

/* this is the delivery service interface implementation of i_sessionmessage,
 * distinct from the transport.session's implementation of i_sessionmessage
 * which is implemented externally and set via set_session().
 */

/**
 * tcpdelsvc_session_message()
 * @param whofrom caller retains, can be null.
 * @param msg caller relinquishes
 * @return 0 (message handled), or -1 (error, closed, or timeout)  
 */
int tcpdelsvc_session_message (i_delivery_service* ids, etch_who* whofrom, etch_message* msg)
{
    etch_tcp_delivery_service* tcpds = get_etch_ds_impl(ids);
    i_sessionmessage* dssession = tcpds->session;
    ETCH_ASSERT(is_etch_sessionmsg(dssession));

    return dssession->session_message (dssession->thisx, whofrom, msg);
}


/**
 * tcpdelsvc_session_control()
 * delivery service interface implementation of i_session_message  
 * @param control event, caller relinquishes.
 * @param value control value, caller relinquishes.
 */
int tcpdelsvc_session_control (i_delivery_service* ids, etch_event* control, objmask* value)
{
    etch_tcp_delivery_service* tcpds = get_etch_ds_impl(ids);
    i_sessionmessage* dssession = tcpds->session;
    ETCH_ASSERT(is_etch_sessionmsg(dssession));

    return dssession->session_control(dssession->thisx, control, value);
}


/**
 * etch_tcpdelsvc_session_notify()
 * @param evt event, caller relinquishes.
 */
int tcpdelsvc_session_notify (i_delivery_service* ids, etch_event* evt)
{
    int result = -1, evtype = evt? evt->value: 0;
    etch_tcp_delivery_service* tcpds = get_etch_ds_impl(ids);
    i_sessionmessage* dssession = tcpds->session;
    ETCH_ASSERT(is_etch_sessionmsg(dssession));

    if (tcpds->waiter) 
    {   switch(evtype)
        {  case ETCHEVT_SESSION_UP: 
           case ETCHEVT_SESSION_DOWN: /* first UNBLOCK waiting threads ... */
                tcpds->waiter->set (tcpds->waiter, evtype);  
        }
    }

    result = dssession->session_notify (dssession->thisx, evt);
    return result;
}


/**
 * etch_tcpdelsvc_session_query()
 * @param query, caller relinquishes.
 */
objmask* tcpdelsvc_session_query (i_delivery_service* ids, objmask* query) 
{
    etch_tcp_delivery_service* tcpds = get_etch_ds_impl(ids);
    i_sessionmessage* dssession = tcpds->session;
    ETCH_ASSERT(is_etch_sessionmsg(dssession));

    return dssession->session_query (dssession->thisx, query);
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * i_deliveryservice :: i_transportmessage (i_transport)
 * - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * tcpdelsvc_transport_message()
 * @param whoto recipient - caller retains this memory, can be null.
 * @param message the message
 * caller relinquishes this memory on success, retains on failure.  
 * @return 0 success, -1 error.
 */
int tcpdelsvc_transport_message (i_delivery_service* ids, etch_who* whoto, etch_message* msg)
{  
    etch_tcp_delivery_service* tcpds = get_etch_ds_impl(ids);
    i_transportmessage* dstransport  = tcpds->transport;
    ETCH_ASSERT(is_etch_transportmsg(dstransport));

    return dstransport->transport_message (dstransport->thisx, whoto, msg);
}


/**
 * tcpdelsvc_get_session()
 * @return a reference to the delivery service i_sessionmessage interface.
 * caller does not own this object.
 */
i_sessionmessage* tcpdelsvc_get_session (i_delivery_service* ids) 
{
    etch_tcp_delivery_service* tcpds = get_etch_ds_impl(ids);

    return tcpds->session;
}


/**
 * tcpdelsvc_set_session()
 * @param session the i_sessionmessage interface. caller retains ownership.
 * this is generally called from the stub constructor.
 */
void tcpdelsvc_set_session (i_delivery_service* ids, i_sessionmessage* session) 
{
    etch_tcp_delivery_service* tcpds = get_etch_ds_impl(ids);
    ETCH_ASSERT(is_etch_sessionmsg(session));
    /* set delivery service session to be the passed (stub's) session */
    tcpds->session = tcpds->ids->ism = session;
}


/**
 * tcpdelsvc_transport_control()
 * @param control, caller relinquishes.
 * @param value control value, caller relinquishes.
 * @remarks as it currently stands, the value object passed through these transport
 * interfaces must be a cloneable object, either through being cloneable by default,
 * such as the etch_primitive or etch_object derivatives (for example, etch_int32, 
 * etch_string, etch_date, etch_event, etch_object, and others); or by virtue of
 * having custom clone() functions assigned to them.
 */
int tcpdelsvc_transport_control (i_delivery_service* ids, etch_event* control, objmask* valobj)
{
    int  result = 0;    
    etch_connection* cx = NULL;
    i_transportmessage* dstransport = NULL;
    etch_tcp_delivery_service* tcpds = NULL;
    const int objclass  = control? control->class_id: 0;
    const int timeoutms = control? control->value: 0;
    ETCH_ASSERT(is_etch_ideliverysvc(ids) && objclass);

    tcpds = get_etch_ds_impl(ids);  /* delivery service implementation */
    ETCH_ASSERT(is_etch_deliverysvc(tcpds));
    dstransport = tcpds->transport; /* delivery service transport (mailbox mgr) */
    ETCH_ASSERT(is_etch_transportmsg(dstransport));
    cx = &tcpds->tcpconx->cx;       /* underlying connection */

    switch(objclass)  /* forward the transport event */
    {           
       case CLASSID_CONTROL_START_WAITUP:  
            /* point to the condition variable on the waiter. the need to do this 
             * is a semikludge; that is, having to set some state prior to calling
             * waitup; however we can't have the function call preset the state
             * because the absence of the state variable means "not waiting", and
             * is tested for by the wait function. so we need to have a target for
             * the "up" state before we do the waitup, since the connect will complete
             * before we get around to asking for the waitup, and it needs to be able
             * to mark state as up, thus we set that target below. previously the state 
             * variable cond_var was not set until the wait_up was invoked. in the 
             * current design the cond_var is nulled out after a wait, in order to 
             * reset wait state to not waiting, so we need to ensure it is populated 
             * in advance of any need to set a wait condition to some state, prior to 
             * actually waiting.  
             */
            etchconx_init_waitstate (cx);  /* see comment above */
            control->class_id = CLASSID_CONTROL_START; /* modify event to not wait */

            result = dstransport->transport_control (dstransport->thisx, control, valobj);

            if (0 == result)     
                result = etchconx_wait_up (cx, timeoutms);

            break; 

       case CLASSID_CONTROL_STOP_WAITDOWN: 
            etchconx_init_waitstate (cx); /* see comment above */
            control->class_id = CLASSID_CONTROL_STOP; /* modify event to not wait */ 

            result = dstransport->transport_control (dstransport->thisx, control, valobj);

            if (0 == result)     
                result = etchconx_wait_down (cx, timeoutms);

            break;

        case CLASSID_CONTROL_START:
        case CLASSID_CONTROL_STOP:
        default: /* event not of interest here so pass it on */

            result = dstransport->transport_control (dstransport->thisx, control, valobj);
    }

    return result;
}


/**
 * tcpdelsvc_transport_notify()
 * @param evt, caller relinquishes.
 */
int tcpdelsvc_transport_notify (i_delivery_service* ids, etch_event* evt)
{
    etch_tcp_delivery_service* tcpds = get_etch_ds_impl(ids);

    return tcpds->transport->transport_notify( tcpds->transport->thisx, evt);
}


/**
 * tcpdelsvc_transport_query()
 * i_transportmessage::transport_query override.
 * @param query, caller relinquishes.
 */
objmask* tcpdelsvc_transport_query (i_delivery_service* ids, etch_query* query) 
{
    int result = 0;
    objmask* resultobj  = NULL;
    etch_connection* cx = NULL;
    const int timeoutms = query? query->value: 0;
    const int objclass  = query? query->class_id: 0;
    etch_tcp_delivery_service* tcpds = get_etch_ds_impl(ids);
    i_transportmessage* dstransport  = tcpds->transport;
    cx = &tcpds->tcpconx->cx;

    switch(objclass)
    {  
       case CLASSID_QUERY_WAITUP:   
            result = etchconx_wait_up(cx, timeoutms);
            break;

       case CLASSID_QUERY_WAITDOWN:  
            result = etchconx_wait_down(cx, timeoutms);
            break;

        default:
            resultobj = dstransport->transport_query (dstransport->thisx, query);
            query = NULL; /* argument was relinquished */
    }

    if (query) query->destroy(query);
    return resultobj;
}


/* - - - - - - - - - - - - - - - - - - - - - - 
 * etch_resources
 * - - - - - - - - - - - - - - - - - - - - - -
 */

/*
 * etch_transport_resources_init()
 * @return transport resources map initialized with default items and values
 */
etch_resources* etch_transport_resources_init(etch_resources* resxmap) 
{
    const int SESSIONPOOL_INITSIZE = 4;

    etch_resources* resources = resxmap? resxmap: new_etch_resources(0);

    /* these threadpools are session thread managers, i.e. they manage 
     * a pool of stub listener threads on either side. */ 
    if (NULL == etch_resources_get (resources, ETCH_RESXKEY_POOLTYPE_QUEUED))  
    {   /* until we implement a queued pool, we use a free pool */
        etch_threadpool* threadpool 
            = new_threadpool (ETCH_THREADPOOLTYPE_FREE, SESSIONPOOL_INITSIZE);

        etch_resources_add (resources, (wchar_t*)ETCH_RESXKEY_POOLTYPE_QUEUED, 
           (objmask*) threadpool);

        if (is_exception(resources)) return resources; /* TODO lose the exceptions */
    }

    if (NULL == etch_resources_get (resources, ETCH_RESXKEY_POOLTYPE_FREE)) 
    {                  
        etch_threadpool* threadpool 
            = new_threadpool (ETCH_THREADPOOLTYPE_FREE, SESSIONPOOL_INITSIZE);

        etch_resources_add (resources, (wchar_t*)ETCH_RESXKEY_POOLTYPE_FREE, 
           (objmask*)threadpool);

        if (is_exception(resources)) return resources;
    }

    if (NULL == etch_resources_get (resources, ETCH_RESXKEY_MSGIZER_FORMAT)) 
    {                  
        etch_resources_add (resources, (wchar_t*)ETCH_RESXKEY_MSGIZER_FORMAT, 
           (objmask*)new_string(ETCH_RESXVAL_XPORTFMT_BINARY, ETCH_ENCODING_UTF16));

        if (is_exception(resources)) return resources;
    }

    return resources;
}


/*
 * get_etch_transport_resources()
 * @return transport resources map initialized with default items and values
 */
etch_resources* get_etch_transport_resources(etch_resources* resxmap)
{
    etch_resources* outresx = NULL;
    ETCH_ASSERT(resxmap == NULL || is_etch_hashtable(resxmap));

    outresx = etch_transport_resources_init(resxmap);

    ETCH_ASSERT(outresx);
    return outresx;
}


/* - - - - - - - - - - - - - - - - - - - - - - 
 * server/client "factories"
 * - - - - - - - - - - - - - - - - - - - - - -
 */

/*
 * destroy_etch_clientsession()
 * destructor for a server's client session's instance data.
 */
int destroy_etch_clientsession (etch_session* thisx)
{
    if (NULL == thisx) return -1;
    if (thisx->refcount > 0 && --thisx->refcount > 0) return -1; 

    if (!is_etchobj_static_content(thisx))
    {              
        if (thisx->ds)          /* destroy session's delivery service */
            thisx->ds->destroy(thisx->ds);   
        if (thisx->server_stub) /* destroy session's stub */
           ((objmask*)thisx->server_stub)->destroy(thisx->server_stub);
        if (thisx->server)      /* destroy session's server impl */
           ((objmask*)thisx->server)->destroy(thisx->server);
        if (thisx->client)      /* destroy session's remote client */
           ((objmask*)thisx->client)->destroy(thisx->client);
        /* note that we do not destroy the conximpl (accepted tcp connection) 
         * here, the receive thread destroys it instead */

        /* remove this entry from active sessions list.
         * note that thisx.thisx is the serverparams hosting the sessions list */
        remove_etch_session (thisx->thisx, thisx->session_id);
    }
            
    return destroy_objectex((objmask*)thisx);
}


/*
 * new_etch_clientsession()
 * constructor for session parameter bundle.
 * this object wraps all of a server's per-session instance data.
 */
etch_session* new_etch_clientsession (void* host, etch_connection* cx)
{
    etch_session* newobj = (etch_session*) new_object(sizeof(etch_session),
        ETCHTYPEB_CLIENT_SESSION, CLASSID_CLIENT_SESSION);

    newobj->destroy = destroy_etch_clientsession;
    newobj->thisx = host;  /* etch_serverparams* */
    newobj->cx = cx;
    newobj->session_id = cx->conxid;  /* session key */

    /* carrying the session instance data with the connection is not good design,
     * however we need it in the receive loop listener threadproc which does not  
     * see transport.h. when we figure another way to get the session's data 
     * through to the listener thread, we should remove it from the connection, 
     * since session data is at a much higher level than connection of course.
     */
    cx->session = (objmask*) newobj;

    return newobj;
}


/*
 * get_etch_session()
 * look up session instance data object matching specified session ID.
 * @return index of entry, or -1 if not found. 
 * array entry is returned via out parameter if specified.
 */
int get_etch_session (etch_server_factory* sf, const int session_id, etch_session** out)
{
    int  ndx = 0, retndx = -1;
    etch_iterator iterator;
    etch_session  *s = NULL;
    ETCH_ASSERT(is_etch_factoryparams(sf));
    set_iterator(&iterator, sf->clientlist, &sf->clientlist->iterable); 

    while(iterator.has_next(&iterator))
    {   if ((NULL != (s = iterator.current_value)) && (s->session_id == session_id))
        {   if (out) *out = s;
            retndx = ndx;
            break;
        } 
        ndx++;
        iterator.next(&iterator);  
    }

    return retndx;
}


/*
 * remove_etch_session()
 * remove session instance data object matching specified session ID.
 * does not destroy list item content but rather returns it for caller disposition.
 * @return the removed object, or NULL if not found.
 */
etch_session* remove_etch_session (etch_server_factory* sf, const int session_id)
{
    etch_session* foundsession = NULL;

    const int whichndx = get_etch_session (sf, session_id, &foundsession);

    if (whichndx >= 0)
        arraylist_remove (sf->clientlist, whichndx, FALSE);

    return foundsession;
}


/*
 * destroy_etch_client_factory()
 * client params destructor. 
 * destroys the ancillary objects attached to a remote server.
 */
int destroy_etch_client_factory (etch_client_factory* thisx)
{
    if (NULL == thisx) return -1;
    if (thisx->refcount > 0 && --thisx->refcount > 0) return -1;  

    if (!is_etchobj_static_content(thisx))
    {                     
        if (thisx->iclient)                           /* destroy i_xxxx_client */
        {   etchlog (ETCHTRAN, ETCHLOG_DEBUG, "destroying client interface ...\n");
            ETCHOBJ_DESTROY(((objmask*)thisx->iclient));  
        }

        if (thisx->stub)                              /* destroy xxxx_client_stub */
        {   etchlog (ETCHTRAN, ETCHLOG_DEBUG, "destroying stub ...\n");
            ETCHOBJ_DESTROY(((objmask*)thisx->stub));   
        } 

        if (thisx->dsvc)                             /* destroy i_delivery_service */
        {   etchlog (ETCHTRAN, ETCHLOG_DEBUG, "destroying delivery service ...\n");
            ETCHOBJ_DESTROY(thisx->dsvc); 
        }               

        if (thisx->in_resx)                          /* destroy etch_resources */
        {   etchlog (ETCHTRAN, ETCHLOG_DEBUG, "destroying resources ...\n");
            ETCHOBJ_DESTROY(thisx->in_resx);   
        }          

        ETCHOBJ_DESTROY(thisx->mainpool);            /* destroy thread manager */
        ETCHOBJ_DESTROY(thisx->rwlock);              /* destroy read/write mutex */
    }
         
    return destroy_objectex(((objmask*)thisx)); 
}


/*
 * new_client_factory()
 * constructor for client parameter bundle.
 */
etch_client_factory* new_client_factory (objmask* session, 
    i_session* isession, new_client_funcptr new_client_func) 
{
    const int MAINPOOL_INITIAL_SIZE = 4;

    etch_client_factory* cf = (etch_client_factory*) new_object 
        (sizeof(etch_client_factory), ETCHTYPEB_FACTORYPARAMS, CLASSID_CLIENTFACTORY);

    cf->destroy    = destroy_etch_client_factory;

    cf->new_client = new_client_func;

    cf->mainpool   = new_threadpool (ETCH_THREADPOOLTYPE_FREE, MAINPOOL_INITIAL_SIZE);

    cf->rwlock     = new_mutex (etch_apr_mempool, ETCHMUTEX_UNNESTED);

    return cf;
}


/*
 * destroy_etch_server_factory()
 * server params destructor. 
 */
int destroy_etch_server_factory (etch_server_factory* thisx)
{
    if (NULL == thisx) return -1;
    if (thisx->refcount > 0 && --thisx->refcount > 0) return -1;  

    if (!is_etchobj_static_content(thisx))
    {                          
        ETCHOBJ_DESTROY(thisx->clientlist);  /* destroy per-client arraylist */
        ETCHOBJ_DESTROY(thisx->mainpool);    /* destroy main thread manager */
        ETCHOBJ_DESTROY(thisx->subpool);     /* destroy client thread manager */
        ETCHOBJ_DESTROY(thisx->mblock);      /* destroy mutex */
    }
            
    return destroy_objectex((objmask*)thisx);
}


/*
 * new_server_factory()
 * constructor for server parameter bundle.
 * fyi invoked from this.new_etch_listener()
 */
etch_server_factory* new_server_factory (objmask* session, i_session* isession,
    helper_new_server_funcptr  helper_new_listener_callback, 
    main_new_server_funcptr    main_new_server_callback)   
{
    const int MAINPOOL_INITIAL_SIZE = 4, SUBPOOL_INITIAL_SIZE = 4;

    etch_server_factory* sf = (etch_server_factory*) new_object 
        (sizeof(etch_server_factory), ETCHTYPEB_FACTORYPARAMS, CLASSID_SERVERFACTORY);
    
    sf->destroy = destroy_etch_server_factory;

    sf->clientlist = new_arraylist(ETCH_DEFSIZE, ETCH_DEFSIZE);
    sf->clientlist->content_type = ETCHARRAYLIST_CONTENT_SIMPLE;
    sf->clientlist->is_readonly = TRUE; /* array does not own its content */

    sf->helper_new_accepted = helper_new_listener_callback;
    sf->main_new_server     = main_new_server_callback;
    sf->session  = session;
    sf->isession = isession;

    sf->mainpool = new_threadpool (ETCH_THREADPOOLTYPE_FREE, MAINPOOL_INITIAL_SIZE);
    sf->subpool  = new_threadpool (ETCH_THREADPOOLTYPE_FREE, SUBPOOL_INITIAL_SIZE);  
    sf->mblock   = new_mutex (etch_apr_mempool, ETCHMUTEX_UNNESTED);

    return sf;
}


/* - - - - - - - - - - - - - - - - - - - - - - 
 * transport listener
 * - - - - - - - - - - - - - - - - - - - - - -
 */

/*
 * tcpxfact_session_accepted()
 * override for transport factory session_accepted()
 * signature is typedef int (*etch_session_accepted) (void* thisx, void* socket);
 * parallels java TcpTransportFactory.newListener.newSessionListener.sessionAccepted
 * @param thisx the i_sessionlistener quasi interface
 * @param socket an open accept raw socket, wrapped by etch_socket. caller relinquishes.
 * in practice this is an apr socket wrapped by etch_socket.
 * @return 0 success, -1 failure.
 */
int tcpxfact_session_accepted (i_sessionlistener* thisx, etch_tcp_connection* tcpconx)
{
    int result = 0;
    void* newstub = NULL;
    etch_session* newsession = NULL;
    etch_server_factory* params = NULL;
    etch_connection* cx = &tcpconx->cx;
    i_delivery_service* delivery_service = NULL;
    const int session_id = cx->conxid;
    ETCH_ASSERT(is_etch_sessionlxr(thisx));
    ETCH_ASSERT(is_etch_tcpconnection(tcpconx));
    params = (etch_server_factory*) thisx->server_params;
    ETCH_ASSERT(params && params->helper_new_accepted);

    /* fyi java binding makes a copy of the generic resources here.  
     * we instead will not use resources for client-specific map entries,
     * but rather will use the parameter bundle. TODO allocate client-specific
     * segment of parameter bundle.
     */
    etchlog (ETCHTRAN, ETCHLOG_INFO, "creating client session %d ...\n", session_id);

    /* instantiate delivery service */
    etchlog (ETCHTRAN, ETCHLOG_DEBUG, "creating delivery service ...\n");

    delivery_service = new_etch_transport_a (thisx->url, thisx->server_params, tcpconx);  

    if (NULL == delivery_service) 
    {   etchlog (ETCHTRAN, ETCHLOG_DEBUG, "could not create delivery service\n");
        return -1;
    }

    ETCH_ASSERT(delivery_service->itm && delivery_service->itm->transport_control);
    etchlog (ETCHTRAN, ETCHLOG_DEBUG, "delivery service created\n");

    newsession = new_etch_clientsession (params, cx);
    newsession->mainlistener = thisx;  /* session points back to accept listener */
    newsession->ds = delivery_service;  
    newsession->conximpl = (objmask*) tcpconx;   

     /* CALL BACK to helper.new_helper_accepted_server() to create this 
      * client's server side listener, server implementation, and stub. 
      */
    newstub = params->helper_new_accepted (params, newsession);  
    ETCH_ASSERT(is_etch_stub(newstub));

    arraylist_add (params->clientlist, newsession);  
    etchlog (ETCHTRAN, ETCHLOG_DEBUG, "client session %d created\n", session_id);

    /* START this client's individual listener. since we have an accepted socket
     * in hand, it is in effect already started.
     */
    etchlog (ETCHTRAN, ETCHLOG_INFO, "starting client session %d ...\n", session_id);
    result = delivery_service->itm->transport_control (delivery_service, 
        new_etch_event(CLASSID_CONTROL_START, 0), NULL);

    return result;
}


/*
 * tcpxfact_get_session()
 * return session interface from the server factory bundle.
 * validate and assert the i_sessionlistener object.
 */
i_session* tcpxfact_get_session (i_sessionlistener* lxr)
{
   i_session* session = NULL;
   etch_server_factory* factory = NULL;
   ETCH_ASSERT(is_etch_sessionlxr(lxr));
   factory = lxr->server_params;
   session = factory? factory->isession: NULL;
   return session;
}


/*
 * tcpxfact_session_control()
 * @param control event, caller relinquishes.
 * @param value control value, caller relinquishes.
 */
int tcpxfact_session_control (i_sessionlistener* thisx, etch_event* control, objmask* value)
{           
    int result = -1;
    i_session* session = tcpxfact_get_session (thisx);

    if (session && session->session_control)  
        result = session->session_control (session, control, value);
    else 
    {   if (control)
            control->destroy(control);
        if (value)
            value->destroy(value);
    }

    return result;
}


/*
 * tcpxfact_session_notify()
 * @param evt event, caller relinquishes.
 */
int tcpxfact_session_notify (i_sessionlistener* thisx, etch_event* evt)
{
    int result = -1;
    i_session* session = tcpxfact_get_session (thisx);

    if (session && session->session_notify)  
        result = session->session_notify (session, evt);
    else 
    if (evt)
        evt->destroy(evt);

    return result;
}


/*
 * tcpxfact_session_query()
 * @param query caller relinquishes
 */
void* tcpxfact_session_query (i_sessionlistener* thisx, etch_query* query)
{
    void* resultobj = NULL;
    i_session* session = tcpxfact_get_session (thisx);

    if (session && session->session_query)  
        resultobj = session->session_query (session, query);
    else 
    if (query)
        query->destroy(query);

    return resultobj;
}


/*
 * transport_thread_id()
 * return thread_id for thread zero on the main pool of this listener.
 */
int transport_thread_id (i_sessionlistener* listener)
{
    etch_server_factory* sf = NULL; etch_thread* thread0 = NULL;
    ETCH_ASSERT(listener && is_etch_serverparams(listener->server_params));
    sf = (etch_server_factory*) listener->server_params;
    ETCH_ASSERT(sf->mainpool);
    thread0 = threadpool_thread (sf->mainpool, 0);
    return thread0? thread0->params.etch_thread_id: 0;
}


/*
 * transport_session_count()
 * return count of outstanding client sessions for this server.
 */
int transport_session_count (i_sessionlistener* listener)
{
    etch_server_factory* sf = NULL;
    if (NULL == listener || NULL == listener->server_params) return 0;
    sf = listener->server_params;
    if (NULL == sf || NULL == sf->clientlist) return 0;
    return sf->clientlist->count;
}

 
/*
 * tcpxfact_teardown_client_sessions()
 * signal and wait for each session thread to exit, destroying each 
 * thread, connection and session. tearing down the session destroys its
 * delivery service, remote client, and stub. this is intended to be invoked 
 * only at server shutdown, after the main (accept) thread has exited.
 */
int transport_teardown_client_sessions (i_sessionlistener* listener)
{
    etch_iterator iterator;
    etch_session* session = NULL;
    etch_server_factory* sf = listener->server_params;
    set_iterator (&iterator, sf->clientlist, &sf->clientlist->iterable);

    while(iterator.has_next(&iterator)) /* for each extant client session ... */
    {    
        if (NULL != (session = iterator.current_value))  
        {   
            if (is_etch_connection(session->cx))
            {   session->cx->is_started = FALSE;   /* mark connection stopped */

                if (is_etch_thread(session->cx->thread))
                {   etch_join (session->cx->thread); /* BLOCK for thread exit */
                    ETCHOBJ_DESTROY(session->cx->thread);   /* destroy thread */
                }
            }

            ETCHOBJ_DESTROY(session->conximpl);      /* destroy transport obj */
            session->destroy(session);               /* teardown this session */
        } 

        iterator.next(&iterator);  
    }  

    return 0;
}


/*
 * etch_listener_waitfor_exit()
 * block until accept listener thread exits.
 */
int etch_listener_waitfor_exit (i_sessionlistener* thisx)
{
    etch_server_factory* p = thisx->server_params;
    const int result = threadpool_waitfor_all (p->mainpool, FALSE);
    return result; 
}


/*
 * destroy_etch_listener()
 * etch_listener destructor.
 */
int destroy_etch_listener (i_sessionlistener* thisx)
{
    if (NULL == thisx) return -1;
    if (thisx->refcount > 0 && --thisx->refcount > 0) return -1;  

    if (!is_etchobj_static_content(thisx))
    {   
        ETCHOBJ_DESTROY(((objmask*)thisx->url));                         

        if (thisx->is_session_owned)
            etch_free(thisx->isession);

        if (thisx->is_transport_owned)
            etch_free(thisx->itransport);

        if (thisx->is_resources_owned)
        {
            etchlog (ETCHTRAN, ETCHLOG_DEBUG, "destroying resources ...\n");
            ETCHOBJ_DESTROY(thisx->resources);
        }

        if (thisx->thisx)
        {   /* watch this spot: the i_sessionlistener and the etch_tcp_server 
             * have mutual references. we must ensure that if we are to 
             * destroy the etch_tcp_server via the i_sessionlistener, that the
             * etch_tcp_server does not also destroy the i_sessionlistener. */
            etch_tcp_server* srvobj = (etch_tcp_server*) thisx->thisx;
            ETCH_ASSERT(is_etch_tcpserver(srvobj));
            srvobj->destroy(srvobj);
        }

        if (thisx->server_params)  
        {
            etch_server_factory* sf = thisx->server_params;
            ETCH_ASSERT(is_etch_serverparams(sf));
            sf->destroy(sf);
        }
    }
            
    return destroy_objectex((objmask*)thisx);
}


/*
 * new_etch_listener()
 * constructs a new transport listener used to construct server sessions.
 * @param uri a uri string, caller relinquishes.
 * @param resx a resources map, caller relinquishes. currently ALWAYS NULL.
 * @param helper_new_server_funcptr pointer to the listener ctor in server helper.
 * @param main_new_server_funcptr pointer to the server impl ctor in main. 
 * @param get_xxxx_resources_funcptr helper new service resources callback.
 * @return an i_sessionlistener interface. caller owns it. note that java binding 
 * returns a transport interface, whereas c binding will instead extract the 
 * transport interface from i_sessionlistener.itransport.
 */
i_sessionlistener* new_etch_listener (wchar_t* uri, etch_resources* resx, 
    helper_new_server_funcptr  helper_new_listener_callback,
    main_new_server_funcptr    main_new_server_callback,
    get_xxxx_resources_funcptr helper_get_resources) 
{
    i_transport* itransport = NULL;
    etch_tcp_server* tcp_server = NULL;
    etch_server_factory* params = NULL;
    etch_url* url = new_url(uri); 
     
    /* listener assumes the session interface of the server factory creator.
     * this accomplishes the same thing as the session method implementations 
     * found in java TcpTransportFactory.newListener().
     */
    i_session* isession = new_session_interface (NULL,  
        tcpxfact_session_control, 
        tcpxfact_session_notify, 
        tcpxfact_session_query);
   
    /* create the listener interface, specifying the on_session_accepted
     * callback to be invoked on each successful server accept in order
     * to create a new server. relinquish isession to listener here. */
    i_sessionlistener* listener = new_sessionlistener_interface (NULL, 
        tcpxfact_session_accepted, isession);
    
    listener->destroy   = destroy_etch_listener;
    listener->wait_exit = etch_listener_waitfor_exit;
    listener->url = url;        /* relinquished */

    /* create server "factory", which is in the c binding a parameter   
     * bundle which includes callbacks to the new server constructors */
    params = new_server_factory ((objmask*) listener, listener->isession, 
        helper_new_listener_callback, 
        main_new_server_callback);

    /* instantiate generic resources and call back to specific helper for vf */
    listener->is_resources_owned = TRUE;    
    listener->resources = get_etch_transport_resources (resx);  /* resx null */
    params->in_resx = listener->resources;  
    params->in_uri  = uri;  
    helper_get_resources (params);   
    listener->server_params = params;
    /* fyi params delivery service is set later, in svr->on_session_accepted(), 
       whose implementation is tcpxfact_session_accepted(), in this module */

    /* create the tcp connection and acceptor SVR BREAK 001 */
    tcp_server = new_tcp_server (url, params->mainpool, params->subpool, resx, listener);

    if (NULL == tcp_server) 
    {   listener->destroy(listener);
        return NULL;
    }

    /* listener [main] expects that i_sessionlistener.thisx is the server,
     * e.g. an etch_tcp_server* */
    listener->thisx = tcp_server; 
    
    /* copy server object's session virtuals to this object */
    /* see java TcpTransportFactory.newListener() for session impls */
    listener->session  = tcp_server->session;
    listener->isession = tcp_server->isession;
    listener->session_control = tcp_server->session_control;
    listener->session_notify  = tcp_server->session_notify;
    listener->session_query   = tcp_server->session_query;

    /* set this listener object's transport to be the server connection's transport */
    ETCH_ASSERT(tcp_server->itransport);
    etch_free(listener->itransport);  /* TODO don't instantiate in the first place */
    listener->itransport = tcp_server->itransport;
    listener->transport_control = tcp_server->transport_control;
    listener->transport_notify  = tcp_server->transport_notify;
    listener->transport_query   = tcp_server->transport_query;
    listener->set_session = tcp_server->set_session;
    listener->get_session = tcp_server->get_session;
    listener->is_transport_owned = FALSE;
 
    return listener;  /* caller owns this object */  
}


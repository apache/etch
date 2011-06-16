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
 * etch_stub.c
 * contains the base runtime code for either type of stub.
 *
 * the generated stub_xxxx_server will define the thread procedures for each message
 * type in the service. for each such type, it will set the virtual stub helpers in 
 * the type objects, to point to these thread functions. each such thread procedure  
 * will call the user-coded calculation in the impl_xxxx_server for that message  
 * type; for example, for an "add" message, it will get the fields from the service
 * value factory which are the arguments to the calculation, call the calculation in
 * the server impl, insert the result of the calculation into the reply message, and
 * call the delivery service transport_message to send the reply back to the sender.  
 */

#include "etch.h"
#include "etch_svcobj_masks.h"
#include "etch_stub.h"
#include "etch_exception.h"
#include "etch_objecttypes.h"
#include "etch_log.h"
#include "etch_mem.h"
#include "etch_object.h"
#include "etch_message.h"
static const char* LOG_CATEGORY = "etch_stub";

int etchstub_session_notify_ex (void*, void*, etch_event*);
int etchstub_session_message(void*, etch_who*, etch_message*);
int etchstub_session_control(void*, etch_event*, etch_object*);
int etchstub_session_notify (void*, etch_event*);
etch_object* etchstub_session_query (void*, etch_query*); 
i_objsession* etchstub_get_session_callbacks_from (void* _obj);

char* scstr = "session control", *snstr = "session notify", *sqstr = "session query";
char* logmask1 = "%s not routed to client\n", *logmask2 = "%s not registered\n";
 


#if(0)
____________________________________________________________________________________
the inheritance hierarchy becomes indefinite at this point. a stub should be able
to query a service object for a particular interface, down through its inheritance
chain. for example, we need to find who has implemented obj_session and call methods
on it. a use case is a custom transport implemented as a dynamically loaded dll.
the stub needs therefore to be able to call into objects it has never seen before. 

for the present we will make some assumptions as to the inheritance hierarchy, that
being that given a service interface, we know its inheritance hierarchy. following
is a diagram illustrating the inheritance of a service interface named XXXX. 
all arrows direct down unless indicated by an arrowhead. (e) indicates extends,
(i) indicates implements. this diagram is upside-down to the way we normally see
the hierarchy.
                                     (obj)
                                       |
                                     XXXX  <- - - - - - - - - - (stub)
                                     |  |
                       (e)  ---------    ----------  (e)
                           |                       |
                      XXXX_server            remote_XXXX
                           |      \ (i)            | (e)
                       (i) |        ------> remote_XXXX_server             
  obj_session <--  base_XXXX_server      
                       (e) |
                   impl_XXXX_server
____________________________________________________________________________________
#endif


#if(0)

IMPL_XXXX_SERVER
   - ctor(REMOTE_XXXX_CLIENT)
   - usermethod_1_impl(); ...
   - usermethod_n_impl();

   BASE_XXXX_SERVER
      - usermethod_1_stub(); ...
      - usermethod_n_stub();
      OBJSESSION
      - _session_control; _session_notify; _session_query; (stubs)
      XXXX_SERVER
      - any server-directed methods go here
         XXXX
         - see user method impls above


STUBXXXX<T extends XXXX> extends STUBBASE<T>
   - ctor(DeliveryService, pool, pool);
   - stubhelper();

   STUBBASE<T> (DeliveryService, pool, pool)
      T obj;
      DeliveryService, Pool, Pool;
      stub_helper();  

      STUBPOOLRUNNABLE
      - set(stub_helper, who, msg); (set state, not in java)
      - stubhelper(stub, delsvc, obj, who, msg);  

      SESSIONMESSAGE
      - session_message(who, message); 
        { stubhelper = message.type.get_helper() 
          stubhelper(delsvc, obj, who, msg); // the stubhelper impl calls threadpool run()
        }
      - session_control; session_notify; session_query;  


STUBXXXSERVER  (generated, hand-codable)
   - ctor(DeliveryService, XXXXSERVER, pool, pool);
   - for each type in the vf, 
        -- implement a run method (threadproc) for the message type 
        -- implement a stub helper method which calls the threadpool run using the threadproc above
    
   STUBXXXX

#endif


/* - - - - - - - - - - - - - - - - - -  
 * stub base method implementations
 * - - - - - - - - - - - - - - - - - -  
 */ 

/**
 * destroy_stub()
 * etch_stub destructor. destroy stub base object.
 * the stub base does not destroy its xxxx_either_stub wrapper.
 */
int destroy_stub(void* data)
{
    etch_stub* thisx = (etch_stub*)data;

    if (!is_etchobj_static_content(thisx))
    {
        /* note that a delivery service session is the stub's session.
         * the delivery service owns the stub, and the stub owns this shared
         * session. the stub wrapper is the session interface's thisx.
         * the delivery service gets xxxx_either_stub* stubbobj from its  
         * isessionmsg->thisx, and etch_stub* stubbase from stubobj.stub_base.
         * destroying the stub will destroy the shared i_sessionmessage, so 
         * the delivery service's session will become invalid once its stub 
         * is destroyed. see transport.destroy_delivery_service_stub().
         */
        etch_object_destroy(thisx->isessionmsg);

        if (thisx->obj && thisx->is_implobj_owned)
            etch_object_destroy(thisx->obj);
    }

    return destroy_objectex((etch_object*) thisx);
}


/**
 * new_stub()
 * etch_stub (stub base) constructor.  
 */
etch_stub* new_stub (void* implobj, unsigned char stubtype, 
   i_delivery_service* ids, etch_threadpool* qp, etch_threadpool* fp) 
{
    i_sessionmessage* ism = NULL;

    etch_stub* stubbase = (etch_stub*) new_object  
       (sizeof(etch_stub), ETCHTYPEB_STUB, CLASSID_STUB);

    ETCH_ASSERT(implobj && stubtype && qp && fp);
    ETCH_ASSERT(is_etch_ideliverysvc(ids));

    stubbase->stub_type = stubtype;
    ((etch_object*)stubbase)->destroy   = destroy_stub; 

    stubbase->obj = implobj; /* server or client impl e.g. i_xxxx_server* */
    stubbase->delivery_service  = ids;

    /* instantiate i_sessionmessage session interface. note that the interface's
     * thisx pointer is this base stub object, and that the base stub's container
     * xxxx_either_stub* is the base stub.stubobj.
     */
    ism = new_sessionmsg_interface (stubbase, etchstub_session_message, NULL); 
    stubbase->isessionmsg = ism;
    stubbase->session_message = ism->session_message = etchstub_session_message;
    stubbase->session_control = ism->session_control = etchstub_session_control;
    stubbase->session_notify  = ism->session_notify  = etchstub_session_notify;
    stubbase->session_query   = ism->session_query   = etchstub_session_query;

    /* set delivery service session interface to be the stub's i_sessionmessage.
     * in a c binding translation from java binding, a reference to a stub object 
     * is only via via the delivery service's session_message interface, shared
     * between the stub and the delivery service.  
     * the delivery service must have provided i_transportmessage implementations,
     * in particular for set_session(). the instantiator of the delivery service
     * must therefore override the delivery service's i_transportmessage.

     * fyi: ids->itm->thisx is mailbox manager.
     * ids->itm is mboxmgr's transport interface, which is the delivery service.
     * set session of next lower level (delivery service) to the stub's session.
     */
    ids->itm->set_session (ids, stubbase->isessionmsg);   

    /* copy impl's i_objsession to the stub for convenience */
    stubbase->impl_callbacks = etchstub_get_session_callbacks_from (implobj); 
 
    return stubbase;
}

 
/**
 * etchstub_get_session_callbacks_from()
 * extract objsession interface from specified stub implementor object.
 */
i_objsession* etchstub_get_session_callbacks_from (void* _obj)
{
    i_objsession* iobjsession = NULL;
    etch_object*  obj = (etch_object*) _obj; 
    const int this_objtype = obj? ((etch_object*)obj)->obj_type: 0;

    switch(this_objtype)
    {
        case ETCHTYPEB_EXESERVERIMPL:
        {    xxxx_server_impl* server_impl = (xxxx_server_impl*) obj;
             iobjsession = server_impl->iobjsession;
             break;
        }

        case ETCHTYPEB_EXECLIENTIMPL:
        {    xxxx_client_impl* client_impl = (xxxx_client_impl*) obj;
             iobjsession = client_impl->iobjsession;
             break;
        }

        case ETCHTYPEB_EXESERVERBASE:
        {   i_xxxx_server* iserver = (i_xxxx_server*) obj;
            xxxx_server_impl* server_impl = (xxxx_server_impl*) iserver->thisx;
            ETCH_ASSERT(server_impl && ((etch_object*)server_impl)->obj_type == ETCHTYPEB_EXESERVERIMPL);
            iobjsession = server_impl->iobjsession;
            break;
        }

        case ETCHTYPEB_EXECLIENTBASE:
        {   i_xxxx_client* iclient = (i_xxxx_client*) obj;
            xxxx_client_impl* client_impl = (xxxx_client_impl*) iclient->thisx;
            ETCH_ASSERT(client_impl && ((etch_object*)client_impl)->obj_type == ETCHTYPEB_EXECLIENTIMPL);
            iobjsession = client_impl->iobjsession;
            break;
        }
    }

    return iobjsession;
}
 
/**
 * etchstub_put_resultobj
 * insert specified result object to message.
 * @param replymsg the message, caller retains.
 * @param resultobj the result object, caller relinquishes regardless of result.
 * @return 0 success, -1 failure. 
 */
int etchstub_put_resultobj (etch_stub* stub, etch_message* replymsg, etch_object* resultobj)
{
    int result = 0;
    etch_field* key_result = builtins._mf_result;

    /* check if the message already has a result object, i.e., an exception */
    if (etch_message_get (replymsg, key_result))
        etch_object_destroy(resultobj);  /* if so, discard the passed result */
    else  /* resultobj is relinquished here regardless of result */
        result = etch_message_put (replymsg, clone_field(key_result), (etch_object*) resultobj);

    return result;
}

 
/**
 * etchstub_send_reply
 * private method invoked by all "stub helpers" (message logic implementation 
 * runners) for messages requiring a reply, in order to instantiate a reply
 * message and transmit it back to sender.
 */
int etchstub_send_reply (etch_stub* stub, i_delivery_service* dsvc, etch_who* whofrom, etch_message* msg, etch_object* resultobj, boolean putResultObj)
{
    int result = 0; 
    const int is_exception_resobj  = is_etch_exception(resultobj);
    
    etch_type* newtype = NULL; /* see comment following */

    /* instantiate the reply message  
     * note that if we are called for a one-way message, it is only if an 
     * exception was thrown by the implementation, and the exception is 
     * therefore to be the reply. in such a case, the etch compiler generates
     * a message type as newtype above, which is the second parameter to 
     * etch_message_reply(), in order that in_reply_to can be instantiated.
     * (our example below makes newtype the 1-way exception reply type in  
     * this case). for two-way messages, newtype, and thus the second parameter
     * to etch_message_reply() is null.
     */
    etch_message* replymsg = etch_message_reply (msg, newtype);

    if (NULL == replymsg && is_exception_resobj)
    {   newtype  = builtins._mt__exception;
        replymsg = etch_message_reply (msg, newtype);
    }

    if (NULL == replymsg) {
        etch_type* replytype = newtype? newtype: etch_message_type(msg);
        char* logmask = "could not create reply message for type %s\n";
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, logmask, replytype->aname);
        return -1;
    }

    if(putResultObj) {
        result = etchstub_put_resultobj (stub, replymsg, resultobj);
        if(result){
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "invalid return value.\n");
            return result;
        }
    }

    if (0 == result) {  /* forward reply message for serialization */
        result = dsvc->itm->transport_message (dsvc, whofrom, replymsg);
        if(result){
            etch_object_destroy(replymsg);
        }
    } 

    return result;
}


/**
 * etchstub_validate_args
 * private method invoked by all "stub helpers" to validate parameters
 * to the individual stub helper and populate some objects for the helper.
 */
int etchstub_validate_args (etch_stub* stub, i_delivery_service* dsvc, 
    etch_message* msg, void* client, default_value_factory** vf, 
    void** vfimpl, void** climpl)
{
    int result = 0;
    i_xxxx_client* iclient = NULL;
    etch_object* clientimpl = NULL, *valufactimpl = NULL;
    /* any assertion failure in this method indicates a coding error in etch core */
    ETCH_ASSERT(stub && dsvc && client && msg && stub->params && vf && vfimpl && climpl);  

    iclient = (i_xxxx_client*) client;
    clientimpl = iclient->thisx;  
    //HCG Check this for client!
    //ETCH_ASSERT(is_etch_server_impl(clientimpl));
    *climpl = clientimpl;

    *vf = (default_value_factory*) stub->params->in_valufact;
    ETCH_ASSERT(is_etch_valuefact(*vf));
    valufactimpl = (*vf)->impl;
    ETCH_ASSERT(is_etch_valuefactimpl(valufactimpl));
    *vfimpl = valufactimpl;

    return result;
}


/**
 * etchstub_session_notify_ex()
 *
 * @param thisx the object which will receive an exception if any.
 * may be the same object as _obj. caller retains. 
 *
 * @param _obj caller retains. usually a xxxx_server_impl. may be null, or
 * may be the same object as thisx. obj will belong to a known and limited set  
 * of classes, e.g. ETCHTYPEB_EXESERVERIMPL, so we can test _obj.obj_type 
 * if needed, and cast _obj to one of the mask objects (etch_svcobj_masks.h) 
 * such as xxxx_server_impl, in order to reference the maskable content.
 * 
 * @param evt a notification event or a throwable exception, caller relinquishes.
 *
 * @return 0 success, -1 failure.
 */
int etchstub_session_notify_ex (void* thisx, void* _obj, etch_event* evt)     
{
    int result = -1, is_evtparm_relinquished = FALSE;

    /* this call arrives from type "stub helpers" implemented in xxxx_server_stub.
     * these are thread procedures with arguments delivery service, some object
     * _obj (possibly implementing objsession or throwable), who, and message, 
     * viz:  int (*stubhelper) (stub, deliverysvc, _obj, sender, message);  
     * so from argument _obj, we extract the objsession interface and if present,    
     * call its _session_notify. 
     */

    /* a server implementation (xxxx_server_impl) can request notifications of 
     * exceptions and the like by implementing and registering i_objsession 
     * callbacks. the presence of these function pointers in the implementation
     * serves as the indicator of whether to forward the notification.
     */
    i_objsession* impl_callbacks = etchstub_get_session_callbacks_from (_obj);  

    etch_session_notify impl_session_notify = impl_callbacks? 
        impl_callbacks->_session_notify: NULL;

    if (impl_session_notify) /* if impl has requested this event, forward it */
    {   /* event is relinquished to notify handlers by contract */
        result = impl_session_notify (impl_callbacks, evt);
        is_evtparm_relinquished = TRUE;  
    }else {
        if(((xxxx_either_impl*)_obj)->_session_notify) {
            result = ((xxxx_either_impl*)_obj)->_session_notify ((xxxx_either_impl*)_obj, evt);
            is_evtparm_relinquished = TRUE;  
        }
    }

    if (0 != result)
    {
        char* logmask = impl_session_notify? logmask1: logmask2;
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, logmask, snstr);
    }

    if (!is_evtparm_relinquished)  
	{
		etch_object_destroy(evt); 
		evt = NULL;
		//ETCHOBJ_DESTROY(evt);
	}

    return result;
}


/* - - - - - - - - - - - - - - - - - - - - -
 * stub helper threadpool execution support
 * - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * etchstub_loghelper()
 * convenience method to log entry or exit of a stub helper function.
 */
void etchstub_loghelper(opaque_stubhelper f, const int result, const int thread_id, const int is_start)
{
    if(is_start) {
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "start stub runner %x [%d]\n", f, thread_id); 
    } else {
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "exit stub runner %x (%d) [%d]\n", f, result, thread_id); 
    }
}

/**
 * etchstub_proxy_threadproc()
 * a thread procedure conforming to the signature expected by etch_threadpool,
 * which accepts a stub parameter bundle as params.data, unbundles the parameters, 
 * and invokes the included stub helper function with those parameters.
 */
void etchstub_proxy_threadproc (void* data)
{
  etch_thread_params* tp = (etch_thread_params*)data;
    int result = 0;
    etchstub_runparams* p = tp? tp->data: NULL;
    ETCH_ASSERT(p && p->run && (p->sig == ETCH_STUBPARAMS_SIGNATURE));
    etchstub_loghelper (p->run, 0, tp->etch_thread_id, 1);

    /* run "stub helper" procedure */
    result = p->run (p->stub, p->ds, p->server, p->who, p->msg); 

    etchstub_loghelper (p->run, result, tp->etch_thread_id, 0);

    //tp->etchobj->destroy(tp->etchobj);
    etch_free(p);
    

}


/**
 * new_etch_stubparams()
 * bundle up stub runner parameters in order they can become 
 * etch_thread_params.data, passed in to the etchstub_proxy_threadproc. 
 */
etchstub_runparams* new_etch_stubparams (etch_stub* stub, opaque_stubhelper runproc, 
    i_delivery_service* ds, i_xxxx_server* server, etch_who* whofrom, etch_message* msg)
{
    etchstub_runparams* p = etch_malloc(sizeof(etchstub_runparams), 0); 
    p->sig  = ETCH_STUBPARAMS_SIGNATURE;
    p->stub = stub;
    p->run  = runproc;
    p->ds   = ds;
    p->who  = whofrom;
    p->msg  = msg;
    p->server = server;
    return p;
}


/**
 * etchstub_run()
 * execute the service method helper function (stub helper). if no threadpool
 * is specified, execute the helper function inline (on caller's thread);  
 * otherwise wrap the stub helper arguments and launch a thread on the supplied  
 * threadpool to execute the helper function.
 * @param stub caller retains.
 * @param runproc the service function for this message type. 
 * @param threadpool caller retains.
 * @param server caller retains.
 * @param who caller retains.
 * @param msg caller retains.
 * @return 0 success, -1 failure.
 */
int etchstub_run (etch_stub* stub, opaque_stubhelper runproc, etch_threadpool* threadpool, 
    i_xxxx_server* server, etch_who* who, etch_message* msg)
{
    int result = 0;
    i_delivery_service* ds = stub->delivery_service;
    ETCH_ASSERT(runproc && ds && server && msg);

    if (threadpool)
    {   
        /* a pointer to this parameter bundle will become etch_thread_params.data. 
         * etch_thread_params.is_own_data determines if the thread frees data on exit.
         * is_own_data assumes the value of threadpool.is_free_data, which is true
         * by default. so unless we have reset is_free_data on this pool, the pool  
         * thread will free this allocation at thread exit. and since also by default
         * pool->is_data_etchobject is false, the deallocation will use etch_free;
         */
        etchstub_runparams* p = new_etch_stubparams(stub, runproc, ds, server, who, msg); 
        etch_thread* poolthread = NULL;
        threadpool->is_manual_start = TRUE;

        /* run stub helper function on a pool thread */
        poolthread = threadpool->run (threadpool, etchstub_proxy_threadproc, p);

        //poolthread->is_joined = TRUE;
        //poolthread->on_exit()
        poolthread->start(poolthread);
//        threadpool->is_manual_start = FALSE;

        //if  (poolthread) /* switch context and block on pool thread exit */
        //{    const int id = poolthread->params.etch_thread_id;
        //     //etch_log(etchstub, etch_log_xdebug, "stub runner joining pool thread %d ...\n", id);
        //     etch_join (poolthread);  
        //}
        //else result = -1;
    }
    else  
    {   etchstub_loghelper(runproc, 0, 0, 1);

        /* run stub helper function on this thread */
        result = runproc (stub, ds, server, who, msg); 
 
        etchstub_loghelper(runproc, result, 0, 0); 

    }
    return result;
}


/* - - - - - - - - - -
 * i_sessionmessage
 * - - - - - - - - - - 
 */

/**
 * etchstub_session_message()
 * @param whofrom caller retains, can be null.
 * @param msg caller relinquishes
 * @return 0 (message handled), or -1 (error, closed, or timeout)  
 */
int etchstub_session_message (void* data, etch_who* whofrom, etch_message* msg)
{
    etch_stub* thisx = (etch_stub*)data;
    xxxx_either_stub* stubimpl   = (xxxx_either_stub*) thisx->stubobj;
    i_xxxx_server*  serverimpl   = NULL;
    etch_session* sessionparams  = NULL;
    opaque_stubhelper stubhelper = NULL;
    etch_threadpool*  threadpool = NULL;
    int result = -1, session_id = 0;
    unsigned char async_mode = 0;

    /* get the message type object associated with this message */
    etch_type*  thistype = etch_message_type(msg);
    ETCH_ASSERT(thistype);
    ETCH_ASSERT(is_etch_stub(stubimpl));

    session_id = stubimpl->owner_id;
    do
    { 
  /* get the message type's stub helper (runner) function.
         * the stub helper is a pointer to a thread procedure function specific    
         * to the message type, which executes the associated service method.
         */
    if (NULL == (stubhelper = etchtype_get_type_stubhelper (thistype)))
    {   char* msgmask = "type '%s' missing stub runner procedure\n";
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, msgmask, thistype->aname);
        break;
    }

        /* get the thread pool, if any, appropriate for the 
         * configured mode of execution for this message type 
         */
    switch(async_mode = etchtype_get_async_mode (thistype))
    {   
        //default:	threadpool = thisx->params->qpool; break;
		case ETCH_ASYNCMODE_QUEUED: threadpool = thisx->params->qpool; break;
		case ETCH_ASYNCMODE_FREE:   threadpool = thisx->params->fpool; break;
        default: threadpool = NULL;
    }


    if(is_etch_client_stub(stubimpl)){
        result = etchstub_run (thisx, stubhelper, threadpool, ((etch_client_factory*)thisx->params)->iclient, whofrom, msg);
    }
    else{
         
            get_etch_session (thisx->params, session_id, &sessionparams);
            ETCH_ASSERT (sessionparams);
            serverimpl = sessionparams->server;

        /* execute the service method execution function (stub helper),  
         * either on a thread or inline. 
         */
            result = etchstub_run (thisx, stubhelper, threadpool, serverimpl, whofrom, msg);
        
    }
    } while(0);
    /* this is the end of the line for the message */
    

    return result;
}


/**
 * etchstub_session_control()
 * @param control event, caller relinquishes.
 * @param value control value, caller relinquishes.
 */
int etchstub_session_control (void* data, etch_event* control, etch_object* value)
{
  etch_stub* thisx = (etch_stub*)data;
    int result = -1, is_params_relinquished = FALSE;

    /* a server implementation (xxxx_server_impl) can request notifications of 
     * exceptions and the like by implementing and registering i_objsession 
     * callbacks. the presence of these function pointers in the implementation
     * serves as the indicator of whether to forward the notification.
     */
    i_objsession* impl_callbacks = thisx->impl_callbacks; 
 
    etch_session_control impl_session_control = impl_callbacks? 
        impl_callbacks->_session_control: NULL;     

    if (impl_session_control) /* if impl has requested this event, forward it */
        if (0 == (result = impl_session_control (impl_callbacks, control, value)))
            is_params_relinquished = TRUE;

    if (0 != result)
    {   char* logmask = impl_session_control? logmask1: logmask2;
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, logmask, scstr);
    }

    if (!is_params_relinquished) 
    {   
	    etch_object_destroy(control);
        control = NULL;

        etch_object_destroy(value);
        value = NULL;
    }

    return result;
}


/**
 * etch_etchstub_session_notify()
 * @param evt event, caller relinquishes.
 */
int etchstub_session_notify(void* data, etch_event* evt)
{
  etch_stub* thisx = (etch_stub*)data;
    const int result = etchstub_session_notify_ex (thisx, thisx->obj, evt);
    return result;
}


/**
 * etch_etchstub_session_query()
 * @param query, caller relinquishes.
 */
etch_object* etchstub_session_query (void* data, etch_query* query) 
{
  etch_stub* thisx = (etch_stub*)data;
   etch_object* resultobj = NULL;
   int is_params_relinquished = FALSE;

    /* a server implementation (xxxx_server_impl) can request notifications of 
     * exceptions and the like by implementing and registering i_objsession 
     * callbacks. the presence of these function pointers in the implementation
     * serves as the indicator of whether to forward the notification.
     */
    i_objsession* impl_callbacks = thisx->impl_callbacks;
 
    etch_session_query impl_session_query = impl_callbacks? 
        impl_callbacks->_session_query: NULL;

    if (impl_session_query) /* if impl has requested this event, forward it */
        if (NULL != (resultobj = impl_session_query (impl_callbacks, query)))
            is_params_relinquished = TRUE;

    if (NULL == resultobj)
    {   char* logmask = impl_session_query? logmask1: logmask2;
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, logmask, sqstr);
    }

    if (!is_params_relinquished) {
      etch_object_destroy(query);
      query = NULL;
    }

    return resultobj;
}


/* - - - - - - - - - - - - - - -  
 * constructors, destructor 
 * - - - - - - - - - - - - - - -  
 */ 

/**
 * destroy_stub_object()
 * stub implementation private destructor.
 * calls back to hand-coded destructor for possible custom deallocation,
 * then destroys the stub base, including the shared session interface.
 */
int destroy_stub_object (void* thisx)
{
    xxxx_either_stub* stubobj = NULL;
    if (NULL == thisx) return -1;

    stubobj = (xxxx_either_stub*) thisx;
    if (!is_etch_stub(thisx)) return -1;

    if (!is_etchobj_static_content(stubobj))
    {    
        if (stubobj->destroyex) /* call back to user dtor */
            stubobj->destroyex(thisx);

        if (stubobj->stub_base)
            etch_object_destroy(stubobj->stub_base);
    }
            
    return destroy_objectex((etch_object*)stubobj);
} 


/**
 * is_etch_stub()
 */
int is_etch_stub(void* x)
{
    int result = FALSE, objtype = x? ((etch_object*)x)->obj_type: 0;
    switch(objtype)
    { case ETCHTYPEB_STUB: case ETCHTYPEB_CLIENTSTUB: case ETCHTYPEB_SERVERSTUB:
           result = TRUE;
    }
    return result;
}


/**
 * new_stubimpl_init()
 * generic stub implementation constructor
 * @param implobj i_xxxx_client* or i_xxxx_server*.
 * @param objsize number of bytes in the stub object implementation. 
 * all we know about here is the mask, or object header if you will.
 * @param stubtype ETCH_STUBTYPE_CLIENT or ETCH_STUBTYPE_SERVER.
 * @param userdtor a hand-coded callback in the implementation conforming 
 * to signature etch_destructor, for the purpose of freeing any custom 
 * memory allocations.
 * @param ids the delivery service, caller retains.
 * @param qp the queued thread pool, optional, caller retains. 
 * @param fp the free thread pool, optional, caller retains. 
 * @param params a etch_server_factory* parameter bundle, caller retains.
 * if it is always the case that this parameter is present and is a
 * etch_server_factory*, we can lose the ids, qp, and fp ctor parameters.
 */
void* new_stubimpl_init (void* implobj, const int objsize, 
      const unsigned char stubtype, etch_object_destructor userdtor,
      i_delivery_service* ids, etch_threadpool* qp, etch_threadpool* fp, 
      void* params)  
{
    unsigned short obj_type, class_id;
    xxxx_either_stub* newstub = NULL;

    switch(stubtype)
    {  
        case ETCH_STUBTYPE_CLIENT:
             obj_type = ETCHTYPEB_CLIENTSTUB;
             class_id = CLASSID_CLIENTSTUB;
             break;
        case ETCH_STUBTYPE_SERVER:
             obj_type = ETCHTYPEB_SERVERSTUB;
             class_id = CLASSID_SERVERSTUB;
             break;
        default: return NULL;
    }

    newstub = (xxxx_either_stub*) new_object (objsize, obj_type, class_id);  
    ((etch_object*)newstub)->destroy   = destroy_stub_object; /* private */
    newstub->destroyex = userdtor;           /* public */
    newstub->stub_base = new_stub (implobj, stubtype, ids, qp, fp);
    newstub->stub_base->stubobj = (etch_object*) newstub;
    newstub->stub_base->params  = params;
 
    return newstub;
}


/**
 * new_clientstub_init()
 * generic client stub implementation constructor.
 * @param bytelen number of bytes in the client stub object implementation. 
 * all we know about here is the mask, or object header if you will.
 * @param dtor a hand-coded callback in the implementation conforming 
 * to signature etch_destructor, for the purpose of freeing any custom 
 * memory allocations.
 * @param ids the delivery service, caller retains.
 * @param qp the queued thread pool, optional, caller retains. 
 * @param fp the free thread pool, optional, caller retains. 
 * @param params a etch_server_factory* parameter bundle, caller retains.
 * if it is always the case that this parameter is present and is a
 * etch_server_factory*, we can lose the ids, qp, and fp ctor parameters.
 */
void* new_clientstub_init (void* implobj, const int bytelen, etch_object_destructor dtor, 
    i_delivery_service* ids, etch_threadpool* qp, etch_threadpool* fp, void* params)  
{
    return new_stubimpl_init (implobj, bytelen, ETCH_STUBTYPE_CLIENT, 
        dtor, ids, qp, fp, params);  
}


/**
 * new_serverstub_init()
 * generic server stub implementation constructor.
 * @param bytelen number of bytes in the server stub object implementation. 
 * all we know about here is the mask, or object header if you will.
 * @param dtor a hand-coded callback in the implementation conforming 
 * to signature etch_destructor, for the purpose of freeing any custom 
 * memory allocations.
 * @param ids the delivery service, caller retains.
 * @param qp the queued thread pool, optional, caller retains. 
 * @param fp the free thread pool, optional, caller retains. 
 * @param params a etch_server_factory* parameter bundle, caller retains.
 * if it is always the case that this parameter is present and is a
 * etch_server_factory*, we can lose the ids, qp, and fp ctor parameters.
 */
void* new_serverstub_init (void* implobj, const int bytelen, etch_object_destructor dtor, 
    i_delivery_service* ids, etch_threadpool* qp, etch_threadpool* fp, void* params)  
{
    return new_stubimpl_init (implobj, bytelen, ETCH_STUBTYPE_SERVER, 
        dtor, ids, qp, fp, params);  
}




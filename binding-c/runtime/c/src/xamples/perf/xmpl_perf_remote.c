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
 * xmpl_perf_remote.c 
 */

#include "apr_time.h" /* some apr must be included first */
#include "xmpl_perf_remote.h"
#include "etch_global.h"
#include "etch_url.h"  

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

int destroy_perf_remote (perf_remote*);
#if(0)
etch_message* etchremote_new_message(perf_remote*, etch_type*);
int etchremote_send(perf_remote*, etch_message*);
int etchremote_begincall(perf_remote*, etch_message*, void**);
int etchremote_endcall  (perf_remote*, i_mailbox*, etch_type*, void**);
int etchremote_transport_control (perf_remote*, etch_event*, etch_int32*);
int etchremote_transport_notify  (perf_remote*, etch_event*);
objmask* etchremote_transport_query (perf_remote*, objmask*); 
int etchremote_start_waitup  (perf_remote*, const int);
int etchremote_stop_waitdown (perf_remote*, const int);
#endif


/* - - - - - - - - - - - - - -  
 * constructors
 * - - - - - - - - - - - - - -  
 */

/**
 * new_perf_remote  
 * @param ids delivery service -- caller retains
 * @param vf perf value factory - caller retains 
 * @param iperf optional perf service interface -- caller retains
 */
perf_remote* new_perf_remote (void* thisx, 
    i_delivery_service* ids, etch_value_factory* vf, i_perf* iservice)
{
    perf_remote* remote = (perf_remote*) new_object (sizeof(perf_remote), 
        ETCHTYPEB_REMOTE, get_dynamic_classid_unique(&CLASSID_REMOTE_PERF));

    remote->destroy = destroy_perf_remote;

    /* perf_remote instance data and methods */
    remote->dsvc = ids; 
    remote->vf   = vf;
    remote->start_waitup  = etchremote_start_waitup;
    remote->stop_waitdown = etchremote_stop_waitdown;

    /* transport methods */
    remote->transport_control = etchremote_transport_control;
    remote->transport_notify  = etchremote_transport_notify;
    remote->transport_query   = etchremote_transport_query;

    /* remote base */
    remote->new_message = etchremote_new_message;
    remote->send        = etchremote_send;
    remote->sendex      = etchremote_sendex;
    remote->begin_call  = etchremote_begincall;
    remote->end_call    = etchremote_endcall;

    /* perf service */
    if (iservice)
        remote->iperf = iservice;
    else
    {   remote->iperf = new_perf_service_interface();
        remote->is_service_interface_owned = TRUE;
    }
    remote->add    = remote->iperf->add;
    remote->sum    = remote->iperf->sum;
    remote->report = remote->iperf->report;
    remote->dist   = remote->iperf->dist;
    remote->add2   = remote->iperf->add2;
    remote->report2= remote->iperf->report2;
    remote->point  = remote->iperf->point;
 
    return remote;
}


/**
 * destroy_perf_remote()
 * perf_remote destructor.
 */
int destroy_perf_remote (perf_remote* thisx)
{
    if (NULL == thisx) return -1;
    if (thisx->refcount > 0 && --thisx->refcount > 0) return -1;  

    if (!is_etchobj_static_content(thisx))
    {    
        if (thisx->is_service_interface_owned && thisx->iperf)
            thisx->iperf->destroy(thisx->iperf);
    }
            
    return destroy_objectex((objmask*)thisx);
}


/* - - - - - - - - - - - - - -  
 * remote methods
 * - - - - - - - - - - - - - -  
 */

/**
 * etchremote_new_message()
 * instantiates a message to be sent via this.send() or this.begin_call().
 * @param thisx this remote object.
 * @param message_type type of message, caller retains.
 * @return message object, which could wrap an exception.
 */
#if(0)
etch_message* etchremote_new_message (perf_remote* thisx, etch_type* message_type)
{
    etch_message* msg = new_message(message_type, ETCH_DEFSIZE, thisx->vf);
    return msg;
}
#endif


/**
 * etchremote_send()
 * sends message to recipient without waiting for a response.
 * @param thisx this remote object.
 * @param msg message, caller relinquishes.
 * @return 0 success, -1 failure.
 */
#if(0)
int etchremote_send (perf_remote* thisx, etch_message* msg)
{
    const int result = thisx->dsvc->itm->transport_message(thisx->dsvc, NULL, msg);
    return result;
}
#endif


/**
 * etchremote_begincall()
 * sends message beginning a call sequence.
 * @param thisx this remote object.
 * @param msg message, caller relinquishes.
 * @return in out parameter, a mailbox which can be used to retrieve the response.
 * @return 0 success, -1 failure.
 */
#if(0)
int etchremote_begincall (perf_remote* thisx, etch_message* msg, i_mailbox** out)
{
    const int result = thisx->dsvc->begin_call(thisx->dsvc, msg, out);
    return result;
}
#endif


/**
 * etchremote_endcall()
 * finishes a call sequence by waiting for a response message.
 * @param thisx this remote object.
 * @param mbox a mailbox which will be used to read an expected message response.
 * @param response_type the message type of the expected response.
 * @return in out parameter, on success, the response.
 * @return 0 success, -1 failure.
 */
#if(0)
int etchremote_endcall (perf_remote* thisx, i_mailbox* mbox, etch_type* response_type, objmask** out)
{
    const int result = thisx->dsvc->end_call(thisx->dsvc, mbox, response_type, out);
    return result;
}
#endif


/**
 * etchremote_transport_control()
 * @param evt caller relinquishes
 * @param value caller relinquishes
 * @return 0 success, -1 failure.
 */
#if(0)
int etchremote_transport_control (perf_remote* thisx, etch_event* evt, etch_int32* value)
{
    const int result = thisx->dsvc->itm->transport_control(thisx->dsvc, evt, value);
    return result;
}
#endif


/**
 * etchremote_transport_notify()
 * @param evt caller relinquishes
 * @return 0 success, -1 failure.
 */
#if(0)
int etchremote_transport_notify  (perf_remote* thisx, etch_event* evt)
{
    const int result = thisx->dsvc->itm->transport_notify(thisx->dsvc, evt);
    return result;
}
#endif


/**
 * etchremote_transport_query()
 * @param query caller relinquishes
 * @return 0 success, -1 failure.
 */
#if(0)
objmask* etchremote_transport_query (perf_remote* thisx, objmask* query) 
{
    objmask* resultobj = thisx->dsvc->itm->transport_query(thisx->dsvc, query);
    return resultobj;
}
#endif


/**
 * etchremote_start_waitup()
 * start the transport and wait for it to come up.
 * @param thisx this remote object.
 * @param waitms how long to wait, in milliseconds.
 * @return 0 success, -1 failure.
 */
#if(0)
int etchremote_start_waitup (perf_remote* thisx, const int waitms)
{
    const int result = thisx->transport_control(thisx, 
        new_etch_event(CLASSID_CONTROL_START_WAITUP, waitms), NULL);

    return result;
}
#endif


/**
 * etchremote_stop_waitdown()
 * stop the transport and wait for it to go down.
 * @param thisx this remote object.
 * @param waitms how long to wait, in milliseconds.
 * @return 0 success, -1 failure.
 */
#if(0)
int etchremote_stop_waitdown (perf_remote* thisx, const int waitms)
{
    const int result = thisx->transport_control(thisx, 
        new_etch_event(CLASSID_CONTROL_STOP_WAITDOWN, waitms), NULL);

    return result;
}
#endif

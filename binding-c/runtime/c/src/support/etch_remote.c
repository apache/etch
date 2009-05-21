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
 * etch_remote.c 
 * base methods for either type of remote
 */

#include "apr_time.h" /* some apr must be included first */
#include "etch_remote.h"
#include "etch_global.h"
#include "etchexcp.h"
#include "etch_url.h"  
#include "etchlog.h"

#include <tchar.h>
#include <stdio.h>
#include <conio.h>
char* ETCHREMO = "REMO";


/**
 * is_etch_remote()
 */
int is_etch_remote(void* x)
{
    int result = FALSE, objtype = x? ((objmask*)x)->obj_type: 0;
    switch(objtype)
    { case ETCHTYPEB_REMOTE: case ETCHTYPEB_REMOTECLIENT: case ETCHTYPEB_REMOTESERVER:
           result = TRUE;
    }
    return result;
}            


/**
 * etchremote_new_message()
 * instantiates a message to be sent via this.send() or this.begin_call().
 * @param thisx this remote object.
 * @param message_type type of message, caller retains.
 * @return message object, or an etch object wrapping an exception.
 */
etch_message* etchremote_new_message (xxxx_remote* thisx, etch_type* message_type)
{
    etch_message* msg = new_message (message_type, ETCH_DEFSIZE, thisx->vf);

    /* removed this throw because caller (remote) can't use the exception
     * without rethrowing it to the mailbox object */
    #if(0)
    if (NULL == msg) /* warning this is not a usable message object */
        msg = (etch_message*) throw_from(EXCPTYPE_ETCHRUNTIME, ETCHTYPEB_MESSAGE, 
            L"could not create message", ETCHEXCP_COPYTEXT);
    #endif

    return msg;
}


/**
 * etchremote_send()
 * sends message to recipient without waiting for a response.
 * @param thisx this remote object.
 * @param msg message, caller relinquishes.
 * @return 0 success, -1 failure.
 */
int etchremote_send (xxxx_remote* thisx, etch_message* msg)
{
    int result = 0;
    ETCH_ASSERT(is_etch_remote(thisx));
    
    result = thisx->dsvc->itm->transport_message(thisx->dsvc, NULL, msg);

    if (0 != result)
        etchlog(ETCHREMO, ETCHLOG_ERROR, "remote server send failed for msg %x\n", msg);

    return result;
}


/**
 * etchremote_sendex()
 * sends message to recipient without waiting for a response.
 * @param thisx this remote object.
 * @param msg message, caller relinquishes.
 * @return etch_int32 whose value is 0 success, -1 failure, caller owns it.
 * result object will wrap an exception if send was not successful.
 */
void* etchremote_sendex (xxxx_remote* thisx, etch_message* msg)
{
    etch_int32* resultobj = new_int32(0);

    if (0 != (resultobj->value = etchremote_send (thisx, msg)))
        etch_throw((objmask*)resultobj, EXCPTYPE_IO, 
            L"remote server send failed", ETCHEXCP_COPYTEXT);
    
    return resultobj;
}


/**
 * etchremote_begincall()
 * sends message beginning a call sequence.
 * @param thisx this remote object.
 * @param msg message, caller relinquishes.
 * @return in out parameter, a mailbox which can be used to retrieve the response.
 * @return 0 success, -1 failure.
 */
int etchremote_begincall (xxxx_remote* thisx, etch_message* msg, void** out)
{
    int result = 0;
    ETCH_ASSERT(is_etch_remote(thisx));

    result = thisx->dsvc->begin_call(thisx->dsvc, msg, out);

    if (0 != result) 
        etchlog(ETCHREMO, ETCHLOG_ERROR, "could not create mailbox for msg %x\n", msg);
         
    return result;
}


/**
 * etchremote_endcall()
 * finishes a call sequence by waiting for a response message.
 * @param thisx this remote object.
 * @param mbox a mailbox which will be used to read an expected message response.
 * @param response_type the message type of the expected response.
 * @return in out parameter, on success, the response objmask* masked object.
 * @return 0 success, -1 failure.
 */
int etchremote_endcall (xxxx_remote* thisx, i_mailbox* mbox, etch_type* response_type, void** out)
{   
    ETCH_ASSERT(is_etch_remote(thisx));

    return thisx->dsvc->end_call(thisx->dsvc, mbox, response_type, out);
}


/**
 * etchremote_transport_control()
 * @param evt caller relinquishes
 * @param value caller relinquishes
 * @return 0 success, -1 failure.
 */
int etchremote_transport_control (xxxx_remote* thisx, etch_event* evt, etch_int32* value)
{
    ETCH_ASSERT(is_etch_remote(thisx));

    return thisx->dsvc->itm->transport_control(thisx->dsvc, evt, value);
}


/**
 * etchremote_transport_notify()
 * @param evt caller relinquishes
 * @return 0 success, -1 failure.
 */
int etchremote_transport_notify  (xxxx_remote* thisx, etch_event* evt)
{
    ETCH_ASSERT(is_etch_remote(thisx));
    
    return thisx->dsvc->itm->transport_notify(thisx->dsvc, evt);
}


/**
 * etchremote_transport_query()
 * @param query caller relinquishes
 * @return 0 success, -1 failure.
 */
objmask* etchremote_transport_query (xxxx_remote* thisx, objmask* query) 
{
    ETCH_ASSERT(is_etch_remote(thisx));
    
    return thisx->dsvc->itm->transport_query(thisx->dsvc, query);
}


/**
 * etchremote_set_session()
 * @param session caller retains  
 * @return 0 success, -1 failure.
 */
void etchremote_set_session (xxxx_remote* thisx, void* session) 
{
    ETCH_ASSERT(is_etch_remote(thisx)); 
}


/**
 * etchremote_get_session()
 * @param session caller retains  
 * @return 0 success, -1 failure.
 */
void* etchremote_get_session (xxxx_remote* thisx)
{           
    ETCH_ASSERT(is_etch_remote(thisx)); 
    return thisx->dsvc->ism;
}


/**
 * etchremote_start_waitup()
 * start the transport and wait for it to come up.
 * @param thisx this remote object.
 * @param waitms how long to wait, in milliseconds.
 * @return 0 success, -1 failure.
 */
int etchremote_start_waitup (xxxx_remote* thisx, const int waitms)
{
    ETCH_ASSERT(is_etch_remote(thisx)); 
    ETCH_ASSERT(thisx->transport_control); 

    /* indicate to transport start whether this request is from a client 
    * (remote server) or a server (remote client) */
    {  const int is_client = thisx->remote_type == ETCH_REMOTETYPE_SERVER;
       etch_int32* txvalue = is_client? new_int32(is_client): NULL;
       etch_event* txevent = new_etch_event(CLASSID_CONTROL_START_WAITUP, waitms);

       return thisx->transport_control (thisx, txevent, txvalue);
    }
}


/**
 * etchremote_stop_waitdown()
 * stop the transport and wait for it to go down.
 * @param thisx this remote object.
 * @param waitms how long to wait, in milliseconds.
 * @return 0 success, -1 failure.
 */
int etchremote_stop_waitdown (xxxx_remote* thisx, const int waitms)
{
    ETCH_ASSERT(is_etch_remote(thisx)); 
    ETCH_ASSERT(thisx->transport_control); 

    /* indicate to transport start whether this request is from a client 
     * (remote server) or a server (remote client) */
    {  const int is_client = thisx->remote_type == ETCH_REMOTETYPE_SERVER;
       etch_int32* txvalue = is_client? new_int32(is_client): NULL;
       etch_event* txevent = new_etch_event(CLASSID_CONTROL_STOP_WAITDOWN, waitms);

       #ifdef ETCH_SHORTCIRCUIT_CLIENT_DEMO
       if (is_client)  /* dead end chain here for client demo */
       {   ETCHOBJ_DESTROY(txevent);   
           ETCHOBJ_DESTROY(txvalue);   
           return -1;                  
       }
       #endif  /* ETCH_SHORTCIRCUIT_CLIENT_DEMO */

       return thisx->transport_control (thisx, txevent, txvalue);
    }
}


/**
 * new_etch_remote_base
 * generic constructor for remote base 
 * @param thisx parent object such as remote server or client.
 * @param objsize byte length of actual remote base object,  
 * specifying ETCH_DEFSIZE defaults to sizeof(xxxx_remote).
 * @param class_id etch class id of this object.
 * @param ids delivery service -- caller retains.
 * @param vf default value factory -- caller retains. 
 * @param ixxxx service interface -- caller retains.
 * @return xxxx_remote* mask over either remote type.
 * fyi xxxx_remote typedefs to etch_remote.
 */
xxxx_remote* new_etch_remote_base (void* thisx, 
    const int objsize, const unsigned short class_id,  
    i_delivery_service* ids, etch_value_factory* vf, objmask* ixxxx)
{
    const int nbytes = objsize? objsize: sizeof(xxxx_remote);

    xxxx_remote* remote = (xxxx_remote*) new_object (nbytes, ETCHTYPEB_REMOTE, class_id);

    /* xxxx_remote instance data and methods */
    remote->dsvc = ids; 
    remote->vf   = vf;
    remote->start_waitup  = etchremote_start_waitup;
    remote->stop_waitdown = etchremote_stop_waitdown;

    /* transport methods */
    remote->transport_control = etchremote_transport_control;
    remote->transport_notify  = etchremote_transport_notify;
    remote->transport_query   = etchremote_transport_query;
    remote->set_session       = etchremote_set_session;
    remote->get_session       = etchremote_get_session;

    /* remote base */
    remote->new_message = etchremote_new_message;
    remote->send        = etchremote_send;
    remote->sendex      = etchremote_sendex;
    remote->begin_call  = etchremote_begincall;
    remote->end_call    = etchremote_endcall;
 
    return remote;
}




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

#include "etch_remote.h"
#include "etch_exception.h"
#include "etch_url.h"
#include "etch_log.h"
#include "etch_objecttypes.h"

static const char* LOG_CATEGORY = "etch_remote";

/**
 * is_etch_remote()
 */
int is_etch_remote(void* x)
{
    int result = FALSE, objtype = x? ((etch_object*)x)->obj_type: 0;
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
etch_message* etchremote_new_message (void* data, etch_type* message_type)
{
  xxxx_remote* thisx = (xxxx_remote*)data;
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
int etchremote_send (void* data, etch_message* msg)
{
    xxxx_remote* thisx = (xxxx_remote*)data;
    int result = 0;
    ETCH_ASSERT(is_etch_remote(thisx));
    
    result = thisx->dsvc->itm->transport_message(thisx->dsvc, NULL, msg);

    if (0 != result)
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "remote server send failed for msg %x\n", msg);

    return result;
}


/**
 * etchremote_sendex()
 * sends message to recipient without waiting for a response.
 * @param thisx this remote object.
 * @param msg message, caller relinquishes.
 * @return NULL or exception
 */
void* etchremote_sendex (void* data, etch_message* msg)
{
    xxxx_remote* thisx = (xxxx_remote*)data;
    etch_object* resultobj = NULL;

    if (0 != etchremote_send (thisx, msg)){
        etch_object_destroy(resultobj);
        resultobj = (etch_object*)new_etch_exception_from_errorcode(ETCH_EIO);

        etch_exception_set_message((etch_exception*)resultobj,new_stringw(L"remote server send failed"));
    }
    
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
int etchremote_begincall (void* data, etch_message* msg, void** out)
{
    xxxx_remote* thisx = (xxxx_remote*)data;
    int result = 0;
    ETCH_ASSERT(is_etch_remote(thisx));

    result = thisx->dsvc->begin_call(thisx->dsvc, msg, out);

    if (0 != result) 
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "could not create mailbox for msg %x\n", msg);
         
    return result;
}


/**
 * etchremote_endcall()
 * finishes a call sequence by waiting for a response message.
 * @param thisx this remote object.
 * @param mbox a mailbox which will be used to read an expected message response.
 * @param response_type the message type of the expected response.
 * @return in out parameter, on success, the response etch_object* masked object.
 * @return 0 success, -1 failure.
 */
int etchremote_endcall (void* data, i_mailbox* mbox, etch_type* response_type, void** out)
{   
    xxxx_remote* thisx = (xxxx_remote*)data;
    ETCH_ASSERT(is_etch_remote(thisx));

    return thisx->dsvc->end_call(thisx->dsvc, mbox, response_type, out);
}


/**
 * etchremote_transport_control()
 * @param evt caller relinquishes
 * @param value caller relinquishes
 * @return 0 success, -1 failure.
 */
int etchremote_transport_control (void* data, etch_event* evt, etch_object* value)
{
  xxxx_remote* thisx = (xxxx_remote*)data;
    ETCH_ASSERT(is_etch_remote(thisx));

    return thisx->dsvc->itm->transport_control(thisx->dsvc, evt, value);
}


/**
 * etchremote_transport_notify()
 * @param evt caller relinquishes
 * @return 0 success, -1 failure.
 */
int etchremote_transport_notify  (void* data, etch_event* evt)
{
  xxxx_remote* thisx = (xxxx_remote*)data;
    ETCH_ASSERT(is_etch_remote(thisx));
    
    return thisx->dsvc->itm->transport_notify(thisx->dsvc, evt);
}


/**
 * etchremote_transport_query()
 * @param query caller relinquishes
 * @return 0 success, -1 failure.
 */
etch_object* etchremote_transport_query (void* data, etch_query* query) 
{
  xxxx_remote* thisx = (xxxx_remote*)data;
    ETCH_ASSERT(is_etch_remote(thisx));
    
    return thisx->dsvc->itm->transport_query(thisx->dsvc, query);
}


/**
 * etchremote_set_session()
 * @param session caller retains  
 * @return 0 success, -1 failure.
 */
void etchremote_set_session (void* data, void* session) 
{
  xxxx_remote* thisx = (xxxx_remote*)data;
    ETCH_ASSERT(is_etch_remote(thisx)); 
}


/**
 * etchremote_get_session()
 * @param session caller retains  
 * @return 0 success, -1 failure.
 */
i_session* etchremote_get_session (void* data)
{           
  xxxx_remote* thisx = (xxxx_remote*)data;
    ETCH_ASSERT(is_etch_remote(thisx)); 
    return (i_session*)thisx->dsvc->ism;
}


/**
 * etchremote_start_waitup()
 * start the transport and wait for it to come up.
 * @param thisx this remote object.
 * @param waitms how long to wait, in milliseconds.
 * @return 0 success, -1 failure.
 */
int etchremote_start_waitup (void* data, const int waitms)
{
  xxxx_remote* thisx = (xxxx_remote*)data;
    ETCH_ASSERT(is_etch_remote(thisx)); 
    ETCH_ASSERT(thisx->transport_control); 

    /* indicate to transport start whether this request is from a client 
    * (remote server) or a server (remote client) */
    {  const int is_client = thisx->remote_type == ETCH_REMOTETYPE_SERVER;
       etch_int32* txvalue = is_client? new_int32(is_client): NULL;
       etch_event* txevent = new_etch_event(CLASSID_CONTROL_START_WAITUP, waitms);

       return thisx->transport_control (thisx, txevent, (etch_object*)txvalue);
    }
}


/**
 * etchremote_stop_waitdown()
 * stop the transport and wait for it to go down.
 * @param thisx this remote object.
 * @param waitms how long to wait, in milliseconds.
 * @return 0 success, -1 failure.
 */
int etchremote_stop_waitdown (void* data, const int waitms)
{
  xxxx_remote* thisx = (xxxx_remote*)data;
    ETCH_ASSERT(is_etch_remote(thisx)); 
    ETCH_ASSERT(thisx->transport_control); 

    /* indicate to transport start whether this request is from a client 
     * (remote server) or a server (remote client) */
    {  const int is_client = thisx->remote_type == ETCH_REMOTETYPE_SERVER;
       etch_int32* txvalue = is_client? new_int32(is_client): NULL;
       etch_event* txevent = new_etch_event(CLASSID_CONTROL_STOP_WAITDOWN, waitms);

       #ifdef ETCH_SHORTCIRCUIT_CLIENT_DEMO
       if (is_client)  /* dead end chain here for client demo */
       {   
	    etch_object_destroy(txevent);
	    txevent = NULL;

	    etch_object_destroy(txvalue);
	    txvalue = NULL;

        return -1;                  
       }
       #endif  /* ETCH_SHORTCIRCUIT_CLIENT_DEMO */

       return thisx->transport_control (thisx, txevent, (etch_object*)txvalue);
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
    i_delivery_service* ids, etch_value_factory* vf, etch_object* ixxxx)
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




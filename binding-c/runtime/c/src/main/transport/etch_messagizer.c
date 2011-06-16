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
 * etch_msgizer.c
 * messagizer accepts packets and translates them to messages,
 * and it accepts messages and translates them to packets.
 */

#include "etch_messagizer.h"
#include "etch_tdformat.h"
#include "etch_thread.h"
#include "etch_objecttypes.h"
#include "etch_log.h"

static const char* LOG_CATEGORY = "etch_messagizer";

// extern types
extern apr_pool_t* g_etch_main_pool;

int destroy_messagizer(void*);
int      etch_msgizer_transport_message(void*, void*, void*);
int      etch_msgizer_session_packet   (void*, void*, void*);
int      etch_msgizer_session_control  (void*, etch_event*, etch_object*);
int      etch_msgizer_session_notify   (void*, etch_event*);
etch_object* etch_msgizer_session_query    (void*, etch_query*); 
int      etch_msgizer_transport_control(void*, etch_event*, etch_object*);
int      etch_msgizer_transport_notify (void*, etch_event*);
etch_object* etch_msgizer_transport_query  (void*, etch_query*); 


/* - - - - - - - - - - - - - - -
 * etch_messagizer
 * - - - - - - - - - - - - - - -
 */

/**
 * new_messagizer()
 * etch_messagizer public constructor
 * @param ixport transport interface owned by caller
 * @param uri a URI string owned by caller.
 * @param resx a resources map owned by caller.
 */

etch_messagizer* new_messagizer (i_transportpacket* ixp, wchar_t* uri, etch_resources* resx)
{
    etch_url* url = new_url(uri);

    etch_messagizer* messagizer = new_messagizer_a(ixp, url, resx);

    etch_object_destroy(url);
    return messagizer;
}


/**
 * new_messagizer_a()
 * etch_messagizer private constructor
 * @param ipacket transport interface owned by caller
 * @param uri a URI string owned by caller.
 * @param resxmap a resources map owned by caller.
 */
etch_messagizer* new_messagizer_a (i_transportpacket* ixp, etch_url* url, etch_resources* resxmap)
{
    etch_status_t status = ETCH_SUCCESS;
    tagdata_format_factory* formatfactory = NULL;
    etch_messagizer* messagizer = NULL;
    i_transport* itransport = NULL;
    etch_value_factory* vf  = NULL;
    etch_string* formatobj  = NULL;
    i_session* isession = NULL;
    etch_mutex* mutex = NULL;
    int result = -1;

    do
    {
        formatobj = (etch_string*) etchurl_get_term (url, ETCH_RESXKEY_MSGIZER_FORMAT);

        if (NULL == formatobj && NULL != resxmap)
            formatobj = (etch_string*) etch_resources_get (resxmap, ETCH_RESXKEY_MSGIZER_FORMAT);

        if (NULL == formatobj) {
            ETCH_LOGW(LOG_CATEGORY, ETCH_LOG_ERROR, L"URI missing format specifier '%s'\n", ETCH_RESXKEY_MSGIZER_FORMAT); 
            break;
        }

        formatfactory = get_format_factory(formatobj->v.valw);
        if (NULL == formatfactory) {
            ETCH_LOGW(LOG_CATEGORY, ETCH_LOG_ERROR, L"no format '%s' is installed\n", 
            formatobj->v.valw); 
            break;
        }

        vf = resxmap? (etch_value_factory*) etch_resources_get(resxmap,
                          ETCH_RESXKEY_MSGIZER_VALUFACT): NULL;
        if (NULL == vf)
        {
            ETCH_LOGW(LOG_CATEGORY, ETCH_LOG_ERROR, L"no value factory '%s' is installed\n", ETCH_RESXKEY_MSGIZER_VALUFACT);
            break;
        }

        // TODO: pool
        status = etch_mutex_create(&mutex, ETCH_MUTEX_NESTED, NULL);
        if(status != ETCH_SUCCESS) {
            // error
            break;
        }

        /* - - - - - - - - - - - - - - -
         * etch_messagizer
         * - - - - - - - - - - - - - - -
         */
        messagizer = (etch_messagizer*) new_object(sizeof(etch_messagizer), ETCHTYPEB_MESSAGIZER, CLASSID_MESSAGIZER);

        ((etch_object*)messagizer)->destroy = destroy_messagizer;
        ((etch_object*)messagizer)->clone   = clone_null;
        messagizer->msglock = mutex;
        messagizer->msgbuf  = new_flexbuffer(ETCH_DEFSIZE);  /* 2K default */

        /* set our transport to that of next lower layer (packetizer) */
        messagizer->transport = ixp;  /* i_transportpacket owned by caller */

        /* - - - - - - - - - - - - - - -
         * i_transportmessage
         * - - - - - - - - - - - - - - -
         */
        itransport = new_transport_interface_ex (messagizer,  
            (etch_transport_control) etch_msgizer_transport_control, 
            (etch_transport_notify)  etch_msgizer_transport_notify, 
            (etch_transport_query)   etch_msgizer_transport_query,
             etch_msgizer_get_session, 
             etch_msgizer_set_session);

        /* instantiate i_transportmessage interface which messagizer implements */
        messagizer->transportmsg = new_transportmsg_interface(messagizer, 
            etch_msgizer_transport_message, 
            itransport);  /* transportmsg now owns itransport */

        /* copy i_transportmessage interface methods up to messagizer */
        messagizer->transport_message = etch_msgizer_transport_message;
        messagizer->transport_control = itransport->transport_control;
        messagizer->transport_notify  = itransport->transport_notify;
        messagizer->transport_query   = itransport->transport_query;
        messagizer->get_session       = itransport->get_session;
        messagizer->set_session       = itransport->set_session;     

        /* - - - - - - - - - - - - - - -
         * i_sessionpacket
         * - - - - - - - - - - - - - - -
         */
        isession = new_session_interface(messagizer,  
            (etch_session_control) etch_msgizer_session_control, 
            (etch_session_notify)  etch_msgizer_session_notify, 
            (etch_session_query)   etch_msgizer_session_query);

        /* instantiate i_sessionpacket interface which messagizer implements */
        messagizer->sessionpkt = new_sessionpkt_interface(messagizer, 
            etch_msgizer_session_packet, 
            isession);  /* transportmsg now owns isession */

        /* copy session interface to parent */
        messagizer->session_packet  = etch_msgizer_session_packet;
        messagizer->session_control = isession->session_control;
        messagizer->session_notify  = isession->session_notify;
        messagizer->session_query   = isession->session_query;

        /* finally set session of next lower layer to our session */
        /* fyi we must pass the implementor of transport as thisx, i.e. packetizer */
        messagizer->transport->set_session (messagizer->transport->thisx, messagizer->sessionpkt);

        /* - - - - - - - - - - - - - - -
         * tagged data in out
         * - - - - - - - - - - - - - - -
         */
        messagizer->tdi = formatfactory->new_tagdata_input(vf);
        if (NULL == messagizer->tdi) break;

        messagizer->tdo = formatfactory->new_tagdata_output(vf);
        if (NULL == messagizer->tdo) break;
        
        result = 0;

    } while(0);


    if (-1 == result) {   
        etch_object_destroy(vf);
        etch_object_destroy(messagizer);
        messagizer = NULL;
    } 

    etch_object_destroy(formatfactory);

    return messagizer;
}


/**
 * destroy_messagizer()
 * destructor for etch_messagizer
 */
int destroy_messagizer (void* data)
{
    etch_messagizer* thisx = (etch_messagizer*)data;
    if (!is_etchobj_static_content(thisx))
    {   
        etch_object_destroy(thisx->transportmsg);
        etch_object_destroy(thisx->sessionpkt);
        etch_object_destroy(thisx->tdi);
        etch_object_destroy(thisx->tdo);
        etch_object_destroy(thisx->msgbuf);
        etch_object_destroy(thisx->msglock);
        
    }

   return destroy_objectex((etch_object*) thisx);
}


/**
 * etch_msgizer_get_transport()
 * @return a reference to the messagizer transport interface.
 * this object is owned by whatever object created the messagizer.
 */
i_transportpacket* etch_msgizer_get_transport (etch_messagizer* thisx)
{
    ETCH_ASSERT(is_etch_messagizer(thisx));
    return thisx->transport;   
}


/* - - - - - - - - - - - - - - -
 * i_transportmessage
 * - - - - - - - - - - - - - - -
 */

/**
 * etch_msgizer_transport_message()
 * i_transportmessage::transport_message override.
 * serializes message and delivers its data to transport  
 * @param whoto recipient - caller retains this memory, can be null.
 * @param message the message
 * caller relinquishes this memory on success, retains on failure.  
 * @return 0 success, -1 error.
 */
int etch_msgizer_transport_message (void* data, void* whoData, void* messageData)
{

    etch_who* whoto = (etch_who*)whoData;
    etch_message* msg = (etch_message*)messageData;

    etch_messagizer* thisx = (etch_messagizer*)data;
    etch_status_t status = ETCH_SUCCESS;
    int result = 0;
    ETCH_ASSERT(is_etch_messagizer(thisx));

    status = etch_mutex_lock(thisx->msglock);
    ETCH_ASSERT(status == ETCH_SUCCESS);

    do
    {   const int headersize = thisx->transport->get_headersize (thisx->transport);

        etch_flexbuf_skip (thisx->msgbuf, headersize, TRUE);

        /* serialize the message to the buffer */
        result = ((struct i_tagged_data_output*)
                        (((etch_object*)thisx->tdo)->vtab))
                            ->write_message (thisx->tdo, msg, thisx->msgbuf);

        if (-1 == result) {
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR,"serialization of message to buffer failed");
            etch_flexbuf_reset(thisx->msgbuf);
            break;
        }

        etch_flexbuf_set_index (thisx->msgbuf, 0);


        /* deliver packet buffer to transport */    /* msgizer->itp->pktizer */
        result = thisx->transport->transport_packet (thisx->transport->thisx, whoto, thisx->msgbuf);

        if (0 == result) {
            etch_object_destroy(msg);
        }

    } while(0);

    status = etch_mutex_unlock(thisx->msglock);
    ETCH_ASSERT(status == ETCH_SUCCESS);

    return result;
}


/**
 * etch_msgizer_get_session()
 * @return a reference to the messagizer i_sessionmessage interface.
 * this object is owned by whatever object set the messagizer session.
 */
i_session* etch_msgizer_get_session (void* data) 
{
    etch_messagizer* thisx = (etch_messagizer*)data;
    ETCH_ASSERT(is_etch_messagizer(thisx));
    return (i_session*)thisx->session;
}


/**
 * etch_msgizer_set_session()
 * @param session an i_sessionmessage reference. caller owns this object.
 */
void etch_msgizer_set_session (void* data, void* sessionData)
{
    i_session* session = (i_session*)sessionData;
    etch_messagizer* thisx = (etch_messagizer*)data;
    ETCH_ASSERT(is_etch_messagizer(thisx));
    ETCH_ASSERT(is_etch_sessionmsg(session));  
    thisx->session = (i_sessionmessage*)session;
}


/**
 * etch_msgizer_transport_control()
 * i_transportmessage::transport_control override.
 * @param control event, caller relinquishes.
 * @param value control value, caller relinquishes.
 */
int etch_msgizer_transport_control (void* data, etch_event* control, etch_object* value)
{
  etch_messagizer* thisx = (etch_messagizer*)data;
    ETCH_ASSERT(is_etch_messagizer(thisx));
    /*     mzr    itp                           mzr    itp   packetizer */
    return thisx->transport->transport_control (thisx->transport->thisx, control, value);  
}


/**
 * etch_msgizer_transport_notify()
 * i_transportmessage::transport_notify override.
 * @param evt, caller relinquishes.
 */
int etch_msgizer_transport_notify (void* data, etch_event* evt)
{
  etch_messagizer* thisx = (etch_messagizer*)data;
    ETCH_ASSERT(is_etch_messagizer(thisx));
    return thisx->transport->transport_notify (thisx->transport->thisx, evt);
}


/**
 * etch_msgizer_transport_query()
 * i_transportmessage::transport_query override.
 * @param query, caller relinquishes.
 */
etch_object* etch_msgizer_transport_query (void* data, etch_query* query) 
{
    etch_messagizer* thisx = (etch_messagizer*)data;
    ETCH_ASSERT(is_etch_messagizer(thisx));
    return thisx->transport->transport_query (thisx->transport->thisx, query);
}


/* - - - - - - - - - - - - - - -
 * i_sessionpacket
 * - - - - - - - - - - - - - - -
 */
  
/**
 * etch_msgizer_session_packet()
 * i_sessionpacket::session_packet override.
 * delivers data to the session from the transport.
 * @param from from who sent the packet.
 * caller retains memory for this object, can be null.
 * @param buffer the packet from the packet source.
 * caller retains memory for this object. 
 * @return 0 success, -1 error (exception condition)
 */
int etch_msgizer_session_packet (void* data, void* whoData, void* bufferData)
{

    etch_who* whofrom = (etch_who*)whoData;
    etch_flexbuffer* fbuf = (etch_flexbuffer*)bufferData;
    etch_messagizer* thisx = (etch_messagizer*)data;
    etch_message* msg = NULL;
    int is_message_handled = FALSE;
    ETCH_ASSERT(is_etch_messagizer(thisx));

    /* create an etch message from the packetized data */
    msg = ((struct i_tagged_data_input*)((etch_object*)thisx->tdi)->vtab)->read_message (thisx->tdi, fbuf);
    
    /* send the new message up via session.
     * memory management rules are: if session_message() handles the message, 
     * it owns msg memory. otherwise (if not handled), msg memory is owned by  
     * the unwanted_message wrapper created here, which itself is owned by the 
     * session_notify() destination.
     */
    is_message_handled = (0 == thisx->session->session_message (thisx->session->thisx, whofrom, msg));
    
    /* if the message was not handled, e.g. the message is an exception returned 
     * from a one-way message and there was therefore no mailbox to receive it, 
     * forward the message to the session via mailbox manager.
     */
    if (!is_message_handled) {
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "unable to post message to a mailbox\n");
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "deferring '%s' to session\n", etch_message_aname(msg));
        thisx->session->session_notify(thisx->session->thisx, (etch_event*)new_unwanted_message(whofrom, msg));
        //etch_object_destroy(msg);
        return -1;
    }

    /* regardless we have relinquished msg at this point. it is now either queued up 
     * in a mailbox, or forwarded in the unwanted message wrapper above. */
    return 0;
}


/**
 * etch_msgizer_session_control()
 * i_sessionpacket::session_control override.
 * @param control event, caller relinquishes.
 * @param value control value, caller relinquishes.
 */
int etch_msgizer_session_control (void* data, etch_event* control, etch_object* value)
{
  etch_messagizer* thisx = (etch_messagizer*)data;
    ETCH_ASSERT(is_etch_messagizer(thisx));       /* ism    mbmgr */
    return thisx->session->session_control (thisx->session->thisx, control, value);
}


/**
 * etch_msgizer_session_notify()
 * i_sessionpacket::session_notify override.
 * @param event, caller relinquishes.
 */
int etch_msgizer_session_notify  (void* data, etch_event* evt)
{
  etch_messagizer* thisx = (etch_messagizer*)data;
    ETCH_ASSERT(is_etch_messagizer(thisx));      /* ism    mbmgr */
    return thisx->session->session_notify (thisx->session->thisx, evt);
}


/**
 * etch_msgizer_session_query()
 * i_sessionpacket::session_query override.
 * @param query, caller relinquishes.
 */
etch_object* etch_msgizer_session_query (void* data, etch_query* query) 
{
    etch_messagizer* thisx = (etch_messagizer*)data;
    ETCH_ASSERT(is_etch_messagizer(thisx));     /* ism    mbmgr */
    return thisx->session->session_query (thisx->session->thisx, query);
}


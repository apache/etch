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
 * etch_plainmailboxmgr.c
 * mailbox manager accepts packets for potential delivery to a mailbox or to an
 * alternate message handler if an appropriate mailbox is not available. 
 * a mailbox can be created on request keyed by the a message's ID.
 */

#include "apr_time.h" /* some apr must be included first */
#include "etch_plainmboxmgr.h"
#include "etch_plainmailbox.h"
#include "etch_syncobj.h"
#include "etch_global.h"
#include "etchthread.h"
#include "etchlog.h"
char* ETCHPMBM = "PMBM";

etch_plainmailboxmgr* new_plain_mailbox_manager_a(i_transportmessage*, const int, etchmutex*); 
int destroy_plainmailboxmgr(etch_plainmailboxmgr*);
etch_hashtable* new_pmboxmgr_mailboxmap();

int      pmboxmgr_transport_call   (i_mailbox_manager*, etch_who*, etch_message*, void**); 
int      pmboxmgr_redeliver        (i_mailbox_manager*, etch_who*, etch_message*); 
int      pmboxmgr_unregister       (i_mailbox_manager*, i_mailbox*);
int      pmboxmgr_transport_message(etch_plainmailboxmgr*, etch_who*, etch_message*); 
int      pmboxmgr_session_message  (etch_plainmailboxmgr*, etch_who*, etch_message*);
int      pmboxmgr_session_control  (etch_plainmailboxmgr*, etch_event*, void*);
int      pmboxmgr_session_notify   (etch_plainmailboxmgr*, etch_event*);
objmask* pmboxmgr_session_query    (etch_plainmailboxmgr*, objmask*); 
int      pmboxmgr_transport_control(etch_plainmailboxmgr*, etch_event*, void*);
int      pmboxmgr_transport_notify (etch_plainmailboxmgr*, etch_event*);
objmask* pmboxmgr_transport_query  (etch_plainmailboxmgr*, objmask*); 
void pmboxmgr_set_session(etch_plainmailboxmgr*, i_sessionmessage*);
i_sessionmessage* pmboxmgr_get_session(etch_plainmailboxmgr*); 


/* - - - - - - - - - - - - - - -
 * ctor/dtor
 * - - - - - - - - - - - - - - -
 */

/**
 * new_plain_mailbox_manager()
 * etch_plainmailboxmgr public constructor
 * @param itm i_transportmessage interface object, caller retains ownership.
 * @param uri url string, caller relinquishes ownership.
 * @param resxmap caller retains ownership.
 * @param rwlock global mutex for mailbox read write sync, caller retains.
 */
etch_plainmailboxmgr* new_plain_mailbox_manager (i_transportmessage* itm, 
    wchar_t* uri, etch_resources* resxmap, etchmutex* rwlock) 
{
    etch_plainmailboxmgr* mgr = new_plain_mailbox_manager_a(itm, ETCH_INFWAIT, rwlock);
    if (NULL == mgr) return NULL;

    /* TODO instantiate this mutex in service initialization 
     * and destroy it in service shutdown */
    if (NULL == g_atomiclock)
        g_atomiclock = new_mutex (etch_apr_mempool, ETCHMUTEX_UNNESTED);
    time (&message_id_farm);  

    return mgr;
}


/**
 * new_plain_mailbox_manager_a()
 * etch_plainmailboxmgr private constructor
 * @param itm transportmessage interface, not owned.
 * @param maxdelay maximum time in milliseconds to wait for a full mailbox
 * @param rwlock global mutex for mailbox read write sync, caller retains.
 * to become unfull before giving up and delivering to the message handler. 
 * ETCH_INFWAIT indicates wait forever, ETCH_NOWAIT indicates do not block.
 */
etch_plainmailboxmgr* new_plain_mailbox_manager_a (i_transportmessage* itm, 
    const int maxdelay, etchmutex* rwlock)
{
    etch_plainmailboxmgr* mgr = NULL;
    i_mailbox_manager* imgr = NULL;
    etch_hashtable* mboxmap = NULL;
    i_transport* itransport = NULL;
    i_session* isession = NULL;
    etchmutex* mutex = NULL;  /* not used - lose it */
    int result = -1;

    do
    {   if (NULL == (mutex   = new_mutex(etch_apr_mempool, ETCHMUTEX_NESTED))) break;
        if (NULL == (mboxmap = new_pmboxmgr_mailboxmap())) break;

        /* - - - - - - - - - - - - - - -
         * etch_plainmailboxmgr
         * - - - - - - - - - - - - - - -
         */
        mgr = (etch_plainmailboxmgr*) new_object
           (sizeof(etch_plainmailboxmgr), ETCHTYPEB_MBOXMGR_IMPL, CLASSID_PLAIN_MBOXMGR);

        mgr->destroy = destroy_plainmailboxmgr;
        mgr->clone   = clone_null;
        mgr->xlock   = mutex;   /* not used - lose it */
        mgr->rwlock  = rwlock;  /* not owned */
        mgr->mailboxes = mboxmap;
        mgr->max_delay = maxdelay;

        /* set our transport to that of next lower layer (messagizer) */
        mgr->transport = itm;  /* not owned */
        /* note that mgr::transport::session has not been set yet. it will be set  
         * to mgr::session in mgr::set_session(), assuming that code invokes it.
         * it is safer to leave mgr::transport::session null, than to point it to
         * a session which is null. this effectively precludes code from setting
         * mgr::session directly; code should use mgr::set_session() to do so.
         */

        /* - - - - - - - - - - - - - - -
         * i_transportmessage (forward)
         * - - - - - - - - - - - - - - -
         */
        itransport = new_transport_interface (mgr,  
            (etch_transport_control) pmboxmgr_transport_control, 
            (etch_transport_notify)  pmboxmgr_transport_notify, 
            (etch_transport_query)   pmboxmgr_transport_query);
        itransport->get_session    = pmboxmgr_get_session;
        itransport->set_session    = pmboxmgr_set_session;

        /* instantiate i_transportmessage interface which messagizer implements */
        mgr->transportmsg = new_transportmsg_interface (mgr, 
            pmboxmgr_transport_message, 
            itransport);  /* transportmsg now owns itransport */

        /* copy i_transportmessage interface methods up to messagizer */
        mgr->transport_message = pmboxmgr_transport_message;
        mgr->transport_control = itransport->transport_control;
        mgr->transport_notify  = itransport->transport_notify;
        mgr->transport_query   = itransport->transport_query;
        mgr->get_session       = itransport->get_session;
        mgr->set_session       = itransport->set_session;  

        /* - - - - - - - - - - - - - - -
         * i_mailbox_manager 
         * - - - - - - - - - - - - - - -
         */
        imgr = new_mailboxmgr_interface (mgr, mgr->transportmsg, mgr->session);
        mgr->imanager = imgr;
        imgr->rwlock  = rwlock;

        imgr->transport_call = mgr->transport_call = pmboxmgr_transport_call;
        imgr->redeliver      = mgr->redeliver      = pmboxmgr_redeliver;
        imgr->unregister     = mgr->unregister     = pmboxmgr_unregister; 

        /* i_mailbox_manager::i_transport */
        imgr->transport_message = mgr->transport_message = pmboxmgr_transport_message;
        imgr->transport_control = mgr->transport_control = pmboxmgr_transport_control;
        imgr->transport_notify  = mgr->transport_notify  = pmboxmgr_transport_notify;
        imgr->transport_query   = mgr->transport_query   = pmboxmgr_transport_query;  
        imgr->get_session       = mgr->get_session       = pmboxmgr_get_session; 
        imgr->set_session       = mgr->set_session       = pmboxmgr_set_session;  

        /* - - - - - - - - - - - - - - -
         * i_sessionmessage (backward) 
         * - - - - - - - - - - - - - - -
         */
        isession = new_session_interface(mgr,  
            (etch_session_control) pmboxmgr_session_control, 
            (etch_session_notify)  pmboxmgr_session_notify, 
            (etch_session_query)   pmboxmgr_session_query);

        /* instantiate the i_sessionmessage interface which mbox mgr implements */
        mgr->isessionmsg = new_sessionmsg_interface(mgr, 
            pmboxmgr_session_message, 
            isession);  /* isessionmsg now owns isession */

        imgr->ism = mgr->isessionmsg;  /* relinquish isessionmsg to imgr */

        /* copy sessionmsg interface methods to parent */
        mgr->session_message = imgr->session_message = pmboxmgr_session_message;
        mgr->session_control = imgr->session_control = isession->session_control;
        mgr->session_notify  = imgr->session_notify  = isession->session_notify;
        mgr->session_query   = imgr->session_query   = isession->session_query;

        /* finally set session of next lower layer (messagizer) to our session */
        /* fyi mgr->transport->thisx is messagizer, the transport implementor */
        if (mgr->transport) /* transport always supplied except by unit test */
            mgr->transport->set_session (mgr->transport->thisx, mgr->isessionmsg);  

        result = 0;    

    } while(0);

    if (-1 == result)
    {
        if (mutex)   mutex->destroy(mutex);
        if (mboxmap) mboxmap->destroy(mboxmap);
        if (imgr)    imgr->destroy(imgr);
        if (mgr)     etch_free(mgr);
        mgr = NULL;
    } 

    return mgr;
}


/**
 * destroy_plainmailboxmgr()
 * destructor for etch_plainmailboxmgr
 */
int destroy_plainmailboxmgr(etch_plainmailboxmgr* thisx)
{
    if (thisx->refcount > 0 && --thisx->refcount > 0) return -1;  

    if (!is_etchobj_static_content(thisx))
    {               
        ETCHOBJ_DESTROY(thisx->xlock);

        ETCHOBJ_DESTROY(thisx->mailboxes);     

        ETCHOBJ_DESTROY(thisx->imanager);

        /* todo we'd like to instantiate this mutex in service initialization and   
         * destroy in service shutdown. problem is that etch_mutex is not global. */
        ETCHOBJ_DESTROY(g_atomiclock); 
    }

   return destroy_objectex((objmask*) thisx);
}


/* - - - - - - - - - - - - - - -
 * i_sessionmessage
 * - - - - - - - - - - - - - - -
 */

 /**
 * get_etchmbm_session()
 * convenience method to validate the etch_plainmailboxmgr, get, validate,
 * and return the etch_plainmailboxmgr's session.
 */
i_sessionmessage* get_etchmbm_session (etch_plainmailboxmgr* mgr)
{
    i_sessionmessage* session = NULL;
    ETCH_ASSERT(is_etch_mailboxmgr(mgr));
    session = mgr->session;
    ETCH_ASSERT(is_etch_sessionmsg(session));
    return session;
}   


/**
 * pmboxmgr_session_message()
 * queue specified message to the message's mailbox.
 * @param whofrom caller retains, can be null.
 * @param msg caller abandons on success (0), retains on other than success.
 * @return 0 (message handled), or -1 (error, mailbox closed, or timeout)  
 */
int pmboxmgr_session_message (etch_plainmailboxmgr* mgr, etch_who* whofrom, etch_message* msg)
{
    int result = 0;
    i_mailbox* ibox = NULL;
    etch_int64* msgid = NULL; 
    i_sessionmessage* session = get_etchmbm_session (mgr);

    if (NULL == (msgid = message_get_in_reply_to (msg)))  /* msgid not ours */
        return session->session_message (session->thisx, whofrom, msg);  

    /* fyi no mailbox will exist for a one-way message */
    ibox = pmboxmgr_get_mailbox (mgr, msgid);

    /* todo ensure that caller destroys message on error return */
    if (NULL == ibox || ibox->is_closed(ibox)) return -1;

    result = ibox->message (ibox, whofrom, msg);  

    if (0 != result)  /* ETCH_MAILBOX_TIMEOUT (-2) or mailbox closed (-1) */
        result = -1;
 
    return result;
}


/**
 * pmboxmgr_session_control()
 * i_sessionmsg::session_control override.
 * @param control event, caller relinquishes.
 * @param value control value, caller relinquishes.
 */
int pmboxmgr_session_control (etch_plainmailboxmgr* mgr, etch_event* control, objmask* value)
{
    i_sessionmessage* session = get_etchmbm_session (mgr);
    return session->session_control (session->thisx, control, value);
}


/**
 * pmboxmgr_session_notify()
 * i_sessionmsg::session_notify override.
 * @param event, caller relinquishes.
 */
int pmboxmgr_session_notify (etch_plainmailboxmgr* mgr, etch_event* evt)
{
    int result = 0;
    const int event_type = evt->value;
    i_sessionmessage* session = get_etchmbm_session (mgr);

    switch(event_type)
    {   case ETCHEVT_SESSION_UP:   
             mgr->is_connection_up = TRUE; 
             break;
        case ETCHEVT_SESSION_DOWN: 
             mgr->is_connection_up = FALSE; 
             result = pmboxmgr_unregister_all (mgr);
    }

    return session->session_notify (session->thisx, evt);
}


/**
 * pmboxmgr_session_query()
 * i_sessionmsg::session_query override.
 * @param query, caller relinquishes.
 */
objmask* pmboxmgr_session_query (etch_plainmailboxmgr* mgr, objmask* query) 
{
    i_sessionmessage* session = get_etchmbm_session (mgr);
    return session->session_query (session->thisx, query);
}


/* - - - - - - - - - - - - - - -
 * i_transportmessage
 * - - - - - - - - - - - - - - -
 */

/**
 * get_etchmbm_transport()
 * convenience method to validate the etch_plainmailboxmgr, get, validate,
 * and return the etch_plainmailboxmgr's transport.
 */
i_transportmessage* get_etchmbm_transport(etch_plainmailboxmgr* mgr)
{
    i_transportmessage* transport = NULL;
    ETCH_ASSERT(is_etch_mailboxmgr(mgr));
    transport = mgr->transport;
    ETCH_ASSERT(is_etch_transportmsg(transport));
    return transport;
}


/**
 * pmboxmgr_transport_message()
 * i_transportmessage::transport_message override.
 * @param whoto, caller retains, can be null.
 * @param msg caller abandons on success, retains on failure.
 */
int pmboxmgr_transport_message (etch_plainmailboxmgr* mgr, etch_who* whoto, etch_message* msg) 
{
    int result = 0;
    i_transportmessage* transport = get_etchmbm_transport(mgr);

    etch_int64* msgid = message_get_id(msg);  /* expect NULL (not yet sent) */
    if (NULL != msgid) return -1;  /* already sent */

    msgid  = new_int64(etch_generate_message_id()); /* assign ID to message */
    result = message_set_id(msg, msgid);  /* relinquish msgid object */

    if (0 == result)  
        result = mgr->transport->transport_message (mgr->transport->thisx, whoto, msg);

    return result;
}


/**
 * pmboxmgr_transport_control()
 * i_transportmessage::transport_control override.
 * @param control event, caller relinquishes.
 * @param value control value, caller relinquishes.
 */
int pmboxmgr_transport_control (etch_plainmailboxmgr* mgr, etch_event* control, objmask* value)
{
    i_transportmessage* transport = get_etchmbm_transport(mgr);
    return transport->transport_control (transport->thisx, control, value);   
}


/**
 * pmboxmgr_transport_notify()
 * i_transportmessage::transport_notify override.
 * @param evt, caller relinquishes.
 */
int pmboxmgr_transport_notify (etch_plainmailboxmgr* mgr, etch_event* evt)
{
    i_transportmessage* transport = get_etchmbm_transport(mgr);
    return transport->transport_notify(transport->thisx, evt);
}


/**
 * pmboxmgr_transport_query()
 * i_transportmessage::transport_query override.
 * @param query, caller relinquishes.
 */
objmask* pmboxmgr_transport_query (etch_plainmailboxmgr* mgr, objmask* query) 
{
    i_transportmessage* transport = get_etchmbm_transport(mgr);
    return transport->transport_query(transport->thisx, query);
}


/**
 * pmboxmgr_get_session()
 * @return a reference to the messagizer i_sessionmessage interface.
 * this object is owned by whatever object set the messagizer session.
 */
i_sessionmessage* pmboxmgr_get_session (etch_plainmailboxmgr* mgr) 
{
    ETCH_ASSERT(is_etch_mailboxmgr(mgr));
    return mgr->session;
}


/**
 * pmboxmgr_set_session()
 * @param newsession an i_sessionmessage reference. caller owns this object.
 * this will be invoked from delivery service constructor.
 */
void pmboxmgr_set_session (etch_plainmailboxmgr* thisx, i_sessionmessage* newsession)
{
    i_transportmessage* transport = NULL;
    ETCH_ASSERT(is_etch_mailboxmgr(thisx));
    ETCH_ASSERT(is_etch_sessionmsg(newsession));

    /* set our session to passed (delivery service's) session */
    thisx->session = newsession;   
}


/* - - - - - - - - - - - - - - -
 * i_mailbox_manager
 * - - - - - - - - - - - - - - -
 */

/**
 * pmboxmgr_transport_call()
 */
 int pmboxmgr_transport_call (i_mailbox_manager* imgr, etch_who* whoto, 
    etch_message* msg, i_mailbox** out) 
{
    int result = 0;
    i_mailbox* ibox = NULL;
    etch_plainmailbox* mbox = NULL;
    etch_plainmailboxmgr* mgr = NULL;
    etch_int64 *msgid = NULL, *inreplyto = NULL;
    const int MAXMESSAGES_ONE = 1, lifetime = MBOX_LIFETIME_UNTIL_CLOSE;
    ETCH_ASSERT(is_etch_imailboxmgr(imgr));
    mgr = imgr->thisx;
    ETCH_ASSERT(is_etch_mailboxmgr(mgr));

    msgid     = message_get_id(msg);   /* expect NULL (not yet sent) */
    inreplyto = message_get_in_reply_to(msg);
    if (NULL != msgid)     return -1;  /* already sent */
    if (NULL != inreplyto) return -1;  /* marked as reply */

    msgid  = new_int64(etch_generate_message_id());
    result = message_set_id(msg, msgid);  /* assign ID to message */
    if (0 != result) return -1;

    mbox = new_mailbox (imgr, msgid->value, mgr->max_delay, lifetime, MAXMESSAGES_ONE);

    if (mbox) ibox = mbox->imailbox;
    if (NULL == ibox) return -1;
 
    result = pmboxmgr_register_mailbox (mgr, ibox);

    /* currently lifetime is always zero (until explictly closed); however if
     * lifetime were to be > 0, a timer would enter the picture, and since we
     * do not currently have a proper timer pool, our simple timer thread   
     * might need a start cushion.
     */
    if (0 == result && lifetime > 0)
        etch_sleep(100);  

    if (0 == result)  
        result = mgr->transport->transport_message (mgr->transport->thisx, whoto, msg);

    if (0 == result)
        if (out) 
            *out = ibox;
        else;
    else 
        pmboxmgr_unregister(imgr, ibox);

    return result;
}


/**
 * pmboxmgr_redeliver()
 * forwards specified message
 */
int pmboxmgr_redeliver (i_mailbox_manager* imgr, etch_who* whofrom, etch_message* msg) 
{
    etch_iterator iterator;
    etch_hashtable* map = NULL;
    etch_plainmailboxmgr* mgr = NULL;
    ETCH_ASSERT(is_etch_imailboxmgr(imgr));
    mgr = imgr->thisx;
    ETCH_ASSERT(is_etch_mailboxmgr(mgr));

    map = mgr->mailboxes;
    hashtable_getlock(map);
    set_iterator(&iterator, map, &map->iterable);

    while(iterator.has_next(&iterator))
    {   /* TODO if change i_mailbox to be objmask based, check class_id here */
        i_mailbox* ibox = (i_mailbox*) iterator.current_value;
        if (ibox)
            ibox->close_delivery(ibox);

        iterator.next(&iterator);
    }

    hashtable_rellock(map);
    return 0;
}


/**
 * pmboxmgr_unregister()
 * remove specified mailbox from manager.
 * the unregistered box is *not* destroyed - whoever registered it owns it.
 */
int pmboxmgr_unregister (i_mailbox_manager* imgr, i_mailbox* ibox)
{
    int64 msgid;   
    unsigned hashval;
    etch_hashtable* map;
    etch_int64* itemkey;
    etch_hashitem hi, *removed = &hi; 
    etch_plainmailboxmgr* mgr = imgr? imgr->thisx: NULL;
    if (NULL == mgr) return -1;

    map   = mgr->mailboxes;
    msgid = ibox->get_message_id(ibox);
    hashval = etchhash(&msgid, sizeof(int64), 0);

    if (-1 == map->vtab->removeh (map->realtable, hashval, map, &removed))
        return -1;

    itemkey = (etch_int64*) removed->key;
    ETCH_ASSERT(is_etch_int64(itemkey));
    etch_free(itemkey);  /* once we remove the entry, we own the key */
    return 0;
}


/* - - - - - - - - - - - - - - 
 * etch_plaingmboxmgr public 
 * - - - - - - - - - - - - - - 
 */

/**
 * pmboxmgr_register_mailbox()
 * add specified mailbox to set of mailboxes receiving responses to messages.
 * @param ibox caller retains ownership. 
 * TODO does mgr need to own and destroy mailboxes? if so, i_mailbox
 * destructor must destroy its parent object. maybe it needs to do this anyway.
 */
int pmboxmgr_register_mailbox(etch_plainmailboxmgr* mgr, i_mailbox* ibox)
{
    int result = -1;

    if (mgr->is_connection_up)
    {   etch_hashtable* map = mgr->mailboxes;
        int64 message_id  = ibox->get_message_id(ibox);
        etch_int64* idobj = new_int64(message_id);
        /* insert mailbox to synchronized map */
        result = map->vtab->inserth(map->realtable, idobj, ibox, map, 0);
        if (-1 == result) etch_free(idobj); /* duplicate id */
    }
    else etchlog (ETCHPMBM, ETCHLOG_ERROR, "attempt to register disconnected mailbox\n");

    return result;
}


/**
 * pmboxmgr_get_mailbox()
 * return interface to mailbox having specified message id, or NULL.
 */
 i_mailbox* pmboxmgr_get_mailbox(etch_plainmailboxmgr* mgr, etch_int64* msgid) 
{
    etch_hashtable* map = mgr->mailboxes;
    etch_hashitem hi, *entry = &hi; 

    const int result = map->vtab->findh(map->realtable, msgid->hashkey, map, &entry);
    return result == 0? (i_mailbox*) entry->value: NULL;
}


/**
 * pmboxmgr_unregister_all()
 * unregisters all mailboxes
 */
 int pmboxmgr_unregister_all(etch_plainmailboxmgr* mgr) 
{
    etch_hashtable* map = mgr->mailboxes;
    etch_iterator iterator;
    hashtable_getlock(map);
    set_iterator(&iterator, map, &map->iterable);

    while(iterator.has_next(&iterator))
    {
        i_mailbox* ibox = (i_mailbox*) iterator.current_value;
        if (ibox)
            ibox->close_delivery(ibox);

        iterator.next(&iterator);
    }

    hashtable_rellock(map);
    return 0;
}


/**
 * pmboxmgr_size()
 * return count of registered mailboxes
 */
 int pmboxmgr_size(etch_plainmailboxmgr* mgr) 
{
    etch_hashtable* map = mgr->mailboxes;
    return map->vtab->count(map->realtable, map, 0);
}


/* - - - - - - - - - - - - - - -
 * etch_plaingmboxmgr private
 * - - - - - - - - - - - - - - -
 */
 
/**
 * new_pmboxmgr_mailboxmap()
 * construct and return a hashtable configured as expected for the mailbox map.
 * this map's destructor will destroy neither key objects nor value objects.  
 */
etch_hashtable* new_pmboxmgr_mailboxmap()
{
    etch_hashtable* map = new_synchronized_hashtable(ETCHMBMGR_DEFNUMMAILBOXES); 
    map->content_type = ETCHHASHTABLE_CONTENT_OBJECT_OBJECT;
    map->is_readonly_keys = FALSE;  /* owns key objects */
    map->is_readonly_values = TRUE; /* does not own mailbox */
    return map;
}


/**
 * etch_generate_message_id()
 * atomically generate a new message ID
 */
int64 etch_generate_message_id()
{
    if (g_atomiclock) g_atomiclock->acquire(g_atomiclock);
    message_id_farm++;
    if (g_atomiclock) g_atomiclock->release(g_atomiclock);
    return message_id_farm;
}

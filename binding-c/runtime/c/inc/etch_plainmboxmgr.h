/*
 * etch_plainmailboxmgr.h
 * mailbox manager accepts packets for potential delivery to a mailbox or to an
 * alternate message handler if an appropriate mailbox is not available. 
 * a mailbox can be created on request keyed by the a message's ID.
 */

#ifndef ETCHPLAINMBOXMGR_H
#define ETCHPLAINMBOXMGR_H

#include "etch_mailboxmgr.h"
#include "etch_resources.h"
#include "etchmutex.h"
#include "etch_url.h"

#define ETCHMBMGR_DEFNUMMAILBOXES 8

#if(0)

 PLAINMAILBOXMGR 
 |  register(mailbox)
 |  unregister(mailbox)
 |  unregisterAll();
 |  getMailbox(messageID);
  - MAILBOXMANAGER
    |  transportCall(Who to, message)  
    |  redeliver(Who from, message)
    |  unregister(mailbox)
    |- SESSIONMESSAGE<SessionData>
    |  |  sessionMessage(Who from, message)  
    |   - SESSION
    |       sessionQuery(); sessionControl(); sessionNotify();
     - TRANSPORTMESSAGE  
       |  transportMessage(to, Message);
        - TRANSPORT
             transportQuery(); transportControl(); transportNotify();
             getSession(); setSession();
#endif

typedef struct etch_plainmailboxmgr etch_plainmailboxmgr;
typedef etch_plainmailboxmgr etch_mailbox_manager;


/*
 * etch_plainmailboxmgr
 */
typedef struct etch_plainmailboxmgr
{
    unsigned int    hashkey;  
    unsigned short  obj_type; 
    unsigned short  class_id;
    struct objmask* vtab;  
    int  (*destroy)(void*);
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;
    struct objmask* parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;

    int  max_delay;
    etchmutex* xlock;  /* owned - unused */
    etchmutex* rwlock; /* not owned */
    etch_hashtable* mailboxes; /* owned */
    unsigned char is_connection_up;

    /* - - - - - - - - - - - - - - -
     * i_mailbox_manager interface
     * - - - - - - - - - - - - - - -
     */
    i_mailbox_manager*      imanager; /* owned */
    etch_mbm_transport_call transport_call;
    etch_mbm_redeliver      redeliver;
    etch_mbm_unregister     unregister;

    /* i_transportmessage of next lower layer (messagizer) */
    i_transportmessage*     transport;  /* not owned */

    /* i_sessionmessage of next higher layer (delivery service) */
    i_sessionmessage*       session;    /* not owned */

    /* - - - - - - - - - - - - - - -
     * i_sessionmessage interface
     * - - - - - - - - - - - - - - -
     */
    i_sessionmessage*    isessionmsg;  /* owned */
    etch_session_message session_message;  
    etch_session_control session_control;
    etch_session_notify  session_notify;
    etch_session_query   session_query;

    /* - - - - - - - - - - - - - - -
     * i_transportmessage interface
     * - - - - - - - - - - - - - - -
     */
    i_transportmessage*    transportmsg;  /* owned */
    etch_transport_message transport_message;
    etch_transport_control transport_control;  
    etch_transport_notify  transport_notify;   
    etch_transport_query   transport_query;   
    etch_transport_get_session  get_session;   
    etch_transport_set_session  set_session;

} etch_plainmailboxmgr;


typedef struct etch_plainmailboxmgr etch_mailboxmgr;
int64 message_id_farm;
etchmutex* g_atomiclock;
int64 etch_generate_message_id();


/**
 * new_plain_mailbox_manager()
 * etch_plainmailboxmgr constructor
 * @param itm i_transportmessage interface object, caller retains ownership.
 * @param uri url string, caller relinquishes ownership.
 * @param resxmap caller retains ownership.
 */
etch_plainmailboxmgr* new_plain_mailbox_manager(i_transportmessage*, 
    wchar_t* uri, etch_resources*, etchmutex*); 

i_mailbox* pmboxmgr_get_mailbox(etch_plainmailboxmgr*, etch_int64* msgid);

int pmboxmgr_register_mailbox(etch_plainmailboxmgr*, i_mailbox*);
int pmboxmgr_unregister_all(etch_plainmailboxmgr*); 
etch_hashtable* new_pmboxmgr_mailboxmap();  
int pmboxmgr_size(etch_plainmailboxmgr*);


#endif  /* ETCHPLAINMBOXMGR_H */
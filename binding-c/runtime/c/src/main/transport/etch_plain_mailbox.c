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
 * etch_plainmailbox.c
 * standard mailbox using a fixed size queue
 */

#include "etch_runtime.h"
#include "etch_plain_mailbox.h"
#include "etch_encoding.h"
#include "etch_thread.h"
#include "etch_exception.h"
#include "etch_objecttypes.h"
#include "etch_log.h"
#include "etch_mem.h"

static const char* LOG_CATEGORY = "etch_plain_mailbox";

// extern types
extern apr_pool_t* g_etch_main_pool;

etch_plainmailbox* new_mailbox_a(const int);
int  destroy_mailbox(void*);
void  etchmbox_alarm_wakeup (void*, const int); 
int  etchmbox_is_alarm_set(etch_plainmailbox*);
int  etchmbox_set_duration (etch_plainmailbox*, const int);
int  etchmbox_test_and_set_alarm_set(etch_plainmailbox*, const int); 

int  etchmbox_message(void*, etch_who* whofrom, etch_message* msg);
int  etchmbox_read   (void*, etch_mailbox_element** out);
int  etchmbox_read_withwait (void*, const int maxdelay, etch_mailbox_element** out);
int  etchmbox_close_delivery (void*);
int  etchmbox_close_read (void* );
int  etchmbox_register_notify (void*, etch_mailbox_notify, etch_object* state, const int maxdelay);
int  etchmbox_unregister_notify (void*, etch_mailbox_notify);
int  etchmbox_is_empty   (void*);
int  etchmbox_is_closed  (void*);
int  etchmbox_is_full    (void*);
int  etchmbox_fire_notify(void*);  
int64 etchmbox_get_message_id (void*);


/* - - - - - - - - - - - - - - -
 * constructor/destructor
 * - - - - - - - - - - - - - - -
 */

/**
 * new_mailbox()
 * etch_plainmailbox public constructor
 * @param mbmgr the manager to use to unregister this mailbox 
 * and to forward undelivered messages.
 * @param message_id
 * @param max_msgdelay the maximum delay in milliseconds to wait when attempting 
 * to put a message to the mailbox.
 * @param lifetime time in milliseconds to keep the mailbox open.
 * a lifetime of zero means keep it open until explicitly closed. 
 * @param capacity maximum number of messages. zero indicates use default capacity.
 */
etch_plainmailbox* new_mailbox (i_mailbox_manager* mbmgr, const int64 message_id, 
    const int max_msgdelay, const int lifetime, const int capacity)
{
    etch_plainmailbox* mbox = NULL;
    if (message_id == 0 || lifetime < 0 || capacity < 0) return NULL;

    if (NULL == (mbox = new_mailbox_a(capacity))) return NULL;

    mbox->manager  = mbmgr;
    mbox->lifetime = lifetime;
    mbox->rwlock   = mbmgr->rwlock; /* not owned */
    mbox->message_id = message_id;
    mbox->max_message_delay = max_msgdelay;
    mbox->max_messages = capacity > 0? capacity: MBOX_DEFMAXMESSAGES;

    if (lifetime)
        etchmbox_set_duration(mbox, lifetime);

    return mbox;
}


/**
 * new_mailbox_a()
 * etch_plainmailbox private constructor
 */
etch_plainmailbox* new_mailbox_a(const int capacity)
{
    etch_status_t status = ETCH_SUCCESS;
    etch_plainmailbox* mailbox = NULL;
    i_mailbox* imailbox = NULL;
    etch_queue* queue = NULL;
    etch_mutex_t* mutex  = NULL;
    int result = -1;

    do
    {
        // TODO: pool
        status = etch_mutex_create(&mutex, ETCH_MUTEX_UNNESTED, NULL);
        if(status != ETCH_SUCCESS) {
            // error
            break;
        }
        if (NULL == (queue  = new_queue(capacity))) break;

        /* - - - - - - - - - - - - - - -
         * i_mailbox 
         * - - - - - - - - - - - - - - -
         */
        imailbox = new_mailbox_interface( NULL, /* parent set below */
            etchmbox_message, 
            etchmbox_read,  
            etchmbox_read_withwait,  
            etchmbox_close_delivery,  
            etchmbox_close_read,   
            etchmbox_register_notify, 
            etchmbox_unregister_notify,  
            etchmbox_is_empty,  
            etchmbox_is_closed,  
            etchmbox_is_full,   
            etchmbox_get_message_id);

        /* - - - - - - - - - - - - - - -
         * etch_plainmailbox
         * - - - - - - - - - - - - - - -
         */
        mailbox = (etch_plainmailbox*) new_object(sizeof(etch_plainmailbox), ETCHTYPEB_MAILBOX, CLASSID_MAILBOX);

        ((etch_object*)mailbox)->destroy  = destroy_mailbox;
        ((etch_object*)mailbox)->clone    = clone_null;
        mailbox->readlock = mutex;
        mailbox->queue    = queue;
       
        /* copy i_mailbox virtuals to parent */
        imailbox->thisx   = mailbox;
        mailbox->imailbox = imailbox;
        mailbox->message  = etchmbox_message; 
        mailbox->read     = etchmbox_read;  
        mailbox->read_withwait     = etchmbox_read_withwait;  
        mailbox->close_delivery    = etchmbox_close_delivery;  
        mailbox->close_read        = etchmbox_close_read;   
        mailbox->register_notify   = etchmbox_register_notify; 
        mailbox->unregister_notify = etchmbox_unregister_notify;  
        mailbox->is_empty  = etchmbox_is_empty;  
        mailbox->is_closed = etchmbox_is_closed;  
        mailbox->is_full   = etchmbox_is_full;   
        mailbox->get_message_id = etchmbox_get_message_id;
        
        mailbox->mailbox_state = ETCH_MAILBOX_STATE_OPEN;
        result = 0;

    } while(0);

    if (-1 == result)
    {
        etch_mutex_destroy(mutex);
        etch_object_destroy(queue);
        if (imailbox)
            etch_free(imailbox);
    } 

    return mailbox;
}


/**
 * destroy_mailbox()
 * destructor for etch_plainmailbox (etch_mailbox)
 */
int destroy_mailbox(void* data)
{
    etch_plainmailbox* thisx = (etch_plainmailbox*)data;

    if (!is_etchobj_static_content(thisx))
    {   
        const int is_locked = 0 == etchqueue_lock(thisx->queue);
        const int current_state = thisx->mailbox_state;
        thisx->mailbox_state = ETCH_MAILBOX_STATE_SHUTDOWN;
        if (is_locked) etchqueue_unlock(thisx->queue);

        if (current_state < ETCH_MAILBOX_STATE_CLOSED_READ)
        {   /* if the mailbox has not been closed, do it now. we do this
             * in order to close the queue, cancel any active timer, and  
             * reroute any messages remaining in the mailbox. */
            etchmbox_close_read(thisx->imailbox);
            etch_sleep(100);  /* todo: remove sleep when tested out */
        }

        if (thisx->impl)
           ((etch_object*) thisx->impl)->destroy(thisx->impl);

	
	    etch_object_destroy(thisx->queue);
	    thisx->queue = NULL;


	    etch_object_destroy(thisx->notify_state);
	    thisx->notify_state = NULL;


	    etch_object_destroy(thisx->lifetimer);
	    thisx->lifetimer = NULL;

        etch_mutex_destroy(thisx->readlock);
        thisx->readlock = NULL;

        /* debug heap issue note: this is/was the spot */
        etch_free(thisx->imailbox);  
    }

   return destroy_objectex((etch_object*) thisx);
}


/* - - - - - - - - - - - - - - -
 * etch_plainmailbox public
 * - - - - - - - - - - - - - - -
 */

/**
 * etchmbox_contains_message() 
 * determines if the mailbox contains a message with the same memory address
 * as the specified message.   
 * @return TRUE or FALSE.   
 */
int etchmbox_contains_message(etch_plainmailbox* thisx, etch_message* thismsg)
{
    int result = FALSE, entrycount = 0;

    if (0 == etchqueue_lock(thisx->queue)) 
    {
        entrycount = thisx && thisx->queue? etch_apr_queue_size(thisx->queue->aprq): 0;

        if (entrycount) 
        {
            etch_iterator iterator;
            set_iterator(&iterator, thisx->queue, &thisx->queue->iterable);

            while(iterator.has_next(&iterator))
            {
                etch_mailbox_element* content = (etch_mailbox_element*) iterator.current_value;

                if (content && content->msg && content->msg == thismsg)  
                {   result = TRUE;
                    break;
                }               
                
                iterator.next(&iterator);
            }  
        }

        etchqueue_unlock(thisx->queue);
    } 

    return result;
}


/* - - - - - - - - - - - - - - -
 * alarm interface 
 * - - - - - - - - - - - - - - -
 */

/**
 * etchmbox_alarm_wakeup()
 * callback from alarm timer indicating timer expired or signaled.
 */
void etchmbox_alarm_wakeup (void* passthrudata, const int wakeupreason) 
{    
    etch_plainmailbox* thisx = (etch_plainmailbox*) passthrudata;

    ETCH_ASSERT(is_etch_mailbox(thisx));
    /* note that the timer can not be destroyed here - its thread has not exited */

    /* if we are called back in the middle of closing the mailbox we will block here 
     * until the close is complete. the close_delivery() below will then do nothing. 
     * related scenario: another thread's call to close_delivery() is blocked on this 
     * etchmbox_test_and_set_alarm_set. etchmbox_test_and_set_alarm_set releases the  
     * lock, the other close_delivery() acquires the lock and proceeds. meanwhile,  
     * close_delivery() called here (3 lines below), blocks, since the other thread
     * has the the lock. when the the other thread completes close_delivery(), the 
     * lock is released and this close_delivery() proeceeds to find that the queue 
     * is now closed, so it takes no action and returns. 
     */

    #ifdef ETCH_DEBUG
    ETCH_LOG(ETCH_PLAINMAILBOX_LOGID, ETCH_LOG_XDEBUG, "wakeup reason %s\n", reasons[wakeupreason+1]);
    #endif

    if (wakeupreason == ETCH_TIMEOUT)   
        thisx->close_delivery(thisx->imailbox);
}


/**
 * etchmbox_set_duration()       
 */
int etchmbox_set_duration(etch_plainmailbox* thisx, const int ms)
{
    thisx->is_alarm_set = TRUE; /* todo implement proper alarm manager */
    thisx->lifetimer = new_timer(ms, thisx, etchmbox_alarm_wakeup);

    if (thisx->lifetimer)
        thisx->lifetimer->start(thisx->lifetimer);

    return NULL != thisx->lifetimer;
}


/* - - - - - - - - - - - - - - -
 * i_mailbox
 * - - - - - - - - - - - - - - -
 */

/**
 * etchmbox_message() 
 * queues specified message to this mailbox.
 * @param whofrom caller retains
 * @param msg caller relinquishes on success, retains on other than success.
 * message is not destroyed here on failure case since caller may want to reroute.
 * @return 0 sucesss, -1 failure, ETCH_MAILBOX_TIMEOUT (-2) timeout,
 * or ETCH_MAILBOX_DUPLICATE (-3), message already queued.
 */
int etchmbox_message (void* data, etch_who* whofrom, etch_message* msg)
{
  i_mailbox* ibox = (i_mailbox*)data;
    int  result = 0;
    etch_mailbox_element* msgelt;
    const char* thistext = "etchmbox_message";
    etch_plainmailbox* thisx = ibox? ibox->thisx: NULL; 
    if (NULL == thisx) return -1;

    if (etchmbox_contains_message(thisx, msg)) 
        return ETCH_MAILBOX_DUPLICATE;

    msgelt = new_mailbox_element(msg, whofrom);

    /* block the mailbox from being read until the put returns and the status 
     * change notification completes. without this lock, the message could be read
     * from the mailbox, causing all client processing to complete, resulting in
     * destruction of the mailbox before the put returns, resulting in a race
     * condition when the notify references a mailbox which has now been destroyed.
     * the lock is released below prior to return from this method.
     */
    etchmbox_get_readlock (thisx, thistext);

    /* insert message to mailbox - relinquishes message on success */
    result = etchqueue_put_withwait (thisx->queue, thisx->max_message_delay, msgelt);
    
    switch(result)
    {   
        case 0: break;

        case ETCH_QUEUE_OPERATION_TIMEOUT:
             result = ETCH_MAILBOX_TIMEOUT;
             break;

        case ETCH_QUEUE_OPERATION_CANCELED: /* wait signaled */
        default:
             result = -1;
    }

    if (0 == result) 
        etchmbox_fire_notify ((etch_plainmailbox*)thisx);   
    else /* on failure case, we destroy the message element wrapper here,
          * but leave the message intact, since caller may want to reroute.
          * if we were to instead invoke the msgelt destructor, the message 
          * would also be destroyed, so we simply etch_free the wrapper. */
        etch_free(msgelt); 

    etchmbox_release_readlock (thisx, thistext);
    return result;
}


/**
 * etchmbox_check_read_result()
 * check result of mailbox read, ensure presence of out reference, 
 * and throw exception into the out object if indicated.
 * @param readresult result of the mailbox queue read operation in question.
 * @param out pointer to location currently hosting mailbox element read
 * from queue, or which will receive such an object to be instantiated here.
 * @return the passed readresult, assumed to be one of these five values:
 * 0: read result was OK and present in the *out mailbox element.
 * -1: some error occurred reading the queue, and *out is an empty mailbox
 * element hosting an exception so indicating.
 * ETCH_QUEUE_OPERATION_TIMEOUT: the queue read timed out, and *out is an empty 
 * mailbox element hosting an exception so indicating.
 * ETCH_QUEUE_OPERATION_CANCELED: the queue read was programmatically interrupted,
 * and *out is an empty mailbox element hosting an exception so indicating.
 * ETCH_QUEUE_EOF: queue was empty, and *out is now null.
 */
int etchmbox_check_read_result (const int readresult, etch_mailbox_element** out)
{
    ETCH_ASSERT(out);

    switch(readresult)
    {   case 0:  /* normal result, verify that a return object was created */
             ETCH_ASSERT(*out); 
             break;

        case ETCH_QUEUE_EOF: /* mailbox was empty, ensure no return object */
             *out = NULL;
             break;

        default:   /* error result, create return object to wrap exception */
        {
            wchar_t*         txt = NULL;
             etch_exception* ex  = NULL;
             /* determine exception text */
             switch(readresult) {
                case ETCH_QUEUE_OPERATION_TIMEOUT:
                    txt = L"mailbox read timed out";
                    break;
                case ETCH_QUEUE_OPERATION_CANCELED:
                    txt = L"mailbox read canceled";
                    break;
                default:
                    txt = L"mailbox read error";
                    break;
             }

             *out = (etch_mailbox_element*) new_etch_exception_from_errorcode(ETCH_ETIMEOUT);
             ex = (etch_exception*)*out;
             etch_exception_set_message(ex, new_stringw(txt));
             clear_etchobj_static_content(((etch_object*)ex));
             ETCH_LOGW(LOG_CATEGORY, ETCH_LOG_ERROR, L"%s\n", txt); 
         }
    }

    return readresult;
}


/**
 * etchmbox_read()
 * get an entry from mailbox, block until available.
 * @param out pointer to caller's location to receive the mailbox entry.
 * @return 0, ETCH_QUEUE_EOF, ETCH_QUEUE_OPERATION_TIMEOUT, ETCH_QUEUE_OPERATION_CANCELED, or -1.
 * if result is 0, return via the out parameter, the message read from the mailbox, wrapped in an
 * etch_mailbox_element object. if result is ETCH_QUEUE_EOF, mailbox was empty, and the out
 * parameter will be null. any other result, and a placeholder etch_mailbox_element is returned
 * via the out parameter, containing a null message, and wrapping an exception indicating the 
 * reason for mailbox read failure.
 */
int etchmbox_read (void* data, etch_mailbox_element** out)
{
  i_mailbox* ibox = (i_mailbox*)data;
    int  result = 0;
    etch_plainmailbox* thisx = ibox? ibox->thisx: NULL; 
    ETCH_ASSERT(thisx && out);
    *out = NULL;
   
    result = etchqueue_get (thisx->queue, (void**)out);

    return etchmbox_check_read_result (result, out);
}


/**
 * etchmbox_read_withwait() 
 * @param maxdelay time interval to wait for a message to arrive in the queue. 
 * this will be one of:
 * a) ETCH_NOWAIT, to specify no wait;
 * b) ETCHQUEUE_CLEARING_CLOSED_QUEUE, to ask for read from a closed queue
 *    without blocking; or
 * c) wait time in milliseconds.
 * @param out pointer to caller's location to receive the mailbox entry.
 * @return 0, ETCH_QUEUE_EOF, ETCH_QUEUE_OPERATION_TIMEOUT, ETCH_QUEUE_OPERATION_CANCELED, or -1.
 * if result is 0, return via the out parameter, the message read from the mailbox, wrapped in an
 * etch_mailbox_element object. if result is ETCH_QUEUE_EOF, mailbox was empty, and the out
 * parameter will be null. any other result, and a placeholder etch_mailbox_element is returned
 * via the out parameter, containing a null message, and wrapping an exception indicating the 
 * reason for mailbox read failure.
 */
int etchmbox_read_withwait (void* data, const int maxdelay, 
    etch_mailbox_element** out)
{
  i_mailbox* ibox = (i_mailbox*)data;
    int  result = 0;
    etch_plainmailbox* thisx = ibox? ibox->thisx: NULL; 
    ETCH_ASSERT(thisx && out);
    *out = NULL; 
 
    result = etchqueue_get_withwait (thisx->queue, maxdelay, (void**)out);

    return etchmbox_check_read_result (result, out);
}


/**
 * etchmbox_get_readlock() 
 * acquire the mailbox read mutex.
 * we wrap the lock thusly to provide logging to aid in deadlock debugging.
 * @return 0 success, -1 failure
 */
int etchmbox_get_readlock (etch_plainmailbox* thisx, const char* caller)
{
    return etchmbox_get_readlockex (thisx->rwlock, caller);
}


/**
 * etchmbox_release_readlock() 
 * release the mailbox read mutex.
 * we wrap the lock thusly to provide logging to aid in deadlock debugging.
 * @return 0 success, -1 failure
 */
int etchmbox_release_readlock (etch_plainmailbox* thisx, const char* caller)
{
    return etchmbox_release_readlockex (thisx->rwlock, caller);
}


/**
 * etchmbox_get_readlockex() 
 * acquire the specified read mutex.
 * we wrap the lock thusly to provide logging to aid in deadlock debugging.
 * @return 0 success, -1 failure
 */
int etchmbox_get_readlockex (etch_mutex* mutex, const char* caller)
{
    etch_status_t status = ETCH_SUCCESS;
    
    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_XDEBUG, "%s gets mbox lock\n", caller); 
    
    status = etch_mutex_lock(mutex);
    if(status != ETCH_SUCCESS) {
        return -1;
    }
    return 0;
}


/**
 * etchmbox_release_readlock() 
 * release the mailbox read mutex.
 * we wrap the lock thusly to provide logging to aid in deadlock debugging.
 * @return 0 success, -1 failure
 */
int etchmbox_release_readlockex (etch_mutex* mutex, const char* caller)
{
    etch_status_t status = ETCH_SUCCESS;
    
    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_XDEBUG, "%s rels mbox lock\n", caller); 

    status = etch_mutex_unlock(mutex);
    if(status != ETCH_SUCCESS) {
        return -1;
    }
    return 0;
}



/**
 * etchmbox_close_delivery() 
 * @return 0 success, -1 failure
 */
int etchmbox_close_delivery (void* data)
{
    i_mailbox* ibox = (i_mailbox*)data;
    int result = -1;
    etch_plainmailbox* thisx = NULL;
    ETCH_ASSERT(is_etch_imailbox(ibox));
    thisx = ibox->thisx;
    ETCH_ASSERT(is_etch_mailbox(thisx));

    /* when arriving here via mailbox destructor, mailbox state will be  
     * ETCH_MAILBOX_STATE_SHUTDOWN but queue may not yet be closed */
    if (thisx->mailbox_state >= ETCH_MAILBOX_STATE_CLOSED_DELIVERY && etchqueue_is_closed(thisx->queue)) {
        return ETCH_MAILBOX_RESULT_ALREADY_CLOSED;
    }

    if (0 == etchqueue_lock(thisx->queue)) 
    {        
        if (!etchqueue_is_closed(thisx->queue)) 
        {
            if (thisx->mailbox_state < ETCH_MAILBOX_STATE_CLOSED_DELIVERY)
                thisx->mailbox_state = ETCH_MAILBOX_STATE_CLOSED_DELIVERY;

            if (thisx->is_alarm_set)   
            {                 
                thisx->is_alarm_set = FALSE;

                /* this signal will usually trigger a callback to the timer    
                 * expiration handler, which will block until we release this 
                 * lock, and then do nothing, since the reason code will indicate 
                 * signaled rather than timer expiration. however if the timer 
                 * were to have expired while we are here but before we signal it,  
                 * the handler would again block, we would signal it below which 
                 * would do nothing since the timer had already fired, and when  
                 * we finally release the lock here, the handler would continue,
                 * attempting to close the mailbox, which it would now find to be
                 * already closed since we just closed it here. if on the other
                 * hand the handler got the lock before we did, then it would
                 * close the mailbox, and we would find it closed instead.
                 */
                etch_timer_stop(thisx->lifetimer);
            }

            /* todo when shutting down, ensure that the mailbox manager is not 
             * destroyed prior to the mailboxes, and ensure that the manager can
             * handle an unregister call on the mailbox and that the manager does
             * not destroy the mailbox on unregister.
             */
            if (thisx->manager)
                thisx->manager->unregister (thisx->manager, thisx->imailbox);

            etchqueue_close(thisx->queue, ETCHQUEUE_NOLOCK);
            result = 0;
        }
        
        if (0 == result) 
            etchmbox_fire_notify((etch_plainmailbox*)thisx);

        etchqueue_unlock(thisx->queue);
    }
    return result;
}


/**
 * etchmbox_close_read() 
 */
int etchmbox_close_read (void* data)
{
    i_mailbox* ibox = (i_mailbox*)data;
    etch_config_t*  config      = NULL;
    int32           propvalue   = 0;
    etch_mailbox_element* qitem = NULL;
    int         is_destroyentry = 0;
    etch_int64* msgidobj        = NULL;
    int64       msgid           = 0;
    const char* delmsg   = "destroyed mailbox message %d\n";
    const char* rrerrmsg = "could not reroute message %d from closed mailbox\n";
    etch_plainmailbox* thisx    = NULL;

    etch_runtime_get_config(&config);
    ETCH_ASSERT(config);

    if(ibox != NULL) {
        thisx = ibox->thisx;
    }
    if(thisx == NULL) {
        // error
        return -1;
    }

    if (0 > etchmbox_close_delivery(ibox)) {
        return -1;
    }

    /* state is ETCH_MAILBOX_CLOSED_DELIVERY or ETCH_MAILBOX_STATE_SHUTDOWN */
    if (thisx->mailbox_state < ETCH_MAILBOX_STATE_CLOSED_READ)
        thisx->mailbox_state = ETCH_MAILBOX_STATE_CLOSED_READ;

    while(1)   /* pop entries off queue and reroute or destroy */
    {
        if (0 != etchmbox_read_withwait(ibox, ETCHQUEUE_CLEARING_CLOSED_QUEUE, &qitem))
            break;

        msgidobj = qitem->msg? etch_message_get_id(qitem->msg): NULL;
        msgid = msgidobj? msgidobj->value: 0;
        is_destroyentry = FALSE;

        etch_config_get_property_int(config, "etch.mailbox.destroy.message", &propvalue);
        if (propvalue == 1 && thisx->mailbox_state == ETCH_MAILBOX_STATE_SHUTDOWN)
            is_destroyentry = TRUE;
        else   /* try to reroute the message */
        if (0 != thisx->manager->redeliver(thisx->manager, qitem->whofrom, qitem->msg))
        {    
            is_destroyentry = TRUE;
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, rrerrmsg, msgid); 
        }

        if (is_destroyentry)   /* if message was not rerouted destroy it */
        {
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, delmsg, msgid); 
            etch_object_destroy(qitem->msg);
            qitem->msg = NULL;
        }

        qitem->msg = NULL;     /* message has been disposed in some manner */
        etch_object_destroy(qitem); /* destroy popped element wrapper */
    }

    return 0;
}


/**
 * etchmbox_register_notify() 
 * @param pfx etch_mailbox_notify callback
 * @param state an etch object relinquishes by caller, owned by mailbox, 
 * used to pass mailbox state on notification.
 * @param maxdelay max time in milliseconds to wait for delivery, 0 forever, -1 no wait.
 * @return 0 success, -1 failure.
 */
int etchmbox_register_notify (void* data, etch_mailbox_notify pfn, 
    etch_object* state, const int maxdelay)
{
  i_mailbox* ibox = (i_mailbox*)data;
    int result = -1, isqueue_notempty_or_closed = FALSE;
    etch_plainmailbox* thisx = ibox? ibox->thisx: NULL;   
    if (!thisx || !pfn || !state) return -1;

    if (0 == etchqueue_lock(thisx->queue)) 
    {
        thisx->notify = pfn;
        thisx->notify_state = state; /* caller relinquishes */
        
        if (maxdelay > 0 && !thisx->is_alarm_set)
            etchmbox_set_duration(thisx, maxdelay);
     
        isqueue_notempty_or_closed 
            = etchqueue_size(thisx->queue) || etchqueue_is_closed(thisx->queue);

        result = etchqueue_unlock(thisx->queue);
    } 

    if (isqueue_notempty_or_closed)
        etchmbox_fire_notify((etch_plainmailbox*)thisx);

    return result;
}


/**
 * etchmbox_unregister_notify() 
 */
int etchmbox_unregister_notify (void* data, etch_mailbox_notify pfn)
{
  i_mailbox* ibox = (i_mailbox*)data;
    int result = -1;
    etch_plainmailbox* thisx = ibox? ibox->thisx: NULL;   
    if (!thisx || !pfn || pfn != thisx->notify) return -1;

    if (0 == etchqueue_lock(thisx->queue)) 
    {
        if (thisx->is_alarm_set)
        {
            thisx->is_alarm_set = FALSE;

            /* this signal will usually trigger a callback to the timer expiration   
             * handler, which will block until we release this lock, and then do
             * nothing, since the reason code will indicate signaled rather than
             * timer expiration. however if the timer were to have expired while
             * we are here but before we signal it, the handler would again block,  
             * we would signal it below which would do nothing since the timer had
             * already fired, and when we finally release the lock here, the handler
             * would continue to presumably close the mailbox. if on the other hand
             * the handler got the lock before we did, then the mailbox will now be
             * closed when we finally get the lock.
             */
            etch_timer_stop(thisx->lifetimer);
        }

        thisx->notify = NULL;
	    etch_object_destroy(thisx->notify_state);
	    thisx->notify_state = NULL;


        result = etchqueue_unlock(thisx->queue);
    }

    return result;
}


/**
 * etchmbox_fire_notify() 
 * notify registered party of mailbox status change
 */
int etchmbox_fire_notify (void* data)
{
    etch_plainmailbox* thisx = (etch_plainmailbox*)data;
    int result = -1, is_closed = 0;
    etch_mailbox_notify notify = NULL;
    etch_object* notify_state = NULL;

    if (0 == etchqueue_lock(thisx->queue)) 
    {   
        notify = thisx->notify;
        is_closed = etchqueue_is_closed(thisx->queue);
        notify_state = thisx->notify_state;

        etchqueue_unlock(thisx->queue);
    }

    if (notify)
        result = notify (thisx, thisx->imailbox, notify_state, is_closed);

    return result;
}


/**
 * etchmbox_is_empty() 
 */
int etchmbox_is_empty (void* data)
{
  i_mailbox* ibox = (i_mailbox*)data;
    int is_locked = FALSE, is_empty = TRUE;
    etch_plainmailbox* thisx = ibox? ibox->thisx: NULL; 

    if (thisx) 
    {   is_locked = 0 == etchqueue_trylock(thisx->queue);
        is_empty  = etchqueue_size(thisx->queue) == 0;
        if (is_locked) 
            etchqueue_unlock(thisx->queue);
    }

    return is_empty;
}


/**
 *  
 */
int etchmbox_is_closed (void* data)
{
  i_mailbox* ibox = (i_mailbox*)data;
    int is_locked = FALSE, is_closed = TRUE;
    etch_plainmailbox* thisx = ibox? ibox->thisx: NULL; 

    if (thisx) 
    {   is_locked = 0 == etchqueue_trylock(thisx->queue);
        is_closed = etchqueue_is_closed(thisx->queue);
        if (is_locked) 
            etchqueue_unlock(thisx->queue);
    }

    return is_closed;
}


/**
 * etchmbox_is_full() 
 */
int etchmbox_is_full (void* data)
{
  i_mailbox* ibox = (i_mailbox*)data;
    int is_locked = FALSE, is_full = TRUE;
    etch_plainmailbox* thisx = ibox? ibox->thisx: NULL; 

    if (thisx) 
    {   is_locked = 0 == etchqueue_trylock(thisx->queue);
        is_full   = etchqueue_is_full(thisx->queue);
        if (is_locked) 
            etchqueue_unlock(thisx->queue);
    }

    return is_full;
}


/**
 * etchmbox_get_message_id() 
 */
int64 etchmbox_get_message_id (void* data)
{
  i_mailbox* imailbox = (i_mailbox*)data;
     etch_plainmailbox* thisx = (etch_plainmailbox*) imailbox->thisx;
     return thisx? thisx->message_id: 0;
}


/* - - - - - - - - -
 * private
 * - - - - - - - - - 
*/

/**
 * etchmbox_is_alarm_set()
 * used to test is_alarm_set from outside a queue lock only.
 * do not use inside a queue lock, test is_alarm_set directly in that case.
 * use from inside a queue lock will deadlock since the lock is non-recursive.
 */
int etchmbox_is_alarm_set(etch_plainmailbox* thisx) 
{
    int result = FALSE;

    if (0 == etchqueue_lock(thisx->queue)) 
    {
        result = thisx->is_alarm_set;
        etchqueue_unlock(thisx->queue);
    }
    
    return result;
}


/**
 * etchmbox_test_and_set_alarm_set()
 * used to test and set is_alarm_set from outside a queue lock only.
 * do not use inside a queue lock, set is_alarm_set directly in that case.
 * use from inside a queue lock will deadlock since the lock is non-recursive
 */
int etchmbox_test_and_set_alarm_set(etch_plainmailbox* thisx, const int value) 
{
    int result = FALSE;

    if (0 == etchqueue_lock(thisx->queue)) 
    {
        result = thisx->is_alarm_set;
        thisx->is_alarm_set = value? TRUE: FALSE;
        etchqueue_unlock(thisx->queue);
    }

    return result;
}

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
 * etch_plainmailbox.h
 * standard mailbox using a fixed size queue
 */

#ifndef ETCHPLAINMBOX_H
#define ETCHPLAINMBOX_H

#include "etch_mailbox.h"
#include "etch_mailbox_manager.h"
#include "etch_simpletimer.h"
#include "etch_thread.h"
#include "etch_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MBOX_LIFETIME_UNTIL_CLOSE 0
#define MBOX_DEFMAXMESSAGES 8
#define ETCH_MAILBOX_RESULT_ALREADY_CLOSED 1

#define ETCH_MAILBOX_STATE_INITIAL         0
#define ETCH_MAILBOX_STATE_OPEN            2
#define ETCH_MAILBOX_STATE_CLOSED_DELIVERY 4
#define ETCH_MAILBOX_STATE_CLOSED_READ     6
#define ETCH_MAILBOX_STATE_SHUTDOWN        8

typedef etch_timer_callback etch_alarm_wakeup;

#if(0)

 PLAINMAILBOX(MAILBOXMANAGER) 
 |- ALARMLISTENER
 |  etch_alarm_wakeup()
  - MAILBOX
    | etch_mailbox_message() 
    | etch_mailbox_read() 
    | etch_mailbox_read_withwait()
    | etch_mailbox_close_delivery()
    | etch_mailbox_close_read()
    | etch_mailbox_register_notify()
    | etch_mailbox_unregister_notify()
    | etch_mailbox_is_empty()
    | etch_mailbox_is_closed()
    | etch_mailbox_is_full()
    | etch_mailbox_get_message_id()
     -
#endif

struct i_mailbox_manager;

/*
 * etch_plainmailbox
 * this object is instantiated, and is also a mask over any other mailbox
 * class object which may be defined. etch_plainmailbox is therefore
 * typedef'ed below as etch_mailbox.
 */
typedef struct etch_plainmailbox
{
    etch_object object;

    void* impl;
    struct i_mailbox_manager* manager;

    /* - - - - - - - - - - - - - - -
     * notify interface
     * - - - - - - - - - - - - - - -
     */
    etch_mailbox_notify notify;  

    /* - - - - - - - - - - - - - - -
     * alarm listener interface
     * - - - - - - - - - - - - - - -
     */
    etch_alarm_wakeup wakeup;

    /* - - - - - - - - - - - - - - -
     * i_mailbox interface
     * - - - - - - - - - - - - - - -
     */
    i_mailbox* imailbox;

    etch_mailbox_message           message;
    etch_mailbox_read              read;
    etch_mailbox_read_withwait     read_withwait;
    etch_mailbox_close_delivery    close_delivery;
    etch_mailbox_close_read        close_read;
    etch_mailbox_register_notify   register_notify;
    etch_mailbox_unregister_notify unregister_notify;
    etch_mailbox_is_empty          is_empty;
    etch_mailbox_is_closed         is_closed;
    etch_mailbox_is_full           is_full;
    etch_mailbox_get_message_id    get_message_id; 

    /* - - - - - - - - - - - - - - -
     * etch_mailbox instance data
     * - - - - - - - - - - - - - - -
     */
    int   lifetime;
    int   max_messages;
    int   max_message_delay;
    int64 message_id;
    etch_queue* queue;  
    etch_timer* lifetimer;
    etch_mutex*  rwlock;    /* global read write lock - not owned */
    etch_mutex*  readlock;  /* unused - lose it */
    etch_object* notify_state;

    unsigned char is_alarm_set;
    unsigned char mailbox_state;
    unsigned char unused_a;
    unsigned char unused_b;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * derivations must include all the above and define data following
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     */

} etch_plainmailbox;

etch_mailbox* new_mailbox
 (  struct i_mailbox_manager*, 
    const int64 message_id, 
    const int max_msgdelay, 
    const int lifetime, 
    const int max_messages
 );

int etchmbox_contains_message(etch_mailbox*, etch_message*);
int etchmbox_get_readlock (etch_plainmailbox*, const char*);
int etchmbox_release_readlock (etch_plainmailbox*, const char*);
int etchmbox_get_readlockex (etch_mutex*, const char*);
int etchmbox_release_readlockex (etch_mutex*, const char*);

#ifdef __cplusplus
}
#endif

#endif  /* ETCHPLAINMBOX_H */

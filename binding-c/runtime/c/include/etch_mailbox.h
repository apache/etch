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
 * etch_mailboxint.h
 * mailbox interface
 */
#ifndef ETCHIMAILBOX_H
#define ETCHIMAILBOX_H

#include "etch_object.h"
#include "etch_message.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCH_MAILBOX_TIMEOUT   (-2)
#define ETCH_MAILBOX_DUPLICATE (-3)

struct i_mailbox;
struct i_mailbox_manager;

/**
 * etch_mailbox_notify()
 * notify interface callback to receive notification of mailbox status changes.
 * @remarks accomodation is made for a single status listener only.
 * @param thisx caller
 * @param mb the mailbox whose status has changed.
 * @param state the state object specified during callback registration.
 * @param closed true if the mailbox timeout has expired and the mailbox is now closed to delivery, 
 * false if a message has arrived.
 */
typedef int (*etch_mailbox_notify) (void* thisx, struct i_mailbox* mb, void* state, const int is_closed);


/**
 * etch_mailbox_element
 * this is for now an etch object, in case it needs to own the message
 */
typedef struct etch_mailbox_element
{
    unsigned int    hashkey;
    unsigned short  obj_type;
    unsigned short  class_id;
    struct etch_object* vtab;
    etch_object_destructor destroy;
    etch_object_clone clone;
    obj_gethashkey  get_hashkey;
    struct etch_object* parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;
    etch_mutex_hookproc  synchook;
    etch_mutex_t*  synclock;

    etch_message*   msg;
    etch_who*       whofrom;

} etch_mailbox_element;


/* - - - - - - - - - - - - - - - - - 
 * signatures of i_mailbox virtuals
 * - - - - - - - - - - - - - - - - - 
 */

/** 
 * etch_mailbox_message()
 * queues the specified message to this mailbox.
 * @param thisx caller
 * @param msg the message to be received, caller relinquishes on success, retains on failure.
 * @param whofrom sender, caller retains
 * @return 0 success, -1 failure. 
 */
typedef int (*etch_mailbox_message)(void* thisx, etch_who* whofrom, etch_message* msg);

/** 
 * etch_mailbox_read()
 * reads the next message from the mailbox, waiting indefinitely for a message to be delivered.
 * @param thisx caller
 * @param out location to receive the etch_mailbox_element* result.
 * @return 0 success, -1 failure. on success, out location is populated with the requested message.
 * if mailbox is empty and closed, result will be zero and *out will contain null.
 * -1 this indicates an exception condition, not an empty condition.
 */
typedef int (*etch_mailbox_read) (void* thisx, etch_mailbox_element** out);

/** 
 * etch_mailbox_read_withwait()
 * reads the next message from the mailbox, waiting the specified time for a message to be delivered.
 * @param thisx caller
 * @param maxdelay the maximum amount of time in milliseconds to wait for a message when the
 * mailbox is empty. zero indicates wait indefinitely, -1 indicates do not wait.
 * @param out location to receive the etch_mailbox_element* result.
 * @return 0 success, -1 failure. on success, out location is populated with the requested message.
 * if mailbox is empty and closed, result will be zero and *out will contain null.
 * -1 this indicates an exception condition, not an empty condition.
 */
typedef int (*etch_mailbox_read_withwait) (void* thisx, const int maxdelay, etch_mailbox_element** out);

/** 
 * etch_mailbox_close_delivery()
 * closes the mailbox such that no more messages can be delivered to it.
 * any messages currently queued will continue to be processed.
 * @param thisx caller
 * @return 0 if mailbox was closed successfuly, -1 if mailbox was already closed or could not be closed.
 */
typedef int (*etch_mailbox_close_delivery) (void* thisx);

/** 
 * etch_mailbox_close_read()
 * closes the mailbox such that no more messages can be delivered to it or read from it.
 * any messages currently queued will be delivered to a default handler.
 * @param thisx caller
 * @return 0 if mailbox was closed successfuly, -1 if mailbox was already closed or could not be closed.
 */
typedef int (*etch_mailbox_close_read) (void* thisx);

/** 
 * etch_mailbox_register_notify()
 * register a etch_mailbox_notify callback to receive notification of mailbox status changes.
 * @param thisx caller
 * @param notify pointer to a function conforming to the etch_mailbox_notify signature.
 * @param state a value to pass through via the supplied notify callback.
 * @param maxdelay the maximum amount of time in milliseconds to wait for delivery of the notification.
 * zero indicates wait indefinitely, -1 indicates do not wait.  
 * @return 0 on success, -1 if a callback is already registered, or on exception condition.
 */
typedef int (*etch_mailbox_register_notify) (void* thisx, etch_mailbox_notify, etch_object* state, const int maxdelay);

/** 
 * etch_mailbox_unregister_notify()
 * remove the specified etch_mailbox_notify callback. cancels any current timeout.
 * @param thisx caller
 * @param notify pointer to a function conforming to the etch_mailbox_notify signature.
 * @return 0 if specified callback was unregistered or -1 is specified calback is not currently
 * registered, or on exception condition.
 */
typedef int (*etch_mailbox_unregister_notify) (void* thisx, etch_mailbox_notify);

/** 
 * etch_mailbox_is_empty()
 * @return boolean value indicating if specified mailbox is empty
 */
typedef int (*etch_mailbox_is_empty)  (void* thisx);

/** 
 * etch_mailbox_is_closed()
 * @return boolean value indicating if specified mailbox is closed
 */
typedef int (*etch_mailbox_is_closed) (void* thisx);

/** 
 * etch_mailbox_is_full()
 * @return boolean value indicating if specified mailbox is full
 */
typedef int (*etch_mailbox_is_full)   (void* thisx);


/** @return the message id of this mailbox  */
typedef int64 (*etch_mailbox_get_message_id) (void* thiz);


/** @return the concrete etch_mailbox object for this mailbox */
typedef struct etch_plainmailbox etch_mailbox;
typedef etch_mailbox* (*etch_mailbox_get_implobj) (struct i_mailbox*);


/** @return the manager for this mailbox */
typedef struct i_mailbox_manager* (*etch_mailbox_get_manager) (struct i_mailbox*);



/* - - - - - - - - - - - - - - - - - 
 * i_mailbox 
 * - - - - - - - - - - - - - - - - - 
 */

/**
 * i_mailbox
 * mailbox interface
 */
typedef struct i_mailbox
{
    unsigned int    hashkey;
    unsigned short  obj_type;
    unsigned short  class_id;
    struct etch_object* vtab;
    etch_object_destructor destroy;
    etch_object_clone clone;
    obj_gethashkey  get_hashkey;
    struct etch_object* parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;
    etch_mutex_hookproc  synchook;
    etch_mutex_t*   synclock;

    void* thisx;  /* instantiating object */
    etch_mailbox_get_implobj mailbox;
    etch_mailbox_get_manager manager;

    etch_mailbox_message message;
    etch_mailbox_read    read;
    etch_mailbox_read_withwait     read_withwait;
    etch_mailbox_close_delivery    close_delivery;
    etch_mailbox_close_read        close_read;
    etch_mailbox_register_notify   register_notify;
    etch_mailbox_unregister_notify unregister_notify;
    etch_mailbox_is_empty  is_empty;
    etch_mailbox_is_closed is_closed;
    etch_mailbox_is_full   is_full;
    etch_mailbox_get_message_id get_message_id;

} i_mailbox;



/* - - - - - - - - - - - - - - - - - 
 * public methods 
 * - - - - - - - - - - - - - - - - - 
 */

/**
 * new_mailbox_element()
 * etch_mailbox_element constructor
 * @param msg todo document ownership
 * @param whofrom todo document ownership
 */
etch_mailbox_element* new_mailbox_element(etch_message* msg, etch_who* whofrom);


/**
 * new_default_mailbox_interface()
 * i_mailbox constructor
 * populates all virtuals with stub methods.
 * @param thisx instantiator object
 */
i_mailbox* new_default_mailbox_interface(void* thisx);


/**
 * etch_mailbox_get_implobj()
 * verify and return the concrete mailbox implementation
 */
etch_mailbox* etchmbox_get_implobj(i_mailbox*);


/**
 * etch_mailbox_get_manager()
 * verify and return the mailbox's mailbox manager
 */
struct i_mailbox_manager* etchmbox_get_manager(i_mailbox*);


/**
 * new_mailbox_interface()
 * i_mailbox constructor
 * populates virtuals with specified methods.
 * @param thisx instantiator object.
 * @param balance of parameters are pointers to virtual method implementations.
 */
i_mailbox* new_mailbox_interface(void* thisx,
    etch_mailbox_message,
    etch_mailbox_read,
    etch_mailbox_read_withwait,
    etch_mailbox_close_delivery,
    etch_mailbox_close_read,
    etch_mailbox_register_notify,
    etch_mailbox_unregister_notify,
    etch_mailbox_is_empty,
    etch_mailbox_is_closed,
    etch_mailbox_is_full,
    etch_mailbox_get_message_id); 

#ifdef __cplusplus
}
#endif

#endif /* ETCHIMAILBOX_H */

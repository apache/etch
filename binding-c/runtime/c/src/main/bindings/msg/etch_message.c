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
 * etch_message.c
 * message object
 */

#include "etch_message.h"
#include "etch_log.h"
#include "etch_objecttypes.h"

static const char* LOG_CATEGORY = "etch_message";


etch_message* new_message_init(etch_type*, const int);

/**
 * new_message()  
 * constructor for etch_message
 * the value factory is a reference, the type is a copy to be passed through
 * to the underlying struct, which will then own the type memory.
 */
etch_message* new_message (etch_type* type, const int size, etch_value_factory* vf) 
{
    etch_message* newmsg = NULL;
    if (!type || !vf) return NULL;

    newmsg = new_message_init(type, size);
    if (newmsg == NULL) return NULL;

    newmsg->vf = vf; 
    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_XDEBUG, "creating message %x\n", newmsg);
    return newmsg;
}


/**
 * destroy_message()
 * destructor for etch_message
 */ 
int destroy_message (void* data) 
{
    etch_message* msg = (etch_message*)data;
    int result = 0;

    if (!is_etchobj_static_content(msg))
    {
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_XDEBUG, "destroying message %x\n", msg);
        result = destroy_structvalue(msg->sv);
    }

    destroy_objectex((etch_object*)msg);
    return result;
}

/**
 * new_message_init() 
 * common initialization on etch_message construction.
 */
etch_message* new_message_init(etch_type* type, const int initialsize)
{
    etch_message* newmsg = NULL;
    if (NULL == type) return NULL;

    newmsg = (etch_message*) new_object(sizeof(etch_message), 
        ETCHTYPEB_MESSAGE, CLASSID_ETCHMESSAGE);
    
    ((etch_object*)newmsg)->destroy = destroy_message;
    newmsg->sv = new_structvalue(type, initialsize);

    return newmsg;
}

/**
 * etch_message_remove()
 * removes an element from the message struct, returning the element.
 * if the element is found, its key is destroyed, and the object is returned.
 * caller owns returned object.
 */
etch_object* etch_message_remove(etch_message* msg, etch_field* key) {
	return structvalue_remove(msg->sv, key);
}

/**
 * etch_message_put()
 * insert key/value pair
 *
 * @param key an etch_field whose destructor will be invoked when the struct is
 * destroyed. 
 *
 * @param value a *disposable* object which is the value of the key/val pair.
 * if passed as null, specified key is removed from the message, per java binding.
 * otherwise this object's destructor will be invoked when its struct is destroyed.
 *
 * @return 0 or -1. 
 * on failure, destructors are invoked here on both key and value.
 *
 * remarks: to simplify caller logic under normal circumstances, caller
 * relinquishes ownership of parameters (other than this of course), regardless
 * of outcome. should caller wish to retain ownership on failure, the parameter
 * object could be marked such that the destructor invoked here becomes benign.
 */
int etch_message_put(etch_message* msg, etch_field* key, etch_object* value) {
	/* on success, struct owns key and value */
	if (0 == structvalue_put(msg->sv, key, value))
		return 0;

	etch_object_destroy(value);
	value = NULL;

	destroy_field(key); /* note we are often passed protected vf keys */
	return -1;
}

/**
 * etch_message_putc()
 * insert key/value pair and clear value reference
 *
 * @param key an etch_field whose destructor will be invoked when the struct is
 * destroyed. 
 *
 * @param value an indirect reference to a *disposable* object which is the value 
 * of the key/val pair.
 * caller's reference is nulled out once this object is relinquished; thus caller
 * can safely test for null and then destroy the object.
 *
 * @return 0 or -1. 
 * on failure, destructors are invoked here on both key and value.
 *
 * remarks: to simplify caller logic under normal circumstances, caller
 * relinquishes ownership of parameters (other than this of course), regardless
 * of outcome. should caller wish to retain ownership on failure, the parameter
 * object could be marked such that the destructor invoked here becomes benign.
 */
int etch_message_putc(etch_message* msg, etch_field* key, void** valref) {
	etch_object* value = NULL;

    if (valref)
    {   value = (etch_object*) *valref;
        *valref = NULL;  /* clear caller's reference as we now own value */
    }
    
	if (value){ /* on success, struct owns key and value */
        if (0 == structvalue_put (msg->sv, key, value)){
            return 0;
		}
	}

	/* a struct put was not successful so destroy parameter objects */
	etch_object_destroy(value);
	value = NULL;

	destroy_field(key); /* note we are often passed protected vf keys */
	return -1;
}

/**
 * etch_message_get()
 * fetch value for key.
 * @return a non-disposable reference to the value, not a copy, or null.
 */
etch_object* etch_message_get(etch_message* msg, etch_field* key)
{
	etch_object* retobj = NULL;

	if (msg && key && msg->sv)
		retobj = structvalue_get(msg->sv, key);

#if(0)
	if (NULL == msg || NULL == key || NULL == msg->sv)
	return throw_from(EXCPTYPE_ILLEGALARG, ETCHTYPEB_UNDEFINED, 0, 0);
	retobj = structvalue_get(msg->sv, key);
#endif

	return retobj;
}

/**
 * etch_message_type()
 * return etch type of specified message
 */
etch_type* etch_message_type(etch_message* msg)
{
	return msg && msg->sv ? msg->sv->struct_type : NULL;
}

/**
 * etch_message_aname()
 * return name of specified message in 8-bit encoding.
 * @return a reference to the message name, caller does not own it.
 */
char* etch_message_aname(etch_message* msg)
{
	etch_type* msgtype = etch_message_type(msg);
	char* msgname = msgtype && msgtype->aname ? msgtype->aname : "";
	return msgname;
}

/**
 * etch_message_reply()
 * creates a message which is a reply to the current message.
 * the current message's value factory is copied to the new message. 
 * message id of the current message (if any) is copied into the 
 * in-reply-to field of the new message.
 * @param newtype the type of the reply. caller retains ownership.
 * @return a reply message, which will contain exception if error.
 */
etch_message* etch_message_reply(etch_message* msg, etch_type* newtype)
{
	int result = -1;
	etch_int64* msgid = NULL;
	etch_message* newmsg = NULL;

	if (NULL == msg)
		return NULL;

	if (NULL == newtype) /* use message type's result type */
		newtype = etchtype_get_result_type(msg->sv->struct_type);

	if (NULL == newtype)
		return NULL;

	/* construct message. caller retains ownership of type */
	newmsg = new_message(newtype, 0, msg->vf);

	msgid = etch_message_get_id(msg); /* get back a ref to ID or null */

    if (msgid)  
        result = etch_message_set_in_reply_to (newmsg, ((etch_object*)msgid)->clone(msgid));
    
    if (0 != result)
    {      
        etch_object_destroy(newmsg);
        newmsg = NULL;
    }     

    return newmsg;
}

/**
 * etch_message_set_id()
 * sets the message-id field of this message.
 * @param id a *disposable* long object wrapping the connection specific 
 * unique identifier of this message, or NULL if the message has not yet
 * been sent. NOTE that the send process overwrites any value which might 
 * otherwise be set here.
 */
int etch_message_set_id(etch_message* msg, etch_int64* id)
{
	int result = 0;
	if (!id)
		return -1;

    /* id object ownership is relinquished here even if the call fails */
    result = ((struct i_value_factory*)((etch_object*)msg->vf)->vtab)->set_message_id(msg->vf, msg, id);
    return result;
}

/**
 * etch_message_get_id()
 * @return a non-disposable reference to the connection specific unique  
 * identifier of this message, or null if there was no such identifier. 
 */
etch_int64* etch_message_get_id(etch_message* msg) {
	etch_int64* id = NULL;
	id = ((struct i_value_factory*) ((etch_object*) msg->vf)->vtab)->get_message_id(msg->vf, msg);
	return id;
}

/**
 * message_get_in_reply_to()
 * @return a non-disposable reference to the message-id of the message that 
 * this message is a response to, or null if this is an original message 
 * or if the original message did not have a message-id. 
 * caller does not own the returned object.
 */
etch_int64* etch_message_get_in_reply_to(etch_message* msg) {
	/* vf returns to us a reference to its value */
	etch_int64
			* id =
					((struct i_value_factory*) ((etch_object*) msg->vf)->vtab)->get_in_reply_to(
							msg->vf, msg);
	return id;
}

/**
 * etch_message_set_in_reply_to()
 * sets the in-reply-to field of this message.
 * @param msgid a *disposable* long object wrapping the message-id of the 
 * message that this message is a response to. note that caller must clone 
 * or otherwise supply a disposable object as this parameter.  
 */
int etch_message_set_in_reply_to(etch_message* msg, etch_int64* msgid) {
	int result = 0;
	if (!msgid)
		return -1;

	/* msgid ownership is relinquished here even if the call fails */
	result
			= ((struct i_value_factory*) ((etch_object*) msg->vf)->vtab)->set_in_reply_to(
					msg->vf, msg, msgid);
	return result;
}

/**
 * etch_message_size()
 */

int etch_message_size(etch_message* msg) {
	return msg ? structvalue_count(msg->sv) : 0;
}


/* - - - - - - - - - - -
 * etch_unwanted_message
 * - - - - - - - - - - -
 */
  
/**
 * destroy_unwanted_message()
 * etch_unwanted_message destructor
 */
int destroy_unwanted_message(void* data)
{
    etch_unwanted_message* msg = (etch_unwanted_message*)data;
    
    if (msg->message != NULL && !is_etchobj_static_content(msg))
    {   
        msg->message->sv->items->is_readonly_keys = 0;
        msg->message->sv->items->is_readonly_values = 0;
        etch_object_destroy(msg->message);
    }

   return destroy_objectex((etch_object*) msg);
}


/**
 * new_unwanted_message()
 * etch_unwanted_message constructor
 * @param whofrom caller retains
 * @param msg caller relinquishes
 */
etch_unwanted_message* new_unwanted_message(etch_who* whofrom, etch_message* msg)
{
   etch_unwanted_message* newmsg = (etch_unwanted_message*) new_object
     (sizeof(etch_unwanted_message), ETCHTYPEB_EVENT, CLASSID_EVENT_UNWANTMSG);

    ((etch_object*)newmsg)->destroy = destroy_unwanted_message;
    ((etch_object*)newmsg)->clone   = clone_null;

    newmsg->message = msg;
    newmsg->whofrom = whofrom;
    return newmsg;
}

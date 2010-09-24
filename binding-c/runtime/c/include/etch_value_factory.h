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
 * etch_valufact.h
 * value factory is an interface defining the value factory which helps the
 * idl compiler serialize and deserialize messages, convert values, etc.
 */

#ifndef ETCH_VALUFACT_H
#define ETCH_VALUFACT_H

#include "etch_msgutl.h"
#include "etch_arraylist.h"
#include "etch_structval.h"

#ifdef __cplusplus
extern "C" {
#endif

struct i_value_factory;
struct etch_message;

/**
 * value_factory
 * value factory implementation
 */
typedef struct etch_value_factory
{
    etch_object object;

    etch_object* impl;

} etch_value_factory;
 

/**
 * i_value_factory
 * virtual function table for value factory
 */
struct i_value_factory
{
    etch_object object;

    etchparentinfo* inherits_from;

    /* - - - - - - - - - - - -
     * type
     * - - - - - - - - - - - -
     */

	/**
	 * adds a type to the set of types.
	 * @return the argument. If there is a collision with
	 * an id and name, both associated with the same type,
	 * then that type is returned instead of the argument.
	 * @throws IllegalArgumentException (returns NULL) if
	 * bad arg, or if collision in the id or name or both,
	 * when not associated with the same type.
	 */
	etch_type* (*add_type) (void* vf, etch_type*);

	/**
	 * translates a type id into the associated etch_type object.
	 */
	etch_type* (*get_type_by_id) (void* vf, const unsigned id);

	/**
	 * translates a name into the associated etch_type object.
	 */
	etch_type* (*get_type_by_name) (void* vf, const wchar_t* name);

	/**
	 * @return a collection of all the types 
	 */
	etch_arraylist* (*get_types) (void* vf);


    /* - - - - - - - - - - - -
     * encoding
     * - - - - - - - - - - - -
     */

	/**
	 * @return the encoding to use for strings.
	 */
	wchar_t* (*get_string_encoding) (void* vf);


    /* - - - - - - - - - - - -
     * message id
     * - - - - - - - - - - - -
     */

	/**
	 * @param msg the message whose well-known id field is to be returned.
	 * @return the value of the well-known message-id field. This is a
	 * unique identifier for this message on a particular transport
	 * during a particular session. If there is no well-known message-id
	 * field defined, or if the message-id field has not been set, then
	 * return null. TODO: determine WHY we can't return an int64 with value
     * zero equating to null?
	 */
	etch_int64* (*get_message_id) (void* vf, struct etch_message*);

	/**
	 * sets the value of the well-known message-id field. this is a unique
	 * identifier for this message on a particular transport during a
	 * particular session. if there is no well-known message-id field
	 * defined then no action is taken. if msgid is null, the field
	 * is cleared. 
     * regardless, the msgid object memory is owned by this method, to be 
     * assigned to the value factory; therefore if the action fails, this
     * method must destroy the object.
	 */
	int (*set_message_id) (void* vf, struct etch_message*, etch_int64* msgid);


    /* - - - - - - - - - - - -
     * reply to
     * - - - - - - - - - - - -
     */

	/**
	 * @return the value of the in-reply-to field, or null if there is
	 * none or if there is no such field defined.
	 */
	etch_int64* (*get_in_reply_to) (void* vf, struct etch_message*);

	/**
	 * if no well-known in-reply-to field defined then no action is taken.
	 * if msgid is null, the field is cleared.
	 */
	 int (*set_in_reply_to) (void* vf, struct etch_message*, etch_int64* msgid);


    /* - - - - - - - - - - - -
     * value conversion
     * - - - - - - - - - - - -
     */

	/**
	 * Converts a value to a struct value representation to be exported
	 * to a tagged data output.
	 * @param value a custom type defined by a service, or a well-known
	 * standard type (e.g., date).
	 * @return a struct value representing the value.
	 */
	etch_structvalue* (*export_custom_value) (void* vf, etch_object* value);

	/**
	 * Converts a struct value imported from a tagged data input to
	 * a normal type.
	 * @param sv a struct value representation of a custom type, or a
	 * well known standard type.
	 * @return a custom type, or a well known standard type.
	 */
	etch_object* (*import_custom_value) (void* vf, etch_structvalue* sv);

	/**
	 * @param class_id the class of a custom value.
	 * @return the struct type of a custom value class.
	 */
	etch_type* (*get_custom_struct_type) (void* vf, const unsigned etchclass);

	/**
	 * get well-known message type for exception thrown by oneway message
	 */
    etch_type* (*get_mt_exception) (void* vf);
};

typedef struct i_value_factory i_value_factory;

/**
 * constructors/destructors
 */
etch_value_factory* new_value_factory(const int objlen);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCH_VALUFACT_H */

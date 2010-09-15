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
 * etch_type.h  
 * an etch_type is an etch_id_name representing a type of a struct or message 
 */

#ifndef ETCHTYPE_H
#define ETCHTYPE_H

#include "etch_id_name.h"
#include "etch_hash.h"
#include "etch_field.h"

#ifdef __cplusplus
extern "C" {
#endif

struct etch_serializer;

#define ETCHTYPE_DEFSIZE_VTORMAP  4
#define ETCHTYPE_DEFSIZE_FIELDMAP 8

/** 
 * etch_type (see etch_id_name.h)
 */
typedef etch_id_name etch_type;

struct etch_stub;
struct i_delivery_service;
struct etch_who;
struct etch_message;

/** 
 * nonspecific stub helper callback signature.
 * the parameter types are in reality (StubBase, DeliveryService, T obj, Who, Message)
 * however we can't include those headers here.
 */
typedef int (*opaque_stubhelper) (struct etch_stub* stub, struct i_delivery_service* delsvc, void* obj, struct etch_who* sender, struct etch_message* msg);

/** 
 * etch_type_impl
 * etch_type instance data extending etch_id_name 
 */
typedef struct etch_type_impl
{
    etch_object object;

    etch_hashtable* fieldmap;          /* map owned, content not owned */
    etch_hashtable* vtormap;           /* map & keys owned, values maybe */
    /* if validators order is significant we can use etch_arraylist */
  
    etch_type*  result_type;           /* not owned */
    etch_type*  super_type;            /* not owned */
    etch_field* response_field;        /* not owned */

    opaque_stubhelper stubhelper;      /* stub helper method */
    struct etch_serializer*  impexphelper;    /* owned */

    unsigned int   timeout;            /* ms to wait for response */
    unsigned int   component_class;    /* type/class of content, not owned */
    unsigned char  is_run_validators;
    unsigned char  async_mode;

} etch_type_impl;


etch_type* new_type(const wchar_t* name);
etch_type* new_static_type(const wchar_t* name);
int destroy_static_type(etch_type*);

etch_field*   etchtype_add_field        (etch_type*, etch_field* field);
etch_field*   etchtype_get_field_by_id  (etch_type*, const unsigned id);
etch_field*   etchtype_get_field_by_name(etch_type*, const wchar_t* name);
etch_id_name* etchtype_get_key_by_id    (etch_hashtable*, const unsigned id);
etch_id_name* etchtype_get_key_by_name  (etch_hashtable*, const wchar_t*);
int etchtype_set_fields_iterator(etch_type*, etch_iterator*);
void* etchtype_get_fields  (etch_type*);
int   etchtype_fields_count(etch_type*);

int etchtype_put_validator (etch_type*, etch_field*, etch_object*); 
etch_object* etchtype_get_validator_by_id  (etch_type*, const unsigned id);
etch_object* etchtype_get_validator_by_name(etch_type*, const wchar_t* name);
int etchtype_set_validators_iterator(etch_type*, etch_iterator*);
int etchtype_clear_validator (etch_type*, etch_field*);
int etchtype_clear_validators(etch_type*);
int etchtype_validators_count(etch_type*);

unsigned int  etchtype_get_timeout(etch_type*);
unsigned int  etchtype_set_timeout(etch_type*, unsigned int ms);
etch_field*   etchtype_get_response_field(etch_type*);
etch_field*   etchtype_set_response_field(etch_type*, etch_field*);
unsigned int  etchtype_get_component_type(etch_type*);
unsigned int  etchtype_set_component_type(etch_type*, unsigned int typeclass);
etch_type*    etchtype_set_result_type(etch_type*, etch_type* rtype);
etch_type*    etchtype_get_result_type(etch_type*);
etch_type*    etchtype_set_super_type(etch_type*, etch_type* stype);
etch_type*    etchtype_get_super_type(etch_type*);
unsigned char etchtype_set_run_validators(etch_type*, unsigned char val);
unsigned char etchtype_get_run_validators(etch_type*);
unsigned char etchtype_set_async_mode (etch_type*, unsigned char);
unsigned char etchtype_get_async_mode (etch_type*);
opaque_stubhelper etchtype_set_type_stubhelper(etch_type*, opaque_stubhelper);
opaque_stubhelper etchtype_get_type_stubhelper(etch_type*);
struct etch_serializer*  etchtype_set_impexphelper(etch_type*, struct etch_serializer*);
struct etch_serializer*  etchtype_get_impexphelper(etch_type*);
int etchtype_is_assignable_from(etch_type* type, etch_type* othertype);

#define is_good_type    is_good_id_name
#define is_equal_types  is_equal_id_names
#define compute_type_id compute_id_name_id
#define compute_type_id_from_widename compute_field_id_from_widename

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHTYPE_H*/ 

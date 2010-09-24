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
 * etch_defvalufact.h
 * default implementation of value factory
 */

#ifndef ETCH_DEFVALUFACT_H
#define ETCH_DEFVALUFACT_H

#include "etch_message.h"
#include "etch_exception.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCH_DEFVF_IDNMAP_DEFINITSIZE 64
#define ETCH_DEVVF_C2TMAP_DEFINITSIZE 16
#define ETCH_DEVVF_MIXINS_DEFINITSIZE 4

typedef etch_hashtable vf_idname_map;
typedef etch_hashtable class_to_type_map;


/**
 * default_value_factory
 * value factory base implementation.
 */
typedef struct default_value_factory
{
    etch_object object;

    etch_object* impl;

    class_to_type_map* class_to_type;
    vf_idname_map*  types;
    etch_arraylist* mixins;
  
    unsigned char is_own_types;
    unsigned char is_own_class_to_type;
	
} default_value_factory;


default_value_factory* new_default_value_factory(vf_idname_map* typemap, class_to_type_map* c2tmap);
default_value_factory* new_default_value_factory_ex(const int objsize, vf_idname_map* typemap, class_to_type_map* c2tmap);

/**
 * defvf_statics
 */
typedef struct defvf_statics
{
    etch_type* _mt__etch_runtime_exception;
	etch_type* _mt__etch_auth_exception; 
	etch_type* _mt__exception;
	etch_type* _mt__etch_list;
	etch_type* _mt__etch_map;
	etch_type* _mt__etch_set;
	etch_type* _mt__etch_datetime;
	
	etch_field* _mf_msg;
	etch_field* _mf__message_id;
	etch_field* _mf__in_reply_to;
	etch_field* _mf_result;

} defvf_statics;


/* 
 * built-in (etch-global, quasi-static) objects.
 * these singleton objects are global to all vfs, instantiated with the 
 * initial vf, and destroyed at etch teardown.
 */
extern defvf_statics  builtins;
extern unsigned char  is_builtins_instantiated;

extern const wchar_t* str_etch_runtime_exception;
extern const wchar_t* str_etch_auth_exception;        
extern const wchar_t* str_exception; 
extern const wchar_t* str_etch_list; 
extern const wchar_t* str_etch_map;
extern const wchar_t* str_etch_set;
extern const wchar_t* str_etch_datetime; 

extern const wchar_t* str_msg;
extern const wchar_t* str_message_id; 
extern const wchar_t* str_in_reply_to; 
extern const wchar_t* str_result; 

extern const wchar_t* str_utf8; 
extern const wchar_t* str_keys; 
extern const wchar_t* str_values;  
extern const wchar_t* str_date_time;
extern const wchar_t* str_keys_values;

extern const wchar_t* str_msgizervf;
extern const wchar_t* str_msgizerfmt;

/* 
 * etchvf_free_builtins()
 * frees memory for etch-global quasi-static builtin objects,
 * and for the validators cache and its validator content.
 * it should be invoked at etch teardown, after last vf is destroyed.
 * unit tests will show memory leaks unless they invoke this after test.
 */
void etchvf_free_builtins();

int destroy_default_value_factory(void* data);
int defvf_initialize_static(vf_idname_map* typemap, class_to_type_map*  c2tmap);
etch_type* etchtypemap_get_by_name(etch_hashtable*, const wchar_t* name);
int defvf_add_mixin(default_value_factory*, etch_value_factory* mixedin_vf);
int get_etch_string_encoding(etch_value_factory*);
int etchtypelist_comparator(void* a, void* b);
unsigned message_get_id32 (etch_message*);
class_to_type_map* new_class_to_type_map(const int initialsize);
etch_set* new_vf_types_collection(const int initialsize);
vf_idname_map* new_vf_idname_map(const int initialsize);
int class_to_type_map_put(class_to_type_map*, const unsigned, etch_type*); 
etch_type* class_to_type_map_get(class_to_type_map*, const unsigned);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCH_DEFVALUFACT_H */

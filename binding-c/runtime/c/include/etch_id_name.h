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
 * etch_id_name.h 
 * etch_id_name is base class for etch_field and etch_type. it binds
 * together a type or field name with its associated id. The id is used
 * for certain operations, such as the key in a map, comparisons, and
 * binary encoding on the wire.  
*/

#ifndef ETCHIDNAME_H
#define ETCHIDNAME_H

#include "etch.h"
#include "etch_object.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * etch_id_name
 */
struct etch_id_name
{
    etch_object object;

    etch_object*    impl; /* extension of id_name, if any */      

	unsigned id;          /* wire ID, not a hashmap key */
    size_t   namebytelen; /* byte length including terminator */
    wchar_t* name;        /* owned */
    char*    aname;       /* owned - 8-bit name 1st step in id_name conversion */
};

typedef struct etch_id_name etch_id_name;  

etch_id_name* new_id_name(const wchar_t* name);
void* clone_id_name(void*);
int destroy_id_name(void*);

int is_good_id_name(etch_id_name* thisp);
int is_equal_id_names(etch_id_name*, etch_id_name*);
int compute_id_name_id(const char*);
int compute_id_name_id_from_widename(const wchar_t*);
uint32 id_name_get_hashkey(void*);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHIDNAME_H */ 

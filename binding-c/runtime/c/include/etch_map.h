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
 * etchmap.h -- generic string to object map
 */

#ifndef ETCHMAP_H
#define ETCHMAP_H

#include "etch.h"
#include "etch_hash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCHMAP_MAX_IKEYSIZE 31

void* etchmap_find         (etch_hashtable*, const unsigned int key, void** out);
void* etchmap_findx        (etch_hashtable*, char* key, void** out);
void* etchmap_findxw       (etch_hashtable*, wchar_t* key, void** out);
void* etchmap_findxl       (etch_hashtable*, char* key, unsigned keylen, void** out);
void* etchmap_find_by_hash (etch_hashtable*, const unsigned hash, void** out);

void* etchmap_del          (etch_hashtable*, const unsigned int key);
void* etchmap_delx         (etch_hashtable*, char* key);
void* etchmap_delxw        (etch_hashtable*, wchar_t* key);
void* etchmap_delxl        (etch_hashtable*, char* ckey, const unsigned keylen);

int etchmap_add            (etch_hashtable*, const unsigned int key, void* data);
int etchmap_addx           (etch_hashtable*, char* key, void* data);
int etchmap_addxw          (etch_hashtable*, wchar_t* key, void* data);
int etchmap_insert         (etch_hashtable*, const unsigned, void*, const int is_check);
int etchmap_insertx        (etch_hashtable*, char* key, void* data, const int is_check);
int etchmap_insertxw       (etch_hashtable*, wchar_t* key, void* data, const int is_check);
int etchmap_insertxl       (etch_hashtable*, char*, const unsigned, void*, const int); 
int etchmap_insertxlw      (etch_hashtable*, wchar_t*, const unsigned, void*, const int); 

int etchmap_count(etch_hashtable*);

etch_hashitem* etchmap_current(etch_hashtable*);

int etchmap_map_add (etch_hashtable* map, etch_object* key, etch_object* value); 
int etchmap_map_find(etch_hashtable* map, etch_object* key, etch_hashitem** out);
int etchmap_set_add (etch_hashtable* set, etch_object* key); 
int etchmap_is_object_key(etch_hashtable*);

int string_to_etchobject_clear_handler    (char* key, etch_object* value);  
int string_to_genericobject_clear_handler (char* key, void* value);  

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHMAP_H*/ 

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
 * etchrun.h -- runtime data management 
 */

#ifndef ETCHRUN_H
#define ETCHRUN_H

#include "etch.h"
#include "etchhash.h"


/*
 * objrec is a node in a runtime cache of instantiated objects, specifically
 * object vtables. The idea is, when we create an object, such as an etch_
 * hashtable, for example, we check this cache to see if that object has
 * been implemented previously; if not, we allocate the abstract part of
 * the object (the vtable), implement it with pointers to concrete methods,  
 * allocate a bucket for it, and add it to this cache. Thus each etch object
 * carries around its own instance data of course, but points to its vtable.
 */
struct objrec /* DEPRECATED */
{
   unsigned obj_type;    /* four 8-bit fields -- see etchobjtypes.h */
   void*    obj_impl;    /* "value" of the object */
   struct objrec* next;  /* link pointer */
}; 
typedef struct objrec objrec; /* DEPRECATED */


/*
 * runtable is the runtime list of instantiated objects, specifically
 * object vtables. See comments above. 
 */
objrec* runtable, *runtable_tail;  /* DEPRECATED */ 


/*
 * runtime cache
 */
etch_hashtable* global_cache;


/*
 * method signatures
 */
objrec* objrec_find(const unsigned int typ); /* DEPRECATED */
void*   objrec_get (const unsigned int typ); /* DEPRECATED */
void*   objrec_del (const unsigned int typ); /* DEPRECATED */
objrec* objrec_add (const unsigned int typ, void* data);  /* DEPRECATED */
int     objtable_clear(); /* DEPRECATED */
int     objtable_count(); /* DEPRECATED */

etch_hashtable* cache_create();
void* cache_find   (const unsigned int typ, void** out);
void* cache_del    (const unsigned int typ); 
int   cache_add    (const unsigned int typ, void* data);  
int   cache_insert (const unsigned int objtype, void* data, const int is_check);
void* cache_findx  (char* key, void** out);
void* cache_delx   (char* key);
int   cache_addx   (char* key, void* data);
int   cache_insertx(char* key, void* data, const int is_check);
void* cache_find_by_hash(const unsigned hash, void** out);
etch_hashitem* cache_current();
int   cache_destroy();
int   cache_clear();   
int   cache_count();  
int   cache_dump();  


#endif /* #ifndef ETCHRUN_H*/ 
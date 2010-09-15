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
 * etch_arrarylist.h 
 * arraylist implementation.
 */

#ifndef ETCHARRAYLIST_H
#define ETCHARRAYLIST_H

#include "etch.h"
#include "etch_mutex.h"
#include "etch_collection.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCHARRAYLIST_DEFSIZE 128
#define ETCHARRAYLIST_CONTENT_SIMPLE 0  /* content memory freed as a unit */
#define ETCHARRAYLIST_CONTENT_OBJECT 1  /* content is etchobject */
#define ETCHARRAYLIST_SYNCHRONIZED TRUE

//TODO: Check´, why this is needed
typedef int (*arraycallback) (int, void*); /* arraylist callback signature */


/** 
 *  etch_arraylist: data structure encapsulating the arraylist
 *  if is_readonly is set, content is read only and therefore is_free_content  
 *  will have no effect -- list items may still be inserted and removed. This
 *  guards against accidentally freeing memory during arraylist destruction 
 *  for arrays of references which are still intended to remain in use. 
 */
typedef struct etch_arraylist
{
    etch_object object;

    void** base;
    unsigned short content_obj_type;  /* etch obj_type of a native value */
    unsigned short content_class_id;  /* etch class_id of a native value */
    unsigned int count;

    /* this object may be masked by etch_collection_mask to determine content
     * type and class, so do not add any fields above this comment */

    unsigned int delta;
    unsigned int size;
    i_iterable iterable;

    byte is_readonly;  
    byte content_type;

    arraycallback freehook; /* hook for free list content */

} etch_arraylist;


etch_arraylist* new_etch_arraylist(const unsigned int initialsize, const unsigned int deltasize);
etch_arraylist* new_etch_arraylist_from(etch_arraylist*);
etch_arraylist* new_etch_arraylist_synchronized(const unsigned int initialsize, const unsigned int deltasize);
etch_arraylist* new_etch_arraylist_synchronized_from(etch_arraylist*);

int   etch_arraylist_add      (etch_arraylist*, void* content);
int   etch_arraylist_add_from (etch_arraylist*, etch_arraylist* newitems, etch_comparator); 
int   etch_arraylist_containsp(etch_arraylist*, void* content, const unsigned startat); /* pointer compare */
int   etch_arraylist_contains (etch_arraylist*, void* content, const unsigned startat, etch_comparator);
int   etch_arraylist_count(etch_arraylist* list);
int   etch_arraylist_indexofp (etch_arraylist*, void* content, const unsigned startat); /* pointer compare */
int   etch_arraylist_indexof  (etch_arraylist*, void* content, const unsigned startat, etch_comparator);
int   etch_arraylist_remove_content(etch_arraylist*, void* content, const unsigned startat, etch_comparator);
int   etch_arraylist_insert   (etch_arraylist*, const unsigned i, void* content);
int   etch_arraylist_remove   (etch_arraylist*, const unsigned i, const int is_free_content);
int   etch_arraylist_set      (etch_arraylist*, const unsigned i, void* content);
void* etch_arraylist_get      (etch_arraylist*, const unsigned i);
void  etch_arraylist_clear    (etch_arraylist*, const int is_free_content);
void  etch_arraylist_destroy  (etch_arraylist*, const int is_free_content);
void  etch_arraylist_copyattrs(etch_arraylist* to, etch_arraylist* from);

/* 
 * explicit synchronization locking methods.
 * these should be used only for locking a list during list iteration.   
 * for synchronization of list operations, the presence of list.synchook  
 * and list.synclock is sufficient. 
 */
int   etch_arraylist_getlock  (etch_arraylist*);
int   etch_arraylist_trylock  (etch_arraylist*);
int   etch_arraylist_rellock  (etch_arraylist*);

/* i_iterable function overrides */
int   etch_arraylist_iterable_first   (etch_iterator*);
int   etch_arraylist_iterable_next    (etch_iterator*);
int   etch_arraylist_iterable_has_next(etch_iterator*);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHARRAYLIST_H */

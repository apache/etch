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
#include "etch_collection.h"

#define ETCHARRAYLIST_DEFSIZE 128
#define ETCHARRAYLIST_CONTENT_SIMPLE 0  /* content memory freed as a unit */
#define ETCHARRAYLIST_CONTENT_OBJECT 1  /* content is etchobject */
#define ETCHARRAYLIST_SYNCHRONIZED TRUE

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
    unsigned int     hashkey;
    unsigned short   obj_type; 
    unsigned short   class_id;
    struct vtabmask* vtab;       
    int  (*destroy) (void*);
    void*(*clone)   (void*); 
    obj_gethashkey   get_hashkey;
    struct objmask*  parent;
    etchresult*      result;
    unsigned int     refcount;
    unsigned int     length;
    unsigned char    is_null;
    unsigned char    is_copy;
    unsigned char    is_static;
    unsigned char    reserved;

    void**   base;
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
    arraycallback synchook; /* hook for list synchronization */
    void* synclock;         /* synchronization mutex */

} etch_arraylist;


etch_arraylist* new_arraylist(const unsigned initialsize, const unsigned deltasize);
etch_arraylist* new_arraylist_from(etch_arraylist*);
int   arraylist_add      (etch_arraylist*, void* content);
int   arraylist_add_from (etch_arraylist*, etch_arraylist* newitems, etch_comparator); 
int   arraylist_containsp(etch_arraylist*, void* content, const unsigned startat); /* pointer compare */
int   arraylist_contains (etch_arraylist*, void* content, const unsigned startat, etch_comparator);
int   arraylist_indexofp (etch_arraylist*, void* content, const unsigned startat); /* pointer compare */
int   arraylist_indexof  (etch_arraylist*, void* content, const unsigned startat, etch_comparator);
int   arraylist_remove_content(etch_arraylist*, void* content, const unsigned startat, etch_comparator);
int   arraylist_insert   (etch_arraylist*, const unsigned i, void* content);
int   arraylist_remove   (etch_arraylist*, const unsigned i, const int is_free_content);
int   arraylist_set      (etch_arraylist*, const unsigned i, void* content);
void* arraylist_get      (etch_arraylist*, const unsigned i);
void  arraylist_clear    (etch_arraylist*, const int is_free_content);
void  arraylist_destroy  (etch_arraylist*, const int is_free_content);
void  arraylist_copyattrs(etch_arraylist* to, etch_arraylist* from);

/* 
 * explicit synchronization locking methods.
 * these should be used only for locking a list during list iteration.   
 * for synchronization of list operations, the presence of list.synchook  
 * and list.synclock is sufficient. 
 */
int   arraylist_getlock  (etch_arraylist*);
int   arraylist_trylock  (etch_arraylist*);
int   arraylist_rellock  (etch_arraylist*);

/* i_iterable function overrides */
int   arraylist_iterable_first   (etch_iterator*);
int   arraylist_iterable_next    (etch_iterator*);
int   arraylist_iterable_has_next(etch_iterator*);


#endif /* #ifndef ETCHARRAYLIST_H */
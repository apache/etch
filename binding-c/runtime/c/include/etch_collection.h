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
 * etch_collection.h 
 * definitions common to collections
 */

#ifndef ETCHCOLLECTION_H
#define ETCHCOLLECTION_H

#include "etch.h"
#include "etch_object.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCH_SYNC_SET 1
#define ETCH_SYNC_TRY 2
#define ETCH_SYNC_REL 3
#define ETCH_SYNC_DEL 4

#define ETCH_SYNC_SET_ ((void*) ETCH_SYNC_SET)
#define ETCH_SYNC_TRY_ ((void*) ETCH_SYNC_TRY)
#define ETCH_SYNC_REL_ ((void*) ETCH_SYNC_REL)
#define ETCH_SYNC_DEL_ ((void*) ETCH_SYNC_DEL)

struct i_iterable;
struct etch_iterator;

/** 
 *  method signatures for i_iterable interface.
 *  each accepts an iterator object as first parameter. the iterator constructor
 *  should call first(), thus establishing a current position. next() should operate 
 *  as first() if there is no current position.
 */
typedef int (*iterable_first)    (struct etch_iterator*); 
typedef int (*iterable_next)     (struct etch_iterator*); 
typedef int (*iterable_has_next) (struct etch_iterator*); 

/** 
 *  etch_iterator: iterator object for any i_iterable.
 */
typedef struct etch_iterator
{
    etch_object object;

    iterable_first    first;
    iterable_next     next;
    iterable_has_next has_next;

    int   ordinal;       /* 1-based position index */
    void* collection;    /* underlying collection */
    void* current_value; /* data at current posn */
    void* current_key;   /* key at current posn */
    unsigned char   is_own_collection;
} etch_iterator;



/** 
 *  i_iterable: vtable for iterable interface
 */
struct i_iterable
{
    etch_object object;

    etchparentinfo* inherits_from; 

    iterable_first    first;    /* set current position at first entry  */
    iterable_next     next;     /* set current position at next entry  */ 
    iterable_has_next has_next; /* return TRUE if next will succeed */
};

typedef struct i_iterable i_iterable;


/** 
 *  new_iterable
 *  constructor for i_iterable interface
 */
i_iterable* new_iterable(i_iterable* thisp, i_iterable* parent, 
    iterable_first, iterable_next, iterable_has_next);

/** 
 *  new_iterator(), set_iterator(), destroy_iterator
 *  constructors and destructors for etch_iterator
 */
etch_iterator* new_iterator(void* collection, i_iterable* iterable);

int set_iterator(etch_iterator* thisp, void* collection, i_iterable* iterable);

/* should be static! call via vtable */
int destroy_iterator(void*);

etch_iterator* new_empty_iterator();


int etch_comparator_noteq(void* a, void* b);
int etch_comparator_equal(void* a, void* b);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHCOLLECTION_H */

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

/**
 * etch_collection.c -- collection support
 */

#include "etch_collection.h" 
#include "etch_object.h"
#include "etch_objecttypes.h"
#include "etch_mem.h"

int default_iterable_first   (etch_iterator*);
int default_iterable_next    (etch_iterator*);
int default_iterable_has_next(etch_iterator*);


/**
 * new_iterable()
 * constructor for an i_iterable interface
 */
i_iterable* new_iterable(i_iterable* thisp, i_iterable* parent, iterable_first func_first, 
    iterable_next func_next, iterable_has_next func_hasnext)
{
    i_iterable* vtab = NULL;

    if  (thisp) /* initializing existing object? */
         vtab = thisp;
    else vtab = new_vtable(parent, sizeof(i_iterable), CLASSID_ITERABLE_VTAB);

    if (NULL == vtab) return NULL;

    vtab->first    = func_first?   func_first:   default_iterable_first;
    vtab->next     = func_next?    func_next:    default_iterable_next;
    vtab->has_next = func_hasnext? func_hasnext: default_iterable_has_next;

    return vtab;
}


/**
 * destroy_iterator()
 * iterator destructor
 */
int destroy_iterator(void* data)
{   
    etch_iterator* iterator = data;

    /* iterator owns itself, and possibly the collection. 
     * its vtable is owned by the collection. */
    if (!is_etchobj_static_shell(iterator))
    {
        if (iterator->is_own_collection)
           ((etch_object*)iterator->collection)->destroy(iterator->collection);
            
        etch_free(iterator); 
    }

    return 0;
}


/**
 * clone_iterator()
 * iterator copy constructor
 */
void* clone_iterator(void* data)
{   
  etch_iterator* iterator = data;
    etch_iterator* newobj = etch_malloc(sizeof(struct etch_iterator), ETCHTYPEB_ITERATOR); 
    memcpy(newobj, iterator, sizeof(struct etch_iterator));
    return newobj;
}


/**
 * new_iterator()
 * constructor for an etch_iterator object.
 */
etch_iterator* new_iterator(void* collection, i_iterable* iterable)
{
    etch_iterator* iterator = etch_malloc(sizeof(etch_iterator), ETCHTYPEB_ITERATOR);

    set_iterator(iterator, collection, iterable);

    return iterator;
}


/**
 * set_iterator()
 * constructor for an existing etch_iterator object.
 */
int set_iterator(etch_iterator* iterator, void* collection, i_iterable* iterable)
{
    memset(iterator, 0, sizeof(etch_iterator));

    ((etch_object*)iterator)->obj_type = ETCHTYPEB_ITERATOR;
    ((etch_object*)iterator)->class_id = CLASSID_ITERATOR; 

    ((etch_object*)iterator)->vtab = (vtabmask*)iterable;
    iterator->collection = collection;

    ((etch_object*)iterator)->destroy  = destroy_iterator;
    ((etch_object*)iterator)->clone    = clone_iterator;

    iterator->first    = iterable->first;
    iterator->next     = iterable->next;
    iterator->has_next = iterable->has_next;

    iterator->first(iterator); /* establish initial position */
    return 0;
}


/**
 * new_empty_iterator()
 * constructor for an empty etch_iterator object, i.e. has_next() is false
 */
etch_iterator* new_empty_iterator()
{
    etch_iterator* iterator = etch_malloc(sizeof(etch_iterator), ETCHTYPEB_ITERATOR);

    ((etch_object*)iterator)->obj_type = ETCHTYPEB_ITERATOR;
    ((etch_object*)iterator)->class_id = CLASSID_ITERATOR; 
    ((etch_object*)iterator)->destroy  = destroy_iterator;

    iterator->first    = default_iterable_first;
    iterator->next     = default_iterable_next;
    iterator->has_next = default_iterable_has_next;

    return iterator;
}


/**
 * default_iterable_first()
 * i_iterable first() default virtual method 
 */
int default_iterable_first(etch_iterator* i) 
{
    return -1;
}


/**
 * default_iterable_next()
 * i_iterable next() default virtual method 
 */
int default_iterable_next(etch_iterator* i) 
{
    return -1;
}


/**
 * default_iterable_has_next()
 * i_iterable has_next() default virtual method 
 */
int default_iterable_has_next (etch_iterator* i)
{
    return FALSE;
}


/**
 * etch_comparator_noteq(), etch_comparator_equal
 * comparators to use when a result not dependent on parameters is desired,
 * such as when using an add_from method to add to a collection when the
 * target collection is empty and thus no contains() comparison is needed. 
 */
int etch_comparator_noteq(void* a, void* b) { return -1;}
int etch_comparator_equal(void* a, void* b) { return 0; }

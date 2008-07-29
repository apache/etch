/**
 * etch_collection.c -- collection support
 */

#include "etch_collection.h" 
#include "etch_global.h"
#include "etchobj.h"

int default_iterable_first   (etch_iterator*);
int default_iterable_next    (etch_iterator*);
int default_iterable_has_next(etch_iterator*);


/**
 * new_iterable()
 * constructor for an i_iterable interface
 * simple object, destroy with etch_free.
 */
i_iterable* new_iterable(i_iterable* thisp, i_iterable* parent, iterable_first func_first, 
    iterable_next func_next, iterable_has_next func_hasnext)
{
    i_iterable* vtab = NULL;

    if  (thisp) /* initializing existing object? */
         vtab = thisp;
    else vtab = new_vtable(parent, sizeof(i_iterable), ETCHTYPEB_VTABLE, 0);

    if (NULL == vtab) return NULL;

    vtab->first    = func_first?   func_first:   default_iterable_first;
    vtab->next     = func_next?    func_next:    default_iterable_next;
    vtab->has_next = func_hasnext? func_hasnext: default_iterable_has_next;

    return vtab;
}


/**
 * new_iterator()
 * constructor for an etch_iterator object.
 * simple object, destroy with etch_free.
 */
etch_iterator* new_iterator(void* collection, i_iterable* iterable)
{
    etch_iterator* iterator = etch_malloc(sizeof(etch_iterator), ETCHTYPEB_ITERATOR);
    memset(iterator, 0, sizeof(etch_iterator));

    iterator->obj_type = short_type(ETCHTYPEA_IS_CLASS, ETCHTYPEB_ITERATOR);
    iterator->class_id = ETCHTYPEB_ITERATOR; 

    iterator->vtab = iterable;
    iterator->collection = collection;

    iterator->vtab->first(iterator); /* establish initial position */

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
    return -1;
}


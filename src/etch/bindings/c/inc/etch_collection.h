/*
 * etch_collection.h 
 * definitions common to collections
 */

#ifndef ETCHCOLLECTION_H
#define ETCHCOLLECTION_H

#include "etch.h"

struct i_iterable;

/** 
 *  etch_iterator: iterator object for any i_iterable.
 */
typedef struct etch_iterator
{
    unsigned short obj_type;     
    unsigned short class_id;    
    struct i_iterable* vtab;  

    int   ordinal;          /* 1-based position index */
    void* collection;       /* underlying collection */
    void* current_value;    /* data at current posn */
    void* current_key;      /* key at current posn */

} etch_iterator;


/** 
 *  function signatures for i_iterable interface.
 *  each accepts an iterator object as first parameter. the iterator constructor
 *  should call first(), thus establishing a current position. next() should operate 
 *  as first() if there is no current position.
 */
typedef int (*iterable_first) (etch_iterator*); 
typedef int (*iterable_next)  (etch_iterator*); 
typedef int (*iterable_has_next) (etch_iterator*); 


/** 
 *  i_iterable: virtual function table for iterable interface
 */
struct i_iterable
{
    unsigned short obj_type;     
    unsigned short class_id;    
    struct i_iterable* vtab;    /* parent vtable if any */

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
 *  new_iterator
 *  constructor for etch_iterator
 */
etch_iterator* new_iterator(void* collection, i_iterable* iterable);


#endif /* #ifndef ETCHCOLLECTION_H */
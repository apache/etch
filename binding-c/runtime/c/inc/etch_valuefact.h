/* 
 * etch_valuefact.h 
 */

#ifndef ETCHVALFACT_H
#define ETCHVALFACT_H

#include "etch_structval.h"


/* 
 * value factory interface 
 */
struct i_value_factory
{
    unsigned short obj_type; 
    unsigned short class_id;  
    struct i_value_factory* vtab;

};

typedef struct i_value_factory i_value_factory;


/* 
 * value factory implementation 
 */
struct etch_value_factory
{
    unsigned short obj_type; 
    unsigned short class_id; 
    i_value_factory* vtab;
    void* impl;     
};

typedef struct etch_value_factory etch_value_factory;


/* 
 * constructors 
 */
etch_value_factory* new_etch_value_factory();
ETCH_VALUEFACTORY*  new_boxed_value_factory();



#endif /* #ifndef ETCHVALFACT_H*/ 
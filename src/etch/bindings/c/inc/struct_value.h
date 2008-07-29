/* 
 * struct_value.h -- defines the etch_structvalue object.
 * An etch_structvalue is a typed map of key/value pairs, where type is an etch_type,
 * a key is an etch_field, and a value is chosen from the basic types boolean, byte, 
 * short, int, long, float, double, String, an array of those, the extended types
 * ArrayValue and StructValue, and specific types supported by ValueFactory.
 */

#ifndef ETCHSTRUCTVAL_H
#define ETCHSTRUCTVAL_H

#include "etch.h"
#include "etchhash.h"
#include "type.h"

/** 
 * etch_structvalue
 */
typedef struct etch_structvalue 
{
	/**
     * type of the struct (or the action or event if this is a message).
	 */
    // etch_type type;  /* private */

	/**
	 * hash table from which the struct_value is "derived"
	 */
    etch_hashtable* items;

} etch_structvalue;


etch_structvalue* new_structvalue(etch_type* type, const int initialsize);  /* public  */
etch_structvalue* ctor_etch_structvalue(etch_hashtable*);  /* private */
void destroy_etch_structvalue(etch_structvalue*);          /* public  */


#endif /* #ifndef ETCHSTRUCTVAL_H */ 
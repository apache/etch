/*
 * etchmsgutl.h -- includes common to the etch message components
 */

#ifndef ETCHMSGUTL_H
#define ETCHMSGUTL_H

#include "etchobj.h"
#include "type.h"
#include "field.h"

#define ETCH_ARRAYVALUE_DEFAULT_INITSIZE 32
#define ETCH_ARRAYVALUE_DEFAULT_DELTSIZE 0
#define ETCH_ARRAYVALUE_DEFAULT_READONLY TRUE


/**
 * equate java array element to an object wrapper.
 * we do so to facilitate porting of the java code.
 */
typedef etchobject ETCH_ARRAY_ELEMENT;


/**
 * etch_struct_element
 */
typedef struct etch_struct_element
{
  etch_field  key;
  etchobject* value;

} etch_struct_element;



etch_struct_element* new_struct_element(etch_field* field, etchobject* value);

typedef etchobject ETCH_STRUCT_ELEMENT;

ETCH_STRUCT_ELEMENT* new_boxed_struct_element(etch_field* field, etchobject* value);



#endif /* #ifndef ETCHMSGUTL_H */
/**
  etch_objects.h 
  etch object wrappers definintion and construction
*/

#ifndef ETCHOBJECTS_H
#define ETCHOBJECTS_H

#include "etchobj.h"

/**
 * equate boxed object types to the object wrapper.
 */
typedef etchobject ETCH_INT;
typedef etchobject ETCH_INT8;
typedef etchobject ETCH_INT16;
typedef etchobject ETCH_INT32;
typedef etchobject ETCH_INT64;
typedef etchobject ETCH_BYTE;
typedef etchobject ETCH_BOOL;
typedef etchobject ETCH_IEEE32;
typedef etchobject ETCH_IEEE64;
typedef etchobject ETCH_STRING;
typedef etchobject ETCH_HASHTABLE;

typedef etchobject ETCH_TYPE;
typedef etchobject ETCH_FIELD;
typedef etchobject ETCH_ID_NAME;
typedef etchobject ETCH_EXCEPTION;
typedef etchobject ETCH_MESSAGE;
typedef etchobject ETCH_STRUCTVALUE;
typedef etchobject ETCH_ARRAYVALUE;
typedef etchobject ETCH_VALUEFACTORY;
typedef etchobject ETCH_TAGDATAINPUT;
typedef etchobject ETCH_TAGDATAOUTPUT;
typedef etchobject ETCH_ARRAYLIST;

/**
 * constructors for boxed primitives 
 */
ETCH_INT32*  new_etch_int32 (const int value);
ETCH_INT8*   new_etch_int8  (const signed char value);
ETCH_INT16*  new_etch_int16 (const short value);
ETCH_INT64*  new_etch_int64 (const int64 value);
ETCH_BYTE*   new_etch_byte  (const unsigned char value);
ETCH_BOOL*   new_etch_bool  (const unsigned char value);
ETCH_IEEE32* new_etch_ieee32(const float value);
ETCH_IEEE64* new_etch_ieee64(const double value);
ETCH_STRING* new_etch_string(const wchar_t* value);
ETCH_ARRAYLIST* new_etch_arraylist
 (const unsigned isize, const unsigned dsize, const unsigned is_readonly);

void destroy_boxed_arraylist(ETCH_ARRAYLIST* thisp);



#endif /* #ifndef ETCHOBJECTS_H */
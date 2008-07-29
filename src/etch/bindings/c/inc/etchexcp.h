/*
 * etchexcp.h -- etch exception
 */

#ifndef ETCHEXCP_H
#define ETCHEXCP_H

#include "etch_objects.h"

/** 
 * etchexception.flags bits
 */
#define ETCHEXCP_STATICTEXT 0
#define ETCHEXCP_COPYTEXT   1
#define ETCHEXCP_FREETEXT   2
#define ETCHEXCP_CHECKED    4


/** 
 * exception type identifiers
 */
typedef enum excptype
{ 
  EXCPTYPE_NONE = 0x0,  
  EXCPTYPE_SOME_UNCHECKED_TBD = 0x1,
  EXCPTYPE_IO      = 0x2,
  EXCPTYPE_NULLPTR = 0x3,
  EXCPTYPE_CHECKED_START = 0x40,
  EXCPTYPE_CHECKED_BOGUS = 0x41,
} excptype; 


/**
 * etchexception native object 
 */
struct etchexception
{   
    excptype excptype; 
    wchar_t* excptext;
    size_t   textlen;  /* byte length including term */
    unsigned flags;    /* ETCHEXCP_XXXX */
};

typedef struct etchexception etchexception;

/**
 * etch_throw()
 * "throw" specified exception, meaning instantiate exception and attach to object
 */
int etch_throw  (etchobject* obj, const excptype t, wchar_t* opttext, unsigned flags);

/**
 * etch_throwex()
 * as "throw" but also including result code and reason code
 */
int etch_throwex(etchobject* obj, const excptype t, wchar_t* opttext,  
    unsigned flags, const int result, const int reason); 

/**
 * native ctors and dtor
 */
void destroy_exception(etchexception* excp);
etchexception* new_exception(const excptype t, wchar_t* x, const unsigned flags);

/**
 * default_excptext()
 * gets default text for specified exception type.
 */
wchar_t* default_excptext(const excptype t);

/**
 * boxed ctors and dtor
 */
ETCH_EXCEPTION* new_etch_exception(const excptype p, wchar_t* t, const unsigned flags);
ETCH_EXCEPTION* new_etch_exception_ex(etchexception* x);
/* TODO why do we pass object + native value on the object value dtors? why not just object*? */
void destroy_etch_exception_value(ETCH_EXCEPTION* thisp, etchexception* value); 


#endif /* #ifndef ETCHEXCP_H */
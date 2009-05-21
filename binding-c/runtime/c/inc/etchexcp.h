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
 * etchexcp.h -- etch exception
 */

#ifndef ETCHEXCP_H
#define ETCHEXCP_H

#include "etchobj.h"


/** 
 * etchexception.flags bits
 */
#define ETCHEXCP_NONE       0
#define ETCHEXCP_STATICTEXT 1
#define ETCHEXCP_COPYTEXT   2
#define ETCHEXCP_FREETEXT   4
#define ETCHEXCP_CHECKED    8


/** 
 * exception type identifiers
 */
typedef enum excptype
{ 
  EXCPTYPE_NONE          = 0x0,  
  EXCPTYPE_GENERIC       = 0x1,
  EXCPTYPE_IO            = 0x2,
  EXCPTYPE_ILLEGALARG    = 0x3,
  EXCPTYPE_NULLPTR       = 0x4,
  EXCPTYPE_UNSUPPORTEDOP = 0x5,
  EXCPTYPE_INTERNALERR   = 0x6,
  EXCPTYPE_ETCHAUTH      = 0x10,
  EXCPTYPE_ETCHRUNTIME   = 0x11,
  EXCPTYPE_NOTFOUND      = 0x20,
  EXCPTYPE_CHECKED_START = 0x40,
  EXCPTYPE_CHECKED_BOGUS = 0x41,
} excptype; 


/** 
 * result codes
 */
typedef enum results
{ 
  ETCHRESULT_TRUE            =   1, 
  ETCHRESULT_OK              =   0,  
  ETCHRESULT_FALSE           =   0,
  ETCHRESULT_ERROR           = (-1),
  ETCHRESULT_NOTFOUND        = (-1),
  ETCHRESULT_EXCPERR_IO      = (-1),
  ETCHRESULT_EXCPERR_BADARG  = (-2),
  ETCHRESULT_EXCPERR_NULLPTR = (-3),
  ETCHRESULT_EXCPERR_BADOP   = (-4), 
  ETCHRESULT_EXCPERR_INTERNAL= (-5), 
} results; 

/* exception text */
const wchar_t* excptext_excp;
const wchar_t* excptext_io;
const wchar_t* excptext_nullptr;
const wchar_t* excptext_arg; 
const wchar_t* excptext_unsupop;
const wchar_t* excptext_internal;
const wchar_t* excptext_notfound;


/**
 * etchexception native object 
 */
struct etchexception
{   
    excptype excptype; 
    wchar_t* excptext;
    char*    ansitext; /* ansi copy of text for display etc always owned */
    size_t   textlen;  /* byte length of unicode text including nullterm */
    size_t   atxtlen;  /* byte length of ansi text including nullterm */
    unsigned flags;    /* ETCHEXCP_XXXX */
};

typedef struct etchexception etchexception;

/**
 * etch_throw()
 * "throw" specified exception, meaning instantiate exception and attach to object
 */
objmask* etch_throw  (void* obj, const excptype t, wchar_t* opttext, unsigned flags);

/**
 * etch_throwex()
 * as "throw" but also including result code and reason code
 */
objmask* get_excpobj(int objsize, short obj_type, short class_id,
    excptype xcptype, wchar_t* xcptext, unsigned flags);

/**
 * native ctors and dtor
 */
void destroy_exception(etchexception* excp);
etchexception*  new_exception     (const excptype t, wchar_t* x, const unsigned flags);
etch_exception* new_etch_exception(const excptype t, wchar_t* x, const unsigned flags); 

/**
 * default_excptext()
 * gets default text for specified exception type.
 */
wchar_t* default_excptext(const excptype t);

objmask* get_excpobj(int objsize, short obj_type, short class_id,
    excptype xcptype, wchar_t* xcptext, unsigned flags);

wchar_t* excp_tostring (etchexception*);
char*    excp_tostringx(etchexception*);

/**
 * catch, throw  
 */
boolean  is_catch_exception(objmask*j, const int result, objmask** out);
boolean  is_catch(objmask*, const int result, const int objtype, objmask** out);
objmask* catch_exception(objmask*, const int result);
objmask* throw_exception(objmask*, const int result);
objmask* throw_exception_from(const short obj_type, const int result);
objmask* throw_from(excptype, const short obj_type, wchar_t* text, unsigned flags);
objmask* etch_rethrow(void* target, void* source); 

objmask* verifyx(objmask* obj, const unsigned short, const unsigned short, excptype xtype);
etch_exception* get_etch_exception_from (objmask*);
etchexception*  get_exception_from (objmask*);
etch_exception* new_etch_exception_from (objmask*);

#endif /* #ifndef ETCHEXCP_H */
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
 * etch_excp.c -- exception objects native and boxed  
 */

#include "etch_encoding.h"
#include "etch_exception.h"
#include "etch_objecttypes.h"
#include "etch_log.h"
#include "etch_mem.h"

static const char* LOG_CATEGORY = "etch_exception";
/* builtin exception text */
const wchar_t* excptext_excp_builtin    = L"etch builtin exception"; 
const wchar_t* excptext_excp_user       = L"etch user defined exception"; 

/**
 * etch_exception
 */
struct etch_exception
{
    etch_object object;

    etch_string*    message;
    uint32          errorcode;
    excptype_t      excptype;

};

/*
 * default_excptext() 
 * find and return default text for exception type
 */
etch_string* default_excptext(const excptype_t type)
{
    wchar_t* text = NULL;
    etch_string* res = NULL;
    switch(type)
    {   
       case EXCPTYPE_BUILTIN:       text = (wchar_t*) excptext_excp_builtin;    break;
       case EXCPTYPE_USERDEFINED:   text = (wchar_t*) excptext_excp_user;       break;
       default:
           text = (wchar_t*) L"unknown error";
           ETCH_LOG(LOG_CATEGORY, ETCH_LOG_WARN, "no default exception text for type %d", type);
           break;
    }
    res = new_stringw(text);
    return res;
}


/**
 * destroy_etch_exception()
 */
int destroy_etch_exception(void* p)
{   
    etch_exception* ee = (etch_exception*) p;

    if ((! is_etchobj_static_content(ee)) && ee->message) {
        etch_object_destroy(ee->message);
    }

    return destroy_objectex((etch_object*) ee);
}


/**
 * clone_etch_exception()
 */
void* clone_etch_exception(void* data)
{   
    etch_exception* ee = (etch_exception*)data;
    etch_exception* newobj = NULL;
    newobj = new_etch_exception (ee->excptype);
    if(ee->message) {
        newobj->message = (etch_string*)etch_object_clone_func(ee->message);
    }else {
        newobj->message = NULL;
    }
    newobj->errorcode = ee->errorcode;
    return newobj;
}

/**
 * new_etch_exception()
 * constructor for wrapped exception
 */ 
etch_exception* new_etch_exception(const excptype_t type) 
{
    etch_exception* newobj = (etch_exception*) new_object
        (sizeof(struct etch_exception), ETCHTYPEB_EXCEPTION, CLASSID_EXCEPTION);

    ((etch_object*)newobj)->destroy = destroy_etch_exception;
    ((etch_object*)newobj)->clone   = clone_etch_exception;
    newobj->errorcode = 0;
    newobj->excptype = type;
    newobj->message = default_excptext(type);

    return newobj;
}

/**
 * create a new builtin etch exception with errorcode
 */
etch_exception* new_etch_exception_from_errorcode(int errorcode){
    etch_exception* result = new_etch_exception(EXCPTYPE_BUILTIN);
    result->errorcode = errorcode;
    return result;
}

void etch_exception_set_message(etch_exception* ex, etch_string* mess)
{
    if(ex->message){ 
        etch_object_destroy(ex->message);
    }
    ex->message = mess;
}

uint32 etch_exception_get_errorcode(etch_exception* ex) {
    if(ex){
        return ex->errorcode;
    }
    return 0;
}

etch_string* etch_exception_get_message(etch_exception* ex)
{
    return ex->message;
}

etch_status_t etch_exception_get_type(etch_exception* exception, excptype_t* type)
{
    if(exception == NULL || type == NULL) {
        return ETCH_EINVAL;
    }
    *type = exception->excptype;
    return ETCH_SUCCESS;
}













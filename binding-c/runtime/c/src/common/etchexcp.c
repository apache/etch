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
 * etchexcp.c -- exception objects native and boxed  
 */

#include <stdio.h>
#include "etch_encoding.h"
#include "etchexcp.h"
#include "etch_global.h"
#pragma warning (disable:4996)


/**
 * etch_throw()
 * "throw" specified exception on specified object, meaning create an exception 
 * and store it in the specified object's return value.
 * type: arbitrary exception type, not edited. if a known type and text is not 
 * supplied, attempt is made to locate and supply default text for that type.
 * text: optional text descriptive of the exception.
 * bitwise flags: 
 * ETCHEXCP_COPYTEXT: make a copy of supplied text and free it on destroy
 * ETCHEXCP_FREETEXT: at destroy time, free the exception text memory
 */
objmask* etch_throw(objmask* obj, const excptype type, wchar_t* text, unsigned flags)
{
    int result = 0;
    if (!obj) return NULL;

    if(!obj->result)
        obj->result = new_etchresult(0, 0);

    if (obj->result->exception)
        destroy_exception(obj->result->exception);

    obj->result->exception = new_exception(type, text, flags);
    return obj;
} 


/**
 * etch_throwex()
 * throw specified exception.
 * same as etch_throw but with etch_result parameters specified as well.
 */
objmask* etch_throwex(objmask* obj, const excptype type, wchar_t* text,  
    unsigned flags, const int resultcode, const int reasoncode) 
{
    if (NULL == etch_throw(obj, type, text, flags)) return NULL;
    obj->result->resultcode = resultcode;
    obj->result->reasoncode = reasoncode;
    return obj;
} 


/*
 * new_exception() 
 * instantiate a native exception object.
 * this is not an etch object, it is a raw struct, see new_etch_exception()
 * bitwise flags: 
 *   ETCHEXCP_COPYTEXT:  make a copy of supplied text and free it on destroy
 *   ETCHEXCP_FREETEXT:  at destroy time, free the exception text memory
 *   ETCHEXCP_UNCHECKED: exception is unchecked
 */
etchexception* new_exception(const excptype type, wchar_t* text, const unsigned flags)
{
    size_t numchars = 0;
    etchexception* excp = etch_malloc(sizeof(etchexception), ETCHTYPEB_EXCEPTION);
    memset(excp, 0, sizeof(etchexception));
    excp->excptype = type;
    excp->flags    = flags;

    if (text && !flags) /* if text without flags, default to copy text */
        excp->flags = ETCHEXCP_COPYTEXT;
    else
    if(!text) /* if no text, get default text and clear copy and free flags */
    {   text = default_excptext(type);
        excp->flags &= ~(ETCHEXCP_FREETEXT | ETCHEXCP_COPYTEXT);  
    }

    if (text) /* calculate byte length of text */
    {   numchars = wcslen(text) + 1;
        excp->textlen  = sizeof(wchar_t) * numchars;
    }
    
    if (text && (flags & ETCHEXCP_COPYTEXT))
    {   /* if asked to make a copy of the text, do so */
        excp->excptext = etch_malloc(excp->textlen, ETCHTYPEB_STRING);
        wcscpy_s(excp->excptext, numchars, text); 
        excp->flags |= ETCHEXCP_FREETEXT; /* indicate free on destroy */
    }    
    else excp->excptext = text; /* ETCHEXCP_FREETEXT may or may not be set */

    /* etch_malloc an ansi copy for display */
    if (0 == etch_unicode_to_ansi (&excp->ansitext, excp->excptext))
        excp->atxtlen = strlen(excp->ansitext);

    if (type >= EXCPTYPE_CHECKED_START)  /* mark checked excp as such */
        excp->flags |= ETCHEXCP_CHECKED;
        
    return excp;
}


/**
 * destroy_etch_exception()
 */
int destroy_etch_exception(etch_exception* ee)
{   
    if (ee->refcount > 0 && --ee->refcount > 0) return -1;  

    if (!is_etchobj_static_content(ee))
        destroy_exception(ee->value);

    return destroy_objectex((objmask*) ee);
}


/**
 * clone_etch_exception()
 */
etch_exception* clone_etch_exception (etch_exception* ee)
{   
    etch_exception* newobj = NULL;
    etchexception* excp = ee? (etchexception*) ee->value: NULL;
    if (!excp) return NULL;
    newobj = new_etch_exception (excp->excptype, excp->excptext, ETCHEXCP_COPYTEXT);
    return newobj;
}


/**
 * new_etch_exception()
 * constructor for wrapped exception
 */ 
etch_exception* new_etch_exception(const excptype type, wchar_t* text, 
    const unsigned flags) 
{
    etch_exception* newobj = (etch_exception*) new_object
        (sizeof(struct etch_exception), ETCHTYPEB_EXCEPTION, CLASSID_EXCEPTION);

    newobj->value = new_exception(type, text, flags);

    newobj->destroy = destroy_etch_exception;
    newobj->clone   = clone_etch_exception;

    return newobj;
}


/* builtin exception text */
const wchar_t* excptext_excp    = L"etch"; 
const wchar_t* excptext_io      = L"I/O";
const wchar_t* excptext_auth    = L"authorization";
const wchar_t* excptext_runtime = L"etch runtime";
const wchar_t* excptext_nullptr = L"null pointer";
const wchar_t* excptext_arg     = L"illegal argument";  
const wchar_t* excptext_unsupop = L"unsupported operation"; 
const wchar_t* excptext_internal= L"internal error";
const wchar_t* excptext_notfound= L"not found";


/*
 * default_excptext() 
 * find and return default text for exception type
 */
wchar_t* default_excptext(const excptype type)
{
    wchar_t* text = NULL;

    switch(type)
    {   
       case EXCPTYPE_GENERIC:       text = (wchar_t*) excptext_excp;    break;  
       case EXCPTYPE_ETCHRUNTIME:   text = (wchar_t*) excptext_runtime; break;  
       case EXCPTYPE_ETCHAUTH:      text = (wchar_t*) excptext_auth;    break;   
       case EXCPTYPE_IO:            text = (wchar_t*) excptext_io;      break;   
       case EXCPTYPE_NULLPTR:       text = (wchar_t*) excptext_nullptr; break;    
       case EXCPTYPE_ILLEGALARG:    text = (wchar_t*) excptext_arg;     break; 
       case EXCPTYPE_UNSUPPORTEDOP: text = (wchar_t*) excptext_unsupop; break;
       case EXCPTYPE_INTERNALERR:   text = (wchar_t*) excptext_internal;break;
       case EXCPTYPE_NOTFOUND:      text = (wchar_t*) excptext_notfound;break;
    }

    return text;
}


/*
 * destroy_exception()
 * free memory for specified native exception object
 */
void destroy_exception (etchexception* excp)
{
    if(!excp) return;  /* text may be static or freed elsewhere */

    if (excp->flags & ETCHEXCP_FREETEXT)
    {   ETCHOBJ_FREE(excp->excptext);   /* free exception text */ 
    }

    /* always allocated in ctor, never passed in */
    ETCHOBJ_FREE(excp->ansitext);
   
    etch_free(excp);   /* free exception wrapper */
}


/*
 * get_excpobj()
 * create and return an etchobject for the sole purpose of returning an exception.
 * we might use this functionality where our method returns an object which we have
 * not yet created, we edit parameters, find them wanting, and wish to throw an 
 * exception, and need an object in which to return the exception.
 */ 
objmask* get_excpobj (int objsize, short obj_type, short class_id,
    excptype xcptype, wchar_t* xcptext, unsigned flags)
{
    objmask* newobj = new_object(objsize, obj_type, class_id);
    newobj->is_null = TRUE; 
    etch_throw(newobj, xcptype, xcptext, flags);
    return newobj;
} 


/*  
 * get_excptype()
 * return exception type given result code
 */
excptype get_excptype(const results result)
{
    excptype xtype;
    if (result >= 0) return EXCPTYPE_NONE;

    switch(result)
    {    
        case ETCHRESULT_EXCPERR_IO:      xtype = EXCPTYPE_IO;            break;
        case ETCHRESULT_EXCPERR_BADARG:  xtype = EXCPTYPE_ILLEGALARG;    break;
        case ETCHRESULT_EXCPERR_NULLPTR: xtype = EXCPTYPE_NULLPTR;       break;
        case ETCHRESULT_EXCPERR_BADOP:   xtype = EXCPTYPE_UNSUPPORTEDOP; break;
        default: xtype = EXCPTYPE_GENERIC;  
    }
    return xtype;
}


/*  
 * excp_tostring
 * returns unicode string representation of exception
 * caller owns returned buffer
 */
wchar_t* excp_tostring (etchexception* excp)
{
    wchar_t *newbuf = NULL;
    const wchar_t *x = L"exception ", charlen = sizeof(wchar_t), *space = L" " ;
    const size_t xbytelen = wcslen(x) * charlen;
    wchar_t* deftext = default_excptext(excp->excptype);
    const size_t deftextbytelen = deftext? 
          wcslen(deftext) * charlen: 0;
    const size_t excptextbytelen = excp->excptext? 
          wcslen(excp->excptext) * charlen: 0;
    wchar_t* excptext 
           = excptextbytelen? excp->excptext: 
             deftextbytelen?  deftext:
             L"generic";
    const size_t textbytelen = wcslen(excp->excptext) * charlen;
    const size_t buflen = xbytelen + textbytelen + (3 * charlen);

    newbuf = etch_malloc(buflen, ETCHTYPEB_BYTES);
    memset(newbuf, 0, buflen);
    
    wcscat(newbuf, excptext); /* e.g. "null pointer" */
    wcscat(newbuf, space);
    wcscat(newbuf, x);        /* "exception " */

    return newbuf; 
}


/*  
 * excp_tostringx
 * returns ascii string representation of exception
 * caller owns returned buffer
 */
char* excp_tostringx(etchexception* excp)
{
    char* ansibuf = NULL;
    wchar_t*  unibuf = excp_tostring(excp);
    const int result = etch_unicode_to_ansi(&ansibuf, unibuf); 
    etch_free(unibuf);
    return ansibuf; 
}


/*  
 * is_catch_exception()
 * "catch" exception defined by result code, returning FALSE if no exception.
 * the etchobject wrapping the exception is returned in the out parameter. 
 * this object is created if no such object is passed.
 */
boolean is_catch_exception(objmask* obj, const int result, objmask** outobj)
{
    objmask* excpobj = throw_exception(obj, result);

    if (is_exception(excpobj))
    {
        if (outobj) *outobj = excpobj;
        return TRUE;
    }

    return FALSE;
}


/*  
 * is_catch()
 * "catch" exception defined by result code, returning FALSE if no exception.
 * the etchobject wrapping the exception is returned in the out parameter. 
 * this object is created if no such object is passed.
 */
boolean is_catch(objmask* obj, const int result, const int objtype, objmask** outobj)
{
    objmask* excpobj = obj? 
        throw_exception(obj, result):
        throw_exception_from(objtype, result);

    if (is_exception(excpobj))
    {
        if (outobj) *outobj = excpobj;
        return TRUE;
    }

    return FALSE;
}


/*  
 * catch_exception()
 * return an etchobject containing the exception mapping to the specified 
 * result code. an anonymous object is generated if none is supplied.
 */
objmask* catch_exception(objmask* obj, const int result)
{
    return throw_exception(obj, result);
}


/*  
 * throw_exception()
 * return an etchobject of the specified type containing the exception mapping to
 * the specified result code. an anonymous object is generated if none is supplied.
 */
objmask* throw_exception(objmask* obj, const int result)
{
    excptype xtype = get_excptype(result);
    if (obj == NULL) 
        obj = get_excpobj(sizeof(objmask), ETCHTYPEB_UNDEFINED, 0, xtype, NULL, 0);

    if (xtype != EXCPTYPE_NONE)  
        etch_throw(obj, xtype, NULL, 0);

    return obj;
}


/*  
 * throw_exception_from()
 * generate and return an etch object of the specified type containing the exception
 * mapping to the specified result code.  
 */
objmask* throw_exception_from(const short obj_type, const int result)
{
    objmask* obj = NULL;
    excptype xtype  = get_excptype(result);
    obj = get_excpobj(sizeof(objmask), obj_type, 0, xtype, NULL, 0);
    return obj;
}


/*  
 * throw_from()
 * generate and return an etchobject of the specified type 
 * containing the specified exception
 */
objmask* throw_from(excptype xtype, const short obj_type, wchar_t* text, unsigned flags)
{
    return get_excpobj(sizeof(objmask), obj_type, 0, xtype, text, flags);
}


/*  
 * etch_rethrow
 * throw an exception from an existing exception 
 */
objmask* etch_rethrow(objmask* target, objmask* source)
{
    etchexception* excp = NULL;
    if (target != NULL)
        excp = get_exception(source);
    if (excp == NULL) return throw_from(EXCPTYPE_INTERNALERR, target->obj_type, NULL, 0);
    
    return etch_throwex(target, excp->excptype, excp->excptext, excp->flags, 
                source->result->resultcode, source->result->reasoncode);     
}


/*  
 * get_etch_exception_from()
 * returns the etch_exception object if any from the specified etch object.
 * @return a non-disposable reference to the etch_exception object, or NULL.
 */
etch_exception* get_etch_exception_from (objmask* obj)
{   
    if (obj && (obj->obj_type == ETCHTYPEB_EXCEPTION)) 
        return (etch_exception*) obj;
    return NULL;
}


/*  
 * get_exception_from
 * returns the unwrapped etchexception if any from the specified etch object.
 */
etchexception* get_exception_from (objmask* obj)
{   
    if (NULL == obj) return NULL;
    if (obj->obj_type == ETCHTYPEB_EXCEPTION) 
        return (etchexception*) ((etch_exception*) obj)->value;
    if (obj->result) return obj->result->exception;
    return NULL;
}


/*  
 * get_exception_from()
 * returns a disposable exception object based on the exception contained in
 * the specified object.
 */
etch_exception* new_etch_exception_from (objmask* obj)
{
    etch_exception* x = NULL;
    etchexception* excp = get_exception_from (obj);
    if (excp)
        x = new_etch_exception (excp->excptype, excp->excptext, ETCHEXCP_COPYTEXT);
    return x;
}


/*  
 * verifyx()
 * test specified object for specified type, throw specified exception if not
 */
objmask* verifyx(objmask* obj, 
    const unsigned short objtype, const unsigned short classid, excptype xtype)
{
    if(!obj)
        obj = throw_from(xtype, objtype, NULL, 0);
    else
    if ((objtype != 0 && obj->obj_type != objtype)
     || (classid != 0 && obj->class_id != classid)) 
        etch_throwex(obj, xtype, NULL, 0, -1, 0);

    return obj;
}










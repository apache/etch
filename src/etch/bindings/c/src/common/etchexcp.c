/*
 * etchexcp.c -- exception objects native and boxed  
 */
#include <stdio.h>
#include "etchexcp.h"
#include "etch_global.h"


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
int etch_throw(etchobject* obj, const excptype type, wchar_t* text, unsigned flags)
{
    int result = 0;
    if (!obj) return -1;

    if(!obj->result)
        obj->result = new_etchresult(0, 0);

    if (obj->result->exception)
        destroy_exception(obj->result->exception);

    obj->result->exception = new_exception(type, text, flags);

    return obj->result->exception? 0: -1;
} 


/**
 * etch_throwex()
 * throw specified exception.
 * same as etch_throw but with etch_result parameters specified as well.
 */
int etch_throwex(etchobject* obj, const excptype type, wchar_t* text,  
    unsigned flags, const int resultcode, const int reasoncode) 
{
    const int result = etch_throw(obj, type, text, flags);
    if (result == -1 || !obj->result) return -1;

    obj->result->resultcode = resultcode;
    obj->result->reasoncode = reasoncode;
    return obj->result->exception? 0: -1;
} 


/*
 * new_exception() 
 * instantiate a native exception object
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

    if(!text) /* if no text, get default text and clear copy and free flags */
    {   text = default_excptext(type);
        excp->flags &= ~(ETCHEXCP_FREETEXT |ETCHEXCP_COPYTEXT);  
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

    if (type >= EXCPTYPE_CHECKED_START)  /* mark checked excp as such */
        excp->flags |= ETCHEXCP_CHECKED;
        
    return excp;
}


/* temporary excp text */
wchar_t* excptext_bogus     = L"bogus error"; 
wchar_t* excptext_io        = L"I/O";
wchar_t* excptext_nullptr   = L"null pointer";


/*
 * default_excptext() 
 * find and return default text for exception type
 */
wchar_t* default_excptext(const excptype type)
{
    wchar_t* text = NULL;
    static wchar_t* bogustext = L"some exception text TBD";

    switch(type)
    {  /* TODO populate default text for all applicable exceptions
        * we will shortly install exceptions in a hashtable, and lookup via
        * hash of excptype, not in a switch like this.
        */
       case EXCPTYPE_SOME_UNCHECKED_TBD:  text = excptext_bogus; break;   
       case EXCPTYPE_IO:                  text = excptext_io; break;   
       case EXCPTYPE_NULLPTR:             text = excptext_nullptr; break;    
      
    }

    return text;
}


/*
 * new_etch_exception() 
 * instantiate a boxed exception object
 */
ETCH_EXCEPTION* new_etch_exception(const excptype type, wchar_t* text, const unsigned flags) 
{
    ETCH_EXCEPTION* newobj = NULL;
    etchexception* excp = new_exception(type, text, flags);

    if (NULL != excp)  
        newobj = new_etch_exception_ex(excp);

    return newobj;
}


/*
 * destroy_exception()
 * free memory for specified native exception object
 */
void destroy_exception(etchexception* excp)
{
    if(!excp) return;   /* text may be static or freed elsewhere */
    if (excp->excptext && (excp->flags & ETCHEXCP_FREETEXT))
        etch_free(excp->excptext); /* free exception text */ 
    etch_free(excp);    /* free exception wrapper */
}


/*
 * destroy_etch_exception_value() 
 * destructor for value of a boxed exception object
 */
void destroy_etch_exception_value(ETCH_EXCEPTION* thisp, etchexception* value)
{
   if (thisp && !thisp->is_static_data) /* unless flagged to not do so ... */
       destroy_exception(value);        /* ... call native destructor */
} 


/*
 * new_etch_exception_ex() 
 * create a boxed exception object
 */ 
ETCH_EXCEPTION* new_etch_exception_ex(etchexception* value) 
{   
    ETCH_EXCEPTION* newobj = new_etchobject(ETCHTYPE_BOXED_EXCEPTION);
    if (NULL == newobj) return NULL;

    newobj->value_ptr_to = value;  /* can be NULL */
    newobj->size = sizeof(etchexception); /* TODO account the id_name content in size? */
    /* TODO if object.size for string is item length, then should size for an exception 
     * be the size of the struct wrapper plus the size of the allocated text? 
     */

    /* override the default object vtable impl of destroy_value() */
    newobj->vtab->destroy_value = destroy_etch_exception_value;
    return newobj;
}


/**
 * etch_objects.c 
 * etch type object wrappers construction
 */

#include <stdio.h>
#include "etch_objects.h"
#include "etchmem.h"
#include "etchrun.h"
#include "etch_arraylist.h"


/**
 * new_etch_int32()
 * constructor for ETCH_INT32, a boxed 32-bit integer
 */
ETCH_INT32* new_etch_int32(const int value)
{
    etchobject* newobj = new_etchobject(ETCHTYPE_BOXED_INT32);
    newobj->value_int32 = value;
    newobj->size = sizeof(int);
    return newobj;
}

/**
 * new_etch_int8()
 * constructor for ETCH_INT8, a boxed 8-bit integer
 */
ETCH_INT8* new_etch_int8(const signed char value)
{
    etchobject* newobj = new_etchobject(ETCHTYPE_BOXED_INT8);
    newobj->value_int8 = value;
    newobj->size = sizeof(char);
    return newobj;
}

/**
 * new_etch_int16()
 * constructor for ETCH_INT16, a boxed 16-bit integer
 */
ETCH_INT16* new_etch_int16(const short value)
{
    etchobject* newobj = new_etchobject(ETCHTYPE_BOXED_INT16);
    newobj->value_int16 = value;
    newobj->size = sizeof(short);
    return newobj;
}


/**
 * new_etch_int64()
 * constructor for ETCH_INT64, a boxed 64-bit integer
 */
ETCH_INT64* new_etch_int64(const int64 value)
{
    etchobject* newobj = new_etchobject(ETCHTYPE_BOXED_INT64);
    newobj->value_int64 = value;
    newobj->size = sizeof(int64);
    return newobj;
}

/**
 * new_etch_byte()
 * constructor for ETCH_BYTE, a boxed unsigned char
 */
ETCH_BYTE* new_etch_byte(const unsigned char value)
{
    etchobject* newobj = new_etchobject(ETCHTYPE_BOXED_BYTE);
    newobj->value_byte = value;
    newobj->size = sizeof(char);
    return newobj;
}

/**
 * new_etch_bool()
 * constructor for ETCH_BOOL, a boxed unsigned char
 */

ETCH_BOOL* new_etch_bool(const unsigned char value)
{
    etchobject* newobj = new_etchobject(ETCHTYPE_BOXED_BOOL);
    newobj->value_byte = value? TRUE: FALSE;
    newobj->size = sizeof(char);
    return newobj;
}

/**
 * new_etch_ieee32()
 * constructor for ETCH_IEEE32, a boxed float
 */
ETCH_IEEE32* new_etch_ieee32(const float value)
{
    etchobject* newobj = new_etchobject(ETCHTYPE_BOXED_IEEE32);
    newobj->value_ieee32 = value;
    newobj->size = sizeof(float);
    return newobj;
}

/**
 * new_etch_ieee64()
 * constructor for ETCH_IEEE64, a boxed float
 */
ETCH_IEEE64* new_etch_ieee64(const double value)
{
    etchobject* newobj = new_etchobject(ETCHTYPE_BOXED_IEEE64);
    newobj->value_ieee64 = value;
    newobj->size = sizeof(double);
    return newobj;
}

/**
 * new_etch_string()
 * constructor for ETCH_STRING, a boxed array of wchar_t
 */
ETCH_STRING* new_etch_string(const wchar_t* value)
{
    etchobject* newobj = NULL;
    size_t bytelen = 0;
    size_t numchars = value? wcslen(value) + 1: 0;
    bytelen = sizeof(wchar_t) * numchars;

    newobj = new_etchobject(ETCHTYPE_BOXED_STRING);
    newobj->value_ptr_to = (wchar_t*)value;
    newobj->size = bytelen;
    return newobj;
}


/**
 * destroy_boxed_arraylist(()
 * destructor for ETCH_ARRAYLIST, a boxed etch_arraylist.
 */
void destroy_boxed_arraylist(ETCH_ARRAYLIST* thisp)
{
  if (!thisp->is_static_data && thisp->value_ptr_to)   
      arraylist_destroy((etch_arraylist*)thisp->value_ptr_to, TRUE); 

  if (!thisp->is_static_object)
      etch_free(thisp);
}



/**
 * new_etch_arraylist(initsize, delta)
 * constructor for ETCH_ARRAYLIST, a boxed etch_arraylist.
 * initsize: initial number of entries allocated, default configured.
 * deltsize: number of entries added upon expansion, default initsize.
 * is_readonly: if TRUE, memory for arraylist content will not be freed
 * upon invoking destroy() (of course, more control could be exerted 
 * by operating on the underlying etch_arraylist directly).
 */
ETCH_ARRAYLIST* new_etch_arraylist
    (const unsigned initsize, const unsigned deltsize, const unsigned is_readonly)
{
    etchobject* newobj = NULL;
    i_etchobject* vtab = NULL;
    const int VTAB_KEY = ETCHTYPEB_ARRAYLIST;

    etch_arraylist* list = new_arraylist(initsize, deltsize);
    if (NULL == list) return NULL; 

    newobj = new_etchobject(ETCHTYPE_BOXED_ARRAYLIST);
    newobj->value_ptr_to = list;
    newobj->size = list->size; /* TODO size as elsewhere */
    list->is_readonly = is_readonly != 0; /* static content indicator */

    vtab = cache_find(VTAB_KEY); 

    if(!vtab)  /* cache the vtable for this object type */
    {  
        vtab = new_vtable(newobj->vtab, ETCHOBJ_VTABSIZE, ETCHTYPEB_VTABLE, 0);

        /* function overrides: TODO override clone() */
        vtab->destroy = destroy_boxed_arraylist;   
        
        vtab->vtab = newobj->vtab; /* chain parent vtable to override vtable */

        cache_add(VTAB_KEY, vtab); /* cache the new vtable */
    } 

    newobj->vtab = vtab;  /* set override vtable */
    return newobj;
}






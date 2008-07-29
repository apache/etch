/*  
 * etch_arrayval.c -- etch_array_value implementation.
 */

#include "etch_arrayval.h"
#include "etch_global.h"

/*  
 * private methods
 */
etch_array_value* new_arrayvalue_init(const int, const int, const int);
int  arrayvalue_read_values(etch_array_value* thisp, tagged_data_input* tdi);
int  arrayvalue_write_values(etch_array_value* thisp, tagged_data_output* tdo);
int  array_value_add(etch_array_value* thisp, ETCH_ARRAY_ELEMENT* content);


/**
 * new_boxed_array_value()
 * constructor for ETCH_ARRAYVALUE, a boxed etch_array_value.
 * initsize: initial number of entries allocated, default configured.
 * deltsize: number of entries added upon expansion, default initsize.
 * is_readonly: if TRUE, memory for arraylist content will not be freed
 * upon invoking destroy() 
 */
ETCH_ARRAYVALUE* new_boxed_array_value
    (const byte type_code, etch_type* custom_struct_type, 
     const int dim, const int initsize, const int deltsize, const int is_readonly)
{
    etchobject* newobj = NULL;
    i_etchobject* vtab = NULL;
    const int VTAB_KEY = ETCHTYPEB_ARRAYVAL;

    etch_array_value* arrayval = new_arrayvalue
       (type_code, custom_struct_type, dim, initsize, deltsize, is_readonly);
    if (NULL == arrayval) return NULL; 

    newobj = new_etchobject(ETCHTYPE_BOXED_ARRAYVALUE);
    newobj->value_ptr_to = arrayval;
    newobj->size = sizeof(etch_array_value); /* TODO size as elsewhere */

    vtab = cache_find(VTAB_KEY); 

    if(!vtab)  /* cache the vtable for this object type */
    {  
        vtab = new_vtable(newobj->vtab, ETCHOBJ_VTABSIZE, ETCHTYPEB_VTABLE, 0);

        /* function overrides: TODO override clone() */
        vtab->destroy = destroy_boxed_arrayvalue;   
        
        vtab->vtab = newobj->vtab; /* chain parent vtable to override vtable */

        cache_add(VTAB_KEY, vtab); /* cache the new vtable */
    } 

    newobj->vtab = vtab;  /* set override vtable */
    return newobj;
}


/**
 * new_arrayvalue()
 * primary constructor for etch_array_value
 */
etch_array_value* new_arrayvalue(const byte type_code, etch_type* custom_struct_type, 
    const int dim, const int initsize, const int deltsize, const int is_readonly)
{
    etch_array_value* newobj = new_arrayvalue_init(initsize, deltsize, is_readonly);

    newobj->dim = dim;
    newobj->type_code = type_code;

    if (custom_struct_type) /* we make a copy of the specified type */
        memcpy(&newobj->custom_struct_type, custom_struct_type, sizeof(etch_type));

    return newobj;
}


/**
 * new_arrayvalue_default()
 * default constructor for etch_array_value
 */
etch_array_value* new_arrayvalue_default()
{
    etch_array_value* newobj = new_arrayvalue_init
        (ETCH_ARRAYVALUE_DEFAULT_INITSIZE, 
         ETCH_ARRAYVALUE_DEFAULT_DELTSIZE, 
         ETCH_ARRAYVALUE_DEFAULT_READONLY);

    return newobj;
}


/**
 * new_arrayvalue_init() (private method)
 * common initialization on etch_array_value construction.
 */
etch_array_value* new_arrayvalue_init(const int initsize, const int deltsize, const int is_readonly)
{
    etch_array_value* newobj = etch_malloc(sizeof(etch_array_value), ETCHTYPEB_ARRAYVAL);
    memset(newobj, 0, sizeof(etch_array_value));

    /** the underlying arraylist is marked as content type object, meaning we can
     *  interpret content as etchobject and call methods on the object accordingly,
     *  most notably destroy(). when is_readonly is true, the underlying list
     *  will not free memory for content when the list is destroyed.
     */
    newobj->list = new_arraylist(initsize, deltsize);  
    newobj->list->is_readonly  = is_readonly != 0;  
    newobj->list->content_type = ETCHARRAYLIST_CONTENT_OBJECT;

    return newobj;
}


/**
 * destroy_array_value()
 * destructor for an etch_array_value object
 */
void destroy_arrayvalue(etch_array_value* thisp)
{
    if (NULL == thisp) return;
        
    /* destroy the underlying arraylist */
    arraylist_destroy(thisp->list, !thisp->list->is_readonly);

    etch_free(thisp); /* destroy the object */
}


/**
 * destroy_boxed_arrayvalue()
 * destructor for an ETCH_ARRAYVALUE object
 */
void destroy_boxed_arrayvalue(ETCH_ARRAYVALUE* thisp)
{
    if (NULL == thisp) return;
        
    /* destroy the underlying arrayvalue */
    destroy_arrayvalue(thisp->value_ptr_to);

    etch_free(thisp); /* destroy the object */
}


/**
 * arrayvalue_add()
 * caller will "throw" a similar exception to what a collection.add would throw
 * if the result is -1 here.
 */
int arrayvalue_add(ETCH_ARRAYVALUE* thisp, ETCH_ARRAY_ELEMENT* content)
{
    etch_array_value* av = thisp->value_ptr_to;

    const int result = arraylist_add(av->list, content);
    return result;
}


/**
 * array_value_add()
 * caller will "throw" a similar exception to what a collection.add would throw
 * if the result is -1 here.
 */
int array_value_add(etch_array_value* thisp, ETCH_ARRAY_ELEMENT* content)
{
    const int result = arraylist_add(thisp->list, content);
    return result;
}


/**
 * arrayvalue_read_values() (private)
 * Reads the values of the array. Does not clear the array first.
 * @param tdi the tagged data input to read from.
 * @throws IOException if there is a problem reading from the tagged data input.
 * caller will throw the exception if the result is -1 here.
 */
int arrayvalue_read_values(etch_array_value* thisp, tagged_data_input* tdi)
{
    int result = 0; /* TODO validate params */
    /* tdi.read_array_element reads into this pointer */
	ETCH_ARRAY_ELEMENT* returned_obj = NULL;  

    /** the java impl does a this.add(ae.value) here. our syntax differs since
     *  we don't wrap the value, the element is the object which is the value,
     *  which is the content, so we are logically and functionally equivalent.        
     */
    while(TRUE == (result = tdi->vtab->read_array_element(tdi, &returned_obj)))
		  array_value_add(thisp, returned_obj); 

    return result == -1? -1: 0;
}


/**
 * arrayvalue_read() (static)
 * Reads an array and all of its values.
 * @param tdi the tagged data input to read from.
 * @return an array value read from the tagged data input.
 * @throws IOException if there is a problem reading from the tagged data input.
 * caller will throw the exception if the result is NULL here.
 */
ETCH_ARRAYVALUE* arrayvalue_read(tagged_data_input* tdi)
{
    int result = 0;
    ETCH_ARRAYVALUE*  newobj = NULL;
    etch_array_value* newarray = NULL;
    if (!tdi) return NULL; /* TODO validate object */

    newobj = tdi->vtab->start_array(tdi); 
	if (NULL == newobj) return NULL;
    newarray = newobj->value_ptr_to;
    
    result = arrayvalue_read_values(newarray, tdi);
    if (-1 == result)
        return NULL;  

	return tdi->vtab->end_array(tdi, newobj) == 0? newobj: NULL;  /* TODO excp */
}


/**
 * Writes the values of the array. (private)
 * @param tdo the tagged data output to write to.
 * @throws IOException if there is a problem writing to the tagged data output.
 * caller will throw the exception if the result is -1 here. 
 */
int arrayvalue_write_values(etch_array_value* thisp, tagged_data_output* tdo)
{
    void** p; /* TODO validate params */
    int i = 0, result = 0, count = 0;
    if (!thisp || !thisp->list || !tdo) return -1;
    count = thisp->list->count;
    p  = thisp->list->base;

    for(; i < count && result == 0; i++, p++)
        result = tdo->vtab->write_array_element(tdo, (etchobject*) *p );

    return result;
}


/**
 * Writes an array and all of its values.
 * @param tdo the tagged data output to write to.
 * @throws IOException if there is a problem writing to the tagged data output.
 * caller will throw the exception if the result is -1 here.
 */
int arrayvalue_write(ETCH_ARRAYVALUE* thisp, tagged_data_output* tdo) 
{
    etch_array_value* av = NULL; 
    int result = 0;
    if (!thisp || !tdo) return -1; /* TODO validate object */ 
	tdo->vtab->start_array(tdo, thisp);  
    av = thisp->value_ptr_to; 
	result = arrayvalue_write_values(av, tdo);
	tdo->vtab->end_array(tdo, thisp);
    return result;
}


/**
 * new_array_element()
 * as ported from java, used when populating an array with nonspecific objects.
 */
ETCH_ARRAY_ELEMENT* new_array_element(const int objtype)
{
   return new_etchobject(objtype? objtype: ETCHTYPE_BOXED_UNDEFINED);
}


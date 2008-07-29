/*  
 * etch_tagdata_out.c -- tagged_data_output implementation.
 */

#include "etch_tagdata_out.h"
#include "etch_global.h"
#include <stdio.h>

/**
 * tdo_start_message()
 * Starts reading a message from the stream.
 * @return the message that we are reading.
 * @throws IOException if there is a problem with the stream.
 */
ETCH_MESSAGE* tdo_start_message(tagged_data_output* tdo)
{
    return 0;
}


/**
 * tdo_end_message()
 * Ends a message that we are reading.
 * @param msg the message that has been read.
 * @throws IOException if there is a problem with the stream.
 */
int tdo_end_message(tagged_data_output* tdo, ETCH_MESSAGE* msg)
{
    return 0;
}


/**
 * Writes the beginning of struct data. The type of the struct is
 * written. The fields and values of the struct are written
 * using {@link #writeStructElement(Field, Object)}.
 * @param struct the struct being written out.
 * @throws IOException if there is a problem with the stream.
 * @see #endStruct(StructValue)
 */
int tdo_start_struct(tagged_data_output* tdo, struct etch_struct_value* sv)
{
    return 0;
}


/**
 * tdo_write_struct_element()
 * Writes a struct element with the specified key and value.
 * @param key
 * @param value
 * @throws IOException if there is a problem with the stream.
 */
int tdo_write_struct_element(tagged_data_output* tdo, etch_field* f, etchobject* x)
{
    return 0;
}


/**
 * tdo_end_struct()
 * Ends a struct that we are reading.
 * @param struct the struct that we read.
 * @throws IOException if there is a problem with the stream.
 */
int tdo_end_struct(tagged_data_output* tdo, struct etch_struct_value* sv)
{
    return 0;
}


/**
 * tdo_start_array()
 * Starts reading an array from the stream.
 * @return the array that we are reading.
 * @throws IOException if there is a problem with the stream.
 * TODO: why do we return the same arrayval we were passed? Is this merely an
 * artifact of the port? Or might we in another scenario be passed NULL, and
 * the implementation would then create and return a new arrayvalue? One point
 * for: done this way we can throw an exception, whereas we could not do so
 * were the return value an int. Perhaps this was the original intent then.
 */
ETCH_ARRAYVALUE* tdo_start_array(tagged_data_output* tdo, ETCH_ARRAYVALUE* thisp)
{
    return 0;
}


/**
 * tdo_write_array_element()
 * Writes an array element with the specified value.
 * @param value
 * @throws IOException if there is a problem with the stream.
 */
int tdo_write_array_element(tagged_data_output* tdo, etchobject* x)
{
    return 0;
}

/**
 * tdo_end_array()
 * Ends an array that we are reading.
 * @param array the array that we read.
 * @throws IOException if there is a problem with the stream.
 */
int tdo_end_array(tagged_data_output* tdo, ETCH_ARRAYVALUE* x)
{
    return 0;
}


/**
 * new_tagged_data_output()
 * tagged_data_output constructor 
 */
tagged_data_output* new_tagged_data_output(const short objtype, const short class_id)
{
    tagged_data_output* tdo = NULL;
    i_tagged_data_output* vtab = NULL;
    const int VTAB_KEY = ETCHTYPE_VTABLE_TAGDATAOUT;
    int result = 0;

    tdo = etch_malloc(sizeof(tagged_data_output), ETCHTYPEB_TAGDATAOUT);
    memset(tdo, 0, sizeof(tagged_data_output));
    tdo->obj_type = objtype;
    tdo->class_id = class_id;

    vtab = cache_find(VTAB_KEY);

    if(!vtab)  
    {    
        vtab = new_vtable(NULL, 
            sizeof(i_tagged_data_output), ETCHTYPEB_VTABLE, ETCHTYPE_TAGDATAOUT);

        /* virtual function implementations */
        vtab->start_message = tdo_start_message;
        vtab->end_message   = tdo_end_message;
        vtab->start_struct  = tdo_start_struct;
        vtab->write_struct_element = tdo_write_struct_element;
        vtab->end_struct    = tdo_end_struct;
        vtab->start_array   = tdo_start_array;
        vtab->write_array_element = tdo_write_array_element;
        vtab->end_array     = tdo_end_array;

        cache_add(VTAB_KEY, vtab);
    } 
 
    tdo->vtab = vtab;   
    return tdo; 
}


/**
 * destroy_boxed_tagged_data_output()
 * ETCH_TAGDATAOUTPUT destructor, of signature etchobj->destroy 
 */
void destroy_boxed_tagged_data_output(ETCH_TAGDATAOUTPUT* tdoobj)
{
    tagged_data_output* tdo = tdoobj->value_ptr_to;
 
    destroy_instancedata(tdo->impl, 0); /* extract and invoke data dtor */

    etch_free(tdo);     /*destroy TDO object */

    etch_free(tdoobj);  /* destroy object shell */
}


/**
 * new_boxed_tagged_data_output()
 * boxed tagged_data_output object constructor 
 */
ETCH_TAGDATAOUTPUT* new_boxed_tagged_data_output(const short objtype, const short class_id)
{
    etchobject* newobj = NULL;
    tagged_data_output* newtdo = NULL;
    i_etchobject*  vtab = NULL;
    const int VTAB_KEY = ETCHTYPE_VTABLE_TDOOBJ;
    newtdo = new_tagged_data_output(objtype, class_id);
    if (NULL == newtdo) return NULL; 

    newobj = new_etchobject(ETCHTYPE_BOXED_TAGDATAOUT);
    newobj->value_ptr_to = newtdo;
    newobj->size = sizeof(tagged_data_output); 
    /* printf("searching cache for %d\n", VTAB_KEY); cache_dump(); */

    vtab = cache_find(VTAB_KEY);  

    if(!vtab)  /* cache the vtable for this object type */
    {  
        vtab = new_vtable(newobj->vtab, ETCHOBJ_VTABSIZE, ETCHTYPEB_VTABLE, 0);

        /* function overrides: TODO override clone() */
        vtab->destroy = destroy_boxed_tagged_data_output;  
       
        vtab->vtab = newobj->vtab;  /* chain parent vtable */

        cache_add(VTAB_KEY, vtab); /* cache the new vtable */
    } 

    newobj->vtab = vtab;  /* set override vtable */

    return newobj;
}


/*  
 * etch_tagdata_inp.c -- tagged_data_input implementation.
 */

#include "etch_tagdata_inp.h"
#include "etch_global.h"

/**
 * tdi_start_message()
 * Starts reading a message from the stream.
 * @return the message that we are reading.
 * @throws IOException if there is a problem with the stream.
 */
ETCH_MESSAGE* tdi_start_message(tagged_data_input* tdi)
{
    return 0;
}


/**
 * tdi_end_message()
 * Ends a message that we are reading.
 * @param msg the message that has been read.
 * @throws IOException if there is a problem with the stream.
 */
int tdi_end_message(tagged_data_input* tdi, ETCH_MESSAGE* msg)
{
    return 0;
}


/**
 * 
 * Starts reading a struct from the stream.
 * @return the struct that we are reading.
 * @throws IOException if there is a problem with the stream.
 */
ETCH_STRUCTVALUE* tdi_start_struct(tagged_data_input* tdi)
{
    return 0;
}


/**
 * tdi_write_struct_element()
 * Writes a struct element with the specified key and value.
 * @param key
 * @param value
 * @throws IOException if there is a problem with the stream.
 */
int tdi_read_struct_element(tagged_data_input* tdi, struct etch_struct_element* x)
{
    return 0;
}


/**
 * tdi_end_struct()
 * Ends a struct that we are reading.
 * @param struct the struct that we read.
 * @throws IOException if there is a problem with the stream.
 */
int tdi_end_struct(tagged_data_input* tdi, ETCH_STRUCTVALUE* sv)
{
    return 0;
}


/**
 * tdi_start_array()
 * Starts reading an array from the stream.
 * @return the array that we are reading.
 * @throws IOException if there is a problem with the stream.
 */
ETCH_ARRAYVALUE* tdi_start_array(tagged_data_input* tdi)
{
    return 0;
}


/**
 * tdi_read_array_element()
 * @throws IOException if there is a problem with the stream.
 */
int tdi_read_array_element(tagged_data_input* tdi, ETCH_ARRAY_ELEMENT** ae)   
{
    return 0;
}


/**
 * tdi_end_array()
 * Ends an array that we are reading.
 * @param array the array that we read.
 * @throws IOException if there is a problem with the stream.
 */
int tdi_end_array(tagged_data_input* tdi, ETCH_ARRAYVALUE* x)
{
    return 0;
}


/**
 * new_tagged_data_input()
 * tagged_data_input constructor 
 */
tagged_data_input* new_tagged_data_input(const short objtype, const short class_id)
{
    tagged_data_input* tdi = NULL;
    i_tagged_data_input* vtab = NULL;
    const int VTAB_KEY = ETCHTYPE_VTABLE_TAGDATAINP;
    int result = 0;

    tdi = etch_malloc(sizeof(tagged_data_input), ETCHTYPEB_TAGDATAINP);
    memset(tdi, 0, sizeof(tagged_data_input));
    tdi->obj_type = objtype;
    tdi->class_id = class_id;
    /* note that instantiator must set tdi.impl */

    vtab = cache_find(VTAB_KEY);

    if(!vtab)  
    {          
        vtab = new_vtable(NULL, 
            sizeof(i_tagged_data_input), ETCHTYPEB_VTABLE, ETCHTYPE_TAGDATAINP);

        vtab->start_message = tdi_start_message;
        vtab->end_message   = tdi_end_message;
        vtab->start_struct  = tdi_start_struct;
        vtab->end_struct    = tdi_end_struct;
        vtab->start_array   = tdi_start_array;
        vtab->end_array     = tdi_end_array;
        vtab->read_array_element  = tdi_read_array_element;
        vtab->read_struct_element = tdi_read_struct_element;

        cache_add(VTAB_KEY, vtab);
    } 
 
    tdi->vtab = vtab;   
    return tdi; 
}


/**
 * destroy_boxed_tagged_data_input()
 * destructor of signature etchobj->destroy
 */
void destroy_boxed_tagged_data_input(ETCH_TAGDATAINPUT* tdiobj)
{
    tagged_data_input* tdi = tdiobj->value_ptr_to;
    i_etchobject* super = get_base_vtable((objmask*)tdiobj);
    ETCH_ASSERT(super);  /* parent vtable */
    /* TODO: possibly move impl* and impltype to etchobject, meaning we  
     * could always call destroy_instancedata, obviating the need to 
     * subclass objects simply because they have instance data.
     */   
    destroy_instancedata(tdi->impl, 0);  /* extract and invoke data dtor */

    etch_free(tdi);         /* destroy TDI object */

    super->destroy(tdiobj); /* call base class dtor to destroy object shell */

    /* tdiobj is now gone. note that if we did not want the base dtor to 
     * etch_free the tdiobj shell, we would set tdiobj->is_static_object first.
     */    
}


/**
 * new_boxed_tagged_data_input()
 * boxed tagged_data_input object constructor 
 */
ETCH_TAGDATAINPUT* new_boxed_tagged_data_input(const short objtype, const short class_id)
{
    etchobject* newobj = NULL;
    tagged_data_input* newtdi = NULL;
    i_etchobject* vtab = NULL;
    const int VTAB_KEY = ETCHTYPE_VTABLE_TDIOBJ;
    newtdi = new_tagged_data_input(objtype, class_id);
    if (NULL == newtdi) return NULL; 

    newobj = new_etchobject(ETCHTYPE_BOXED_TAGDATAINP);
    newobj->value_ptr_to = newtdi;
    newobj->size = sizeof(tagged_data_input);  

    vtab = cache_find(VTAB_KEY);   

    if(!vtab)  /* cache the vtable for this object type */
    {         
        vtab = new_vtable(newobj->vtab, ETCHOBJ_VTABSIZE, ETCHTYPEB_VTABLE, 0);       

        /* function overrides: TODO override clone() */
        vtab->destroy = destroy_boxed_tagged_data_input;    
                
        vtab->vtab = (i_etchobject*) newobj->vtab; /* chain parent vtable */ 

        cache_add(VTAB_KEY, vtab);  /* cache the new vtable */
    } 

    newobj->vtab = vtab; /* set override vtable */

    return newobj;
}


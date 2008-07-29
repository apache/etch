/*  
 * etch_structval.c -- etch_structvalue implementation.
 */

#include "etch_structval.h"
#include "etch_global.h"

/*  
 * private methods
 */
etch_struct_value* new_structvalue_init(const int, const int);


/*  
 * new_struct_element()
 * constructor for etch_struct_element
 */
etch_struct_element* new_struct_element(etch_field* field, etchobject* value)
{
    etch_struct_element* newelt 
        = etch_malloc(sizeof(etch_struct_element), ETCHTYPEB_STRUCTELEMENT);
    memset(newelt, 0, sizeof(etch_struct_element));

    if (field)
    {   /* field must already have a name and hash of name */
        if (!field->id || !field->name || !field->namebytelen) return NULL;

        memcpy(&newelt->key, field, sizeof(etch_field));
    }

    newelt->value = value;
    return newelt;
}


/*  
 * destroy_struct_element()
 * destructor for etch_struct_element
 */
int destroy_struct_element(etch_struct_element* thisp, const int is_free_value)
{
    etchobject* value = NULL;
    if (!thisp) return -1;
    value = thisp->value;

    if (is_free_value && value)  /* free element value */
    {   /* TODO validate as etchobject, return -1 */
        value->vtab->destroy(value);
    }

    etch_free(thisp);  /* free element shell */
    return 0;
}


/*  
 * new_boxed_struct_element()
 * constructor for boxed etch_struct_element
 */
ETCH_STRUCT_ELEMENT* new_boxed_struct_element(etch_field* field, etchobject* value)
{
    etchobject* newobj = NULL;

    etch_struct_element* newelt = new_struct_element(field, value);
    if (NULL == newelt) return NULL; 

    newobj = new_etchobject(ETCHTYPE_BOXED_STRUCTELEMENT);
    newobj->value_ptr_to = newelt;
    newobj->size = sizeof(etch_struct_element); /* TODO */
    return newobj;
}


/**
 * new_boxed_struct_value(type, initsize, readonly)
 * constructor for ETCH_STRUCTVALUE, a boxed etch_struct_value.
 * initsize: initial number of entries allocated, default configured.
 * is_readonly: if TRUE, memory for hashed values will not be freed
 * upon destroy() 
 * NOTE not currently using this, struct elements are etch_struct_element*
 */
ETCH_STRUCTVALUE* new_boxed_struct_value
    (etch_type* type, const int initsize, const int is_readonly) 
{
    etchobject* newobj = NULL;

    etch_struct_value* structval = new_structvalue(type, initsize, is_readonly);
    if (NULL == structval) return NULL; 

    newobj = new_etchobject(ETCHTYPE_BOXED_STRUCTVALUE);
    newobj->value_ptr_to = structval;
    newobj->size = sizeof(etch_struct_value); /* TODO */
    return newobj;
}


/**
 * new_structvalue()  
 * primary constructor for etch_struct_value. 
 */
etch_struct_value* new_structvalue
    (etch_type* type, const int initsize, const int is_readonly)
{
    etch_struct_value* newobj = new_structvalue_init(initsize, is_readonly);

    if (type) /* we make a copy of the specified type */
        memcpy(&newobj->type, type, sizeof(etch_type));

    return newobj;
}


/**
 * new_structvalue_init() (private)
 * common initialization on etch_struct_value construction.
 */
etch_struct_value* new_structvalue_init(const int initsize, const int is_readonly)
{
    etch_struct_value* newobj = etch_malloc(sizeof(etch_struct_value), ETCHTYPEB_STRUCTVAL);
    memset(newobj, 0, sizeof(etch_struct_value));
    newobj->is_readonly_element_values = is_readonly != 0; 

    /** the underlying hashtable is NOT marked as content type object, meaning 
     *  we do not interpret content as etchobject; rather content is always 
     *  an etch_struct_element*. we have marked the underlying hashtable
     *  to not free memory for value content when the hashtable is destroyed.
     *  TODO: this means we require a method to iterate the hashtable, free
     *  memory for the element value if requested, and finally free the 
     *  element shell, for each element in the hashtable.
     */
    newobj->items = new_hashtable(initsize); 
    newobj->items->is_readonly_values = TRUE;  
    newobj->items->content_type = 0; /* not ETCHHASHTABLE_CONTENT_OBJECT */

    return newobj;
}


/**
 * destroy_struct_value()
 * destructor for an etch_struct_value object
 */
int destroy_struct_value(etch_struct_value* thisp) 
{
    if (NULL == thisp) return -1;
        
    /* destroy the underlying hashtable */    
    destroy_hashtable(thisp->items, TRUE, !thisp->items->is_readonly_values);

    etch_free(thisp); /* destroy the object */
    return 0;
}

/**
 * solution for problem of hashing objects containing pointers to actual hashdata.
 * jenkins hfind takes a raw key, computes a hash, accesses the item with that
 * hash, and points at the pointer to the raw key stored in the table. our
 * analysis is based on id_name {int hashkey, int bytelen, wchar_t* name };
 * 1. if we hash with the name and length, we can then look up the name, but we
 * can't then get the pointer to the entire id_name struct to free it later.
 * 2. if we hash the entire struct, we can then get the struct back, free the
 * name, and then free the struct. however we need something good to hash on.
 * we should then first get a hashkey for the name, and store this in id_name's
 * hashkey. (we now no longer really need the bytelen part of id_name). we
 * then hash the entire 12 (or 8 without len) byte struct.
 * STILL TODO: determine if we should hash an id_name with a length of 4,
 * (just the name key), or a length of 12 (the entire struct).
 */


/**
 * put()
 * TODO: ensure that the hashtable is marked to NOT FREE KEYS,  
 * once we know how we are called and thus know this is the case.
 * TODO: verify caller can throw excp up the line on return of -1.
 * TODO: determine if we should hash an id_name with a length of 4,
 * (just the name key), or a length of 12 (the entire struct).
 * TODO: verify where the etch_field is created, and that it is in fact
 * destroyed up the line. alternatively, should the hashtable destroy the  
 * field wrapper?
 */
int structvalue_put(etch_struct_value* thisp, etch_struct_element* item)
{
    /* TODO: verify parameters */
    int result = 0;
    const int hashitem_length = sizeof(etch_field);
    etch_hashtable* hashtab = thisp->items;

    result = hashtab->vtab->insert(hashtab->realtable, 
        &item->key, hashitem_length, item->value, 0, 0, 0);

    return result == 0? 0: -1;
}


/**
 * Reads the struct from the tagged data input.
 * @param tdi the tagged data input to read from.
 * @return a struct value read from the tagged data input.
 * @throws IOException if there is a problem reading from the tagged data input.
 */
ETCH_STRUCTVALUE* structvalue_read(etch_struct_value* thisp, tagged_data_input* tdi)
{
    int result = 0;
    ETCH_STRUCTVALUE*  newobj = NULL;
    etch_struct_value* newstruct = NULL;
    if (!tdi) return NULL; /* TODO validate object */

    /* TODO set exception in object rather than returning NULL */
	if (NULL == (newobj = tdi->vtab->start_struct(tdi))) return NULL;
    newstruct = newobj->value_ptr_to;
    
    result = structvalue_read_keys_values(newstruct, tdi);
    if (result == -1) return NULL;     /* TODO exception */

    result = tdi->vtab->end_struct(tdi, newobj);
	return result == 0? newobj: NULL;  /* TODO exception */     
}


/**
 * Reads the key / value pairs and puts them in the struct.
 * @param tdi the tagged data input to read from.
 * @throws IOException if there are problems with the tagged data input.
 */
int structvalue_read_keys_values(etch_struct_value* thisp, tagged_data_input* tdi)
{
	etch_struct_element* se = new_struct_element(NULL, NULL);
    int result = 0; /* TODO validate params */

    while(TRUE == (result = tdi->vtab->read_struct_element(tdi, se)))
          structvalue_put(thisp, se);
    
    destroy_struct_element(se, FALSE);
    return result == -1? -1: 0;
}


/**
 * Writes a struct to the tagged data output.
 * @param tdo the tagged data output to write to.
 * @throws IOException if there is a problem with the tagged data output.
 */
int structvalue_write_struct(ETCH_STRUCTVALUE* thisp, tagged_data_output* tdo) 
{
    int result = 0;
    etch_struct_value* sv = NULL; 
    if (!thisp || !tdo) return -1; /* TODO validate object */ 
   
    sv = thisp->value_ptr_to;

	result = tdo->vtab->start_struct(tdo, sv);
    if (result == -1) return -1;

	result = structvalue_write_keys_values(sv, tdo);
    if (result == -1) return -1;

	result = tdo->vtab->end_struct(tdo, sv);
    return result;
}


/**
 * Writes the key / value pairs.
 * @param tdo the tagged data output to write to.
 * @throws IOException if there are problems with the tagged data output.
 * TODO: replace this detail first-next code with iterator
 * TODO: ensure excecption is thrown up the line
 */
int structvalue_write_keys_values(etch_struct_value* thisp, tagged_data_output* tdo)
{
    int i = 0, result = 0, realcount = 0, currcount = 0;
    etch_hashtable* hashtab = NULL;
    etch_hashitem  hashbucket;
    etch_hashitem* thisitem = &hashbucket;
    if (!thisp || !thisp->items || !tdo) return -1;
    hashtab   = thisp->items;
    realcount = hashtab->vtab->count(hashtab->realtable, 0, 0);
    if (realcount == 0) return 0;

    do 
    {   result = hashtab->vtab->first(hashtab->realtable, NULL, &thisitem);
        if (result == -1) break;   
        /* TODO code assumes we're using a field as a hashkey, verify this is so */         
        result = tdo->vtab->write_struct_element(tdo, (etch_field*)thisitem->key, thisitem->value);
        if (result == -1) break;
        currcount++;    

        while(1)
        {   result = hashtab->vtab->next(hashtab->realtable, NULL, &thisitem); 
            if (result == -1) break;
            result = tdo->vtab->write_struct_element(tdo, (etch_field*)thisitem->key, thisitem->value);
            if (result == -1) break;       
            currcount++;      
        }
    } while(0);
     
    return (result == -1 && currcount != realcount)? -1: 0;
}


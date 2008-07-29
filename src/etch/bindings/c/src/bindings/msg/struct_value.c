/*  
 * struct_value.c -- etch_structvalue implementation.
 */

#include "struct_value.h"


/**
   Public constructor for an etch_structvalue.
   type: is the type of the struct or message.
   initialsize: is the number of items the object can hold before it reallocates.
   Note that this value may be altered by the implementation, e.g. if it is out 
   of range or if it is not a power of 2. Pass zero to get the installation
   default size, which at this writing was 16 items.
*/
etch_structvalue* new_structvalue(etch_type* type, const int initialitemsize)
{
    etch_hashtable   *ht = new_hashtable(initialitemsize); 
    etch_structvalue *sv = ctor_etch_structvalue(ht);

    if (sv && type)  /* we make a copy of the specified type */
        memcpy(&sv->type, type, sizeof(etch_type));

    return sv; 
}


/**
   Private constructor for an etch_structvalue object.
*/
etch_structvalue* ctor_etch_structvalue(etch_hashtable* ht)
{
    etch_structvalue* sv = NULL;
    if (ht == NULL) return sv;

    sv = malloc(sizeof(etch_structvalue));
    memset(sv, 0, sizeof(etch_structvalue)); 
    sv->items = ht;

    return sv; 
}


/**
   Destructor for an etch_structvalue object.
*/
void destroy_etch_structvalue(etch_structvalue* this_p)
{
    etch_hashtable *ht = NULL;
    if (this_p == NULL) return;
    ht = this_p->items; if (ht == NULL) return;
    /* the dtor should probably destroy the hashtable from which it "inherits"
    */

    /* ht->destroy(ht->realtable, 0, 0); */  /* destroy the hashtable */

    free(this_p);  /* destroy the object */
}




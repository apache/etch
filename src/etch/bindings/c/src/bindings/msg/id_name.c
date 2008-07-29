/**
 * id_name.c
 * An id_name is a base class for Field or Type. It is used
 * to bind together a type or field name with the associated id. The id
 * is used for certain operations, such as the key in a Map, comparisons,
 * and binary encoding on the wire, while the name is used for display.
 */

#include "etch.h"
#include "id_name.h"
#include "etch_global.h"


/**
 * destroy_id_name_value
 * destructor for the value of a boxed etched_id_name. 
 * this is an etchobject virtual function invoked by etchobject->destroy()
 */
void destroy_id_name_value(ETCH_ID_NAME* thisp, etch_id_name* value, const int is_free_name) 
{     /* TODO do we check is_static_data here, in etchobject dtor, or both? */
   if (thisp && !thisp->is_static_data) /* unless flagged to not do so ... */
       id_name_destroy(value, is_free_name);  /* ... call native destructor */
}


/**
 * new_etch_id_name
 * constructor for boxed etch_id_name, accepting a constructed etch_id_name.
 */
ETCH_ID_NAME* new_etch_id_name(etch_id_name* value)
{
    ETCH_ID_NAME* newobj = new_etchobject(ETCHTYPE_BOXED_ID_NAME);
    if (NULL == newobj) return NULL;

    newobj->value_ptr_to = value;        /* can be NULL */
    newobj->size = sizeof(etch_id_name); /* TODO account the id_name content in size? */
    /* TODO if object.size for string is item length, then should size for an id_name 
     * be the size of the struct wrapper plus the size of the allocated name? 
     */

    /* override the default object vtable impl of destroy_value() */
    /* TODO ? ideally we would only have to override the vtable once? */
    newobj->vtab->destroy_value = destroy_id_name_value;

    return newobj;
}


/**
 * new_etch_id_name_2
 * constructor 2 for boxed etch_id_name, calls native ctor 1 to construct id_name content.
 * see comments at id_name_1()
 */
ETCH_ID_NAME* new_etch_id_name_2(const wchar_t* name, 
    const size_t bytelen, etch_id_name* existing)
{
    ETCH_ID_NAME* newobj = NULL;
    etch_id_name* idname = new_id_name_1(name, bytelen, existing);

    if (NULL != idname)  
        newobj = new_etch_id_name(idname);

    return newobj;
}


/**
 * new_etch_id_name_3
 * constructor 3 for boxed etch_id_name, calls native ctor 2 to construct id_name content.
 * see comments at id_name_2()
 */
ETCH_ID_NAME* new_etch_id_name_3(const unsigned int id, const wchar_t* name, 
    const size_t bytelen, etch_id_name* existing) 
{
    ETCH_ID_NAME* newobj = NULL;
    etch_id_name* idname = new_id_name_2(id, name, bytelen, existing);

    if (NULL != idname)  
        newobj = new_etch_id_name(idname);

    return newobj;
}


/**
 * id_name_2
 * Constructor accepting id and name
 * Allocates if necessary, initializes and returns an etch_id_name*
 * If you already have such an object that you need initialized, pass its
 * reference in the idn parameter. If instead you want to allocate the object,
 * and take responsibility for destroying it, leave the idn parameter null. 
 */
etch_id_name* new_id_name_2(const unsigned int id, const wchar_t* name, 
    const size_t bytelen, etch_id_name* idn) 
{
    if (!name || bytelen <= 0 || bytelen > (MAX_ETCHHASHKEY*sizeof(wchar_t))) 
         return NULL;
    if (!idn)  /* if object does not exist, allocate it */
         idn = etch_malloc(sizeof(etch_id_name), ETCHTYPEB_ID_NAME);
    idn->id   = id;
    idn->name = (wchar_t*)name; 
    idn->namebytelen = bytelen;
    return idn;
}


/**
 * id_name_1
 * Constructor accepting name. id is generated from a hash of the name.
 * Allocates if necessary, initializes and returns an etch_id_name*
 * If you already have such an object that you need initialized, pass its
 * reference in the idn parameter. If instead you want to allocate the object,
 * and take responsibility for destroying it, leave the idn parameter null. 
 */
etch_id_name* new_id_name_1(const wchar_t* name, const size_t bytelen, etch_id_name* idn) 
{
    if (!name || bytelen <= 0 || bytelen > (MAX_ETCHHASHKEY*sizeof(wchar_t))) 
         return NULL;
    if (!idn)  /* if object not passed, allocate it */
         idn = etch_malloc(sizeof(etch_id_name), ETCHTYPEB_ID_NAME);
    idn->name = (wchar_t*)name; 
    idn->namebytelen = bytelen;
    idn->id = id_name_hashfunc(name, bytelen);
    return idn;
}


/**
 * dtor_id_name
 * Destructor for an etch_id_name object.
 * Deallocates all memory allocated for the object and its contents.
 * Note that the name field is stored by reference but is freed only on request.
 */
void id_name_destroy(etch_id_name* thisp, const int is_free_name)
{
    if (thisp) 
    {
        if (is_free_name && thisp->name)
            etch_free(thisp->name);

        etch_free(thisp);
    }
}


/**
 * id_name_hashfunc 
 * Computes the hash value of the name to be used as the id during construction.
 * @param name the name of the type or field.
 * @return a hash of name in the unicode character encoding which is
 * very likely to be unique over reasonable name spaces. Collisions
 * should be very unlikely as they will force the user to change the name.      
 */
unsigned int id_name_hashfunc(const wchar_t* name, const size_t len)
{
   return etchhash(name, (int)len, 0);  
}


/**
 * id_name_hash   
 */
unsigned int id_name_hash(const etch_id_name* this_p)
{
   return this_p? id_name_hashfunc(this_p->name, this_p->namebytelen): 0;
}
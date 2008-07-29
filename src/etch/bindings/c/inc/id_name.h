/* 
 * id_name.h -- defines the etch_id_name object.
 * An etch_id_name is a base class for Field or Type. It is used to bind
 * together a type or field name with its associated id. The id is used
 * for certain operations, such as the key in a Map, comparisons, and
 * binary encoding on the wire, while the name is used for display.
*/

#ifndef ETCHIDNAME_H
#define ETCHIDNAME_H
#include "etch.h"
#include "etch_objects.h"

/**
 * struct etch_id_name
 * "object methods" are static and are defined in id_name.c
 * The "name" is stored by reference and is not freed when the etch_id_name
 * is destroyed. 
*/
struct etch_id_name
{
	unsigned int id;
    size_t   namebytelen;	/* byte length not including any terminator */
    wchar_t* name;   
};

typedef struct etch_id_name etch_id_name;  


/**
 * constructors for a boxed etch_type 
 */
ETCH_ID_NAME* new_etch_id_name(etch_id_name* value);
ETCH_ID_NAME* new_etch_id_name_2(const wchar_t* name, const size_t len, etch_id_name*);
ETCH_ID_NAME* new_etch_id_name_3(const unsigned int id, const wchar_t* name, const size_t len, etch_id_name*);


/**
 * destructor for a boxed etch_type 
 */
void destroy_id_name_value(ETCH_ID_NAME* thisp, etch_id_name* value, const int is_free_name);



etch_id_name* new_id_name_2(const unsigned int id, const wchar_t* name, const size_t len, etch_id_name*); 
etch_id_name* new_id_name_1(const wchar_t* name, const size_t len, etch_id_name*); 
void          id_name_destroy(etch_id_name* thisp, const int is_free_name);
unsigned int  id_name_hashfunc(const wchar_t* name, const size_t len);
unsigned int  id_name_hash(const etch_id_name* this_p);


#endif /* #ifndef ETCHIDNAME_H */ 
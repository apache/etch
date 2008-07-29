/* 
 * field.h -- defines the etch_field object.
 * An etch_field is an etch_id_name representing a field of a struct or message 
 * (i.e. a key for a value).
 */

#ifndef ETCHFIELD_H
#define ETCHFIELD_H
#include "etch.h"
#include "id_name.h"

/** 
 * etch_field
 * We're simply typef'ing this for now, but we've left it open to later define
 * an etch_field struct having an etch_id_name as a member.
 */
typedef etch_id_name etch_field;

/** 
 * for each etch_field method we substitute the corresponding etch_id_name method
 */
#define destroy_field_value  destroy_id_name_value
#define new_etch_field       new_etch_id_name
#define new_etch_field_2     new_etch_id_name_2
#define new_etch_field_3     new_etch_id_name_3

#define new_field_1    new_id_name_1  /* equate etch_field method names */
#define new_field_2    new_id_name_2  /* with their etch_id_name counterparts */
#define field_destroy  id_name_destroy
#define field_hashfunc id_name_hashfunc


#endif /* #ifndef ETCHFIELD_H */ 
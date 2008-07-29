/* 
 * type.h -- defines the etch_type object.
 * An etch_type is an etch_id_name representing a type of a struct or message 
 * (i.e. a key for a value).
 */

#ifndef ETCHTYPE_H
#define ETCHTYPE_H

#include "etch.h"
#include "id_name.h"
#include "etch_objects.h"

/** 
 * etch_type
 * We're simply typef'ing this for now, but we've left it open to later define
 * an etch_type struct having an etch_id_name as a member.
 */
typedef etch_id_name etch_type;

/** 
 * for each etch_type method we substitute the corresponding etch_id_name method
 */
#define destroy_type_value  destroy_id_name_value
#define new_etch_type       new_etch_id_name
#define new_etch_type_2     new_etch_id_name_2
#define new_etch_type_3     new_etch_id_name_3

#define new_type_1     new_id_name_1  /* equate etch_type method names */
#define new_type_2     new_id_name_2  /* with their etch_id_name counterparts */
#define type_destroy  id_name_destroy
#define type_hashfunc id_name_hashfunc


#endif /* #ifndef ETCHTYPE_H*/ 
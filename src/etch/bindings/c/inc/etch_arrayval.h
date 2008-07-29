/**
 * etch_arrayval.h 
 */

#ifndef ETCHARRAYVAL_H
#define ETCHARRAYVAL_H

#include "etchmsgutl.h"
#include "etch_tagdata_inp.h"
#include "etch_tagdata_out.h"
#include "etch_arraylist.h"


/**
 * etch_array_value
 * an array of values, where each value is of arbitrary type chosen from the 
 * basic java types boolean, byte, short, int, long, float, double, string, 
 * an array of those, the extended types array_valuse and struct_value, plus
 * specific types supported by value_factory 
 */
struct etch_array_value
{
    unsigned short obj_type; /* ID_ETCHARRAYVAL or whatever */
    unsigned short class_id;  

    etch_arraylist* list;
    int  dim;
    byte type_code;
    etch_type custom_struct_type;
    /* TODO the equals plumbing */
};

typedef struct etch_array_value etch_array_value;


/* public methods on etch_array_value */
etch_array_value* new_arrayvalue(const byte type_code, etch_type* custom_struct_type, 
    const int dim, const int initsize, const int deltsize, const int is_readonly);

etch_array_value* new_arrayvalue_default();

void destroy_arrayvalue(etch_array_value* thisp);
void destroy_boxed_arrayvalue(ETCH_ARRAYVALUE* thisp);

ETCH_ARRAY_ELEMENT* new_array_element(const int objtype);

int  arrayvalue_add (ETCH_ARRAYVALUE*  thisp, ETCH_ARRAY_ELEMENT* content);


int  arrayvalue_write(ETCH_ARRAYVALUE* thisp, tagged_data_output* tdi);
ETCH_ARRAYVALUE* arrayvalue_read(tagged_data_input* tdi);

ETCH_ARRAYVALUE* new_boxed_array_value
    (const byte type_code, etch_type* custom_struct_type, 
     const int dim, const int initsize, const int deltsize, const int is_readonly);

#endif /* #ifndef ETCHARRAYVAL_H */
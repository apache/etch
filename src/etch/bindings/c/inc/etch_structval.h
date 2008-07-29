/* 
 * struct_value.h -- defines the etch_structvalue object.
 * An etch_structvalue is a typed map of key/value pairs, where type is an etch_type,
 * a key is an etch_field, and a value is chosen from the basic types boolean, byte, 
 * short, int, long, float, double, String, an array of those, the extended types
 * ArrayValue and StructValue, and specific types supported by ValueFactory.
 */

#ifndef ETCHSTRUCTVAL_H
#define ETCHSTRUCTVAL_H

#include "etchmsgutl.h"
#include "etch_tagdata_inp.h"
#include "etch_tagdata_out.h"
#include "etchhash.h"


/**
 * etch_struct_value
 */
typedef struct etch_struct_value
{
    unsigned short obj_type; /* ID_ETCHSTRUCTVAL or whatever */
    unsigned short class_id; 

    etch_type type;

    etch_hashtable* items;
    byte is_readonly_element_values;

} etch_struct_value;



/* public methods */
etch_struct_value* new_structvalue
    (etch_type* type, const int initsize, const int is_readonly);


int destroy_struct_value(etch_struct_value* thisp);
int destroy_struct_element(etch_struct_element* thisp, const int is_free_value);

ETCH_STRUCTVALUE* new_boxed_struct_value
    (etch_type* type, const int initsize, const int is_readonly);

int structvalue_put(etch_struct_value* thisp, etch_struct_element* item);

/**
 * Reads the struct from the tagged data input.
 * @param tdi the tagged data input to read from.
 * @return a struct value read from the tagged data input.
 * @throws IOException if there is a problem reading from the tagged data input.
 */
ETCH_STRUCTVALUE* structvalue_read(etch_struct_value* thisp, tagged_data_input*);

/**
 * Writes a struct to the tagged data output.
 * @param tdo the tagged data output to write to.
 * @throws IOException if there is a problem with the tagged data output.
 */
int structvalue_write_struct(ETCH_STRUCTVALUE* thisp, tagged_data_output*);

/**
 * Reads the key / value pairs and puts them in the struct.
 * @param tdi the tagged data input to read from.
 * @throws IOException if there are problems with the tagged data input.
 */
int structvalue_read_keys_values(etch_struct_value* thisp, tagged_data_input*);

/**
 * Writes the key / value pairs.
 * @param tdo the tagged data output to write to.
 * @throws IOException if there are problems with the tagged data output.
 */
int structvalue_write_keys_values(etch_struct_value* thisp, tagged_data_output*);



#endif /* #ifndef ETCHSTRUCTVAL_H */ 
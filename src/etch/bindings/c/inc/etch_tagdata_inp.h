/*
 * etch_tagdata_inp.h
 * tagged data input
 */

#ifndef ETCH_TAGDATA_INP_H
#define ETCH_TAGDATA_INP_H

#include "etchmsgutl.h"

struct etch_message;
struct etch_struct_value;
struct i_tagged_data_input;


/**
 * tagged_data_input
 * tagged data input implementation
 */
typedef struct tagged_data_input
{
    unsigned short obj_type; 
    unsigned short class_id;  
    struct i_tagged_data_input* vtab;

    void* impl;

} tagged_data_input;
 


/**
 * i_tagged_data_input
 * virtual function table for tagged data input
 */
struct i_tagged_data_input
{
    unsigned short obj_type; 
    unsigned short class_id;  
    struct i_tagged_data_input* vtab;

	/**
	 * Starts reading a message from the stream.
	 * @return the message that we are reading.
	 * @throws IOException if there is a problem with the stream.
	 */
	ETCH_MESSAGE* (*start_message) (tagged_data_input*);  

	/**
	 * Ends a message that we are reading.
	 * @param msg the message that has been read.
	 * @throws IOException if there is a problem with the stream.
	 */
	int (*end_message) (tagged_data_input*, ETCH_MESSAGE*);

	/**
	 * Starts reading a struct from the stream.
	 * @return the struct that we are reading.
	 * @throws IOException if there is a problem with the stream.
	 */
	ETCH_STRUCTVALUE* (*start_struct) (tagged_data_input*);

	/**
	 * Reads the next key and value in the sequence of struct
	 * elements by stuffing the fields of the passed in StructElement.
	 * @param se place to store the read key and value.
	 * @return true if a key and value were read, false if there aren't any more
	 * key / value pairs.
	 * @throws IOException if there is a problem with the stream.
	 */
	 int (*read_struct_element) (tagged_data_input*, struct etch_struct_element*);

	/**
	 * Ends a struct that we are reading.
	 * @param struct the struct that we read.
	 * @throws IOException if there is a problem with the stream.
	 */
	int (*end_struct) (tagged_data_input*, ETCH_STRUCTVALUE*);

	/**
	 * Starts reading an array from the stream.
	 * @return the array that we are reading.
	 * @throws IOException if there is a problem with the stream.
	 */
	ETCH_ARRAYVALUE* (*start_array) (tagged_data_input*);

	/**
	 * Reads the next value in the sequence of array elements by
	 * stuffing the fields of the passed in ArrayElement.
	 * @param ae place to store the read value.
	 * @return true if a value was read, false if there aren't any more.
	 * @throws IOException if there is a problem with the stream.
	 */
	int (*read_array_element) (tagged_data_input*, ETCH_ARRAY_ELEMENT**);

	/**
	 * Ends an array that we are reading.
	 * @param array the array that we read.
	 * @throws IOException if there is a problem with the stream.
	 */
	int (*end_array) (tagged_data_input*, ETCH_ARRAYVALUE*);
};

typedef struct i_tagged_data_input i_tagged_data_input;


/**
 * constructors
 */
tagged_data_input* new_tagged_data_input();
ETCH_TAGDATAINPUT* new_boxed_tagged_data_input();

#endif /* #ifndef ETCH_TAGDATA_INP_H */
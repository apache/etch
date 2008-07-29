/*
 * etch_tagdata_out.h
 * tagged data output
 */

#ifndef ETCH_TAGDATA_OUT_H
#define ETCH_TAGDATA_OUT_H

#include "etchmsgutl.h"

struct etch_message;
struct etch_struct_value;
struct i_tagged_data_output;


/**
 * tagged_data_output
 * tagged data output implementation
 */
typedef struct tagged_data_output
{
    unsigned short obj_type; 
    unsigned short class_id;  
    struct i_tagged_data_output* vtab;

    void* impl;

} tagged_data_output;

 

/**
 * i_tagged_data_output
 */
struct i_tagged_data_output
{
    unsigned short obj_type; 
    unsigned short class_id;  
    struct i_tagged_data_output* vtab;

	/**
	 * Starts reading a message from the stream.
	 * @return the message that we are reading.
	 * @throws IOException if there is a problem with the stream.
	 */
	ETCH_MESSAGE* (*start_message) (tagged_data_output*);  

	/**
	 * Ends a message that we are reading.
	 * @param msg the message that has been read.
	 * @throws IOException if there is a problem with the stream.
	 */
	int (*end_message) (tagged_data_output*, ETCH_MESSAGE*);

	/**
	 * Writes the beginning of struct data. The type of the struct is
	 * written. The fields and values of the struct are written
	 * using {@link #writeStructElement(Field, Object)}.
	 * @param struct the struct being written out.
	 * @throws IOException if there is a problem with the stream.
	 * @see #endStruct(StructValue)
	 */
	int (*start_struct) (tagged_data_output*, struct etch_struct_value*);

	/**
	 * Writes a struct element with the specified key and value.
	 * @param key
	 * @param value
	 * @throws IOException if there is a problem with the stream.
	 */
	int (*write_struct_element) (tagged_data_output*, etch_field*, etchobject*);

	/**
	 * Ends a struct that we are reading.
	 * @param struct the struct that we read.
	 * @throws IOException if there is a problem with the stream.
	 */
	int (*end_struct) (tagged_data_output*, struct etch_struct_value*);

	/**
	 * Starts reading an array from the stream.
	 * @return the array that we are reading.
	 * @throws IOException if there is a problem with the stream.
	 */
	ETCH_ARRAYVALUE* (*start_array) (tagged_data_output*, ETCH_ARRAYVALUE*);

	/**
	 * Writes an array element with the specified value.
	 * @param value
	 * @throws IOException if there is a problem with the stream.
	 */
	int (*write_array_element) (tagged_data_output*, etchobject*);

	/**
	 * Ends an array that we are reading.
	 * @param array the array that we read.
	 * @throws IOException if there is a problem with the stream.
	 */
	int (*end_array) (tagged_data_output*, ETCH_ARRAYVALUE*);
};

typedef struct i_tagged_data_output i_tagged_data_output;



/**
 * constructors
 */
tagged_data_output* new_tagged_data_output();
ETCH_TAGDATAOUTPUT* new_boxed_tagged_data_output();



#endif /* #ifndef ETCH_TAGDATA_OUT_H */
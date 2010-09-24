/* $Id$ 
 * 
 * Licensed to the Apache Software Foundation (ASF) under one or more 
 * contributor license agreements. See the NOTICE file distributed with  
 * this work for additional information regarding copyright ownership. 
 * The ASF licenses this file to you under the Apache License, Version  
 * 2.0 (the "License"); you may not use this file except in compliance  
 * with the License. You may obtain a copy of the License at 
 * 
 * http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and 
 * limitations under the License. 
 */ 

/*
 * etch_tagdata_inp.h
 * tagged data input base class
 */

#ifndef ETCH_TAGDATA_INP_H
#define ETCH_TAGDATA_INP_H

#include "etch_msgutl.h"
#include "etch_tagged_data.h"

#ifdef __cplusplus
extern "C" {
#endif

struct etch_message; 
struct etch_structvalue;
struct etch_arrayvalue;
struct etch_flexbuffer;

/**
 * tagged_data_input
 * tagged data input implementation
 */
typedef struct tagged_data_input
{
    etch_object object;

    etch_object* impl;

} tagged_data_input;


/**
 * i_tagged_data_input virtual function signatures
 */
typedef struct etch_message* (*etchtdi_start_message)(tagged_data_input*);  
typedef struct etch_message* (*etchtdi_read_message) (tagged_data_input*, struct etch_flexbuffer*);  
typedef int (*etchtdi_end_message) (tagged_data_input*, struct etch_message*); 
 
typedef struct etch_structvalue*(*etchtdi_start_struct) (tagged_data_input*); 
typedef struct etch_structvalue*(*etchtdi_read_struct)  (tagged_data_input*); 
typedef int (*etchtdi_read_struct_element)       (tagged_data_input*, struct etch_struct_element*);  
typedef int (*etchtdi_end_struct) (tagged_data_input*, struct etch_structvalue*); 

typedef struct etch_arrayvalue* (*etchtdi_start_array)(tagged_data_input*);
typedef struct etch_arrayvalue* (*etchtdi_read_array) (tagged_data_input*, struct etch_validator*);
typedef int (*etchtdi_read_array_element)      (tagged_data_input*, ETCH_ARRAY_ELEMENT**);
typedef int (*etchtdi_end_array)(tagged_data_input*, struct etch_arrayvalue*);


/**
 * i_tagged_data_input
 * virtual function table for tagged data input
 */
struct i_tagged_data_input
{
    etch_object object;

    etchparentinfo* inherits_from; 

	etchtdi_start_message start_message;
    etchtdi_read_message  read_message;
	etchtdi_end_message   end_message;
	etchtdi_start_struct  start_struct;
    etchtdi_read_struct   read_struct;
	etchtdi_end_struct    end_struct;
	etchtdi_start_array   start_array;
    etchtdi_read_array    read_array;
	etchtdi_end_array     end_array;

    tagdata_get_native_type_code  get_native_type_code;
    tagdata_get_custom_structtype get_custom_structtype;
    tagdata_check_value check_value;
};

typedef struct i_tagged_data_input i_tagged_data_input;


tagged_data_input* new_tagged_data_input();

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCH_TAGDATA_INP_H */

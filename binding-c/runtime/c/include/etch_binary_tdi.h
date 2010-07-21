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
 * etch_binary_tdi.h
 * binary tagged data input
 */

#ifndef ETCH_BINARY_TDI_H
#define ETCH_BINARY_TDI_H

#include "etch_tagged_data.h"
#include "etch_tagdata_inp.h"
#include "etch_flexbuffer.h"
#include "etch_value_factory.h"

#if(0)

 BINARY_TAGGED_DATA_INPUT
 value_factory vf;
 ctor(valuefactory);

     BINARY_TAGGED_DATA
     bool  (*check_value)(object value, validator v);
     byte  (*adjust_small_value)(byte type, object value);   
     byte  (*check_value)(object value);
     byte  (*get_native_type_code)(class*);
     byte  (*get_custom_struct_type)(class*);
     class*(*get_native_type)(byte type);
     byte  (*check_byte)  (byte  v, byte is1, byte dt);
     byte  (*check_short) (short v, byte is2, byte dt);
     byte  (*check_int)   (int   v, byte is4, byte dt);
     byte  (*check_long)  (long  v, byte is8, byte dt);

        TAGGED_DATA
        array_value* (*to_array_value) (object value);
        object* (*from_array_value) (object value);
        object* (*alloc_array_value)(byte type, Type custtructtype, int dim, int len);
        class*  (*get_component_type)(byte type, Type custtructtype, int dim);
        byte    (*check_value) (object value);
        byte    (*get_native_typecode) (class* c);
        byte    (*get_custom_struct_type)(class*);
        class*  (*get_native_type)(byte type);

    TAGGED_DATA_INPUT
    etch_message* (*start_message) (tagged_data_input*);  
    etch_message* (*end_message) (tagged_data_input*, etch_message*);
    etch_structvalue* (*start_struct) (tagged_data_input*);
    int (*read_struct_element) (tagged_data_input*, struct etch_struct_element*);
    int (*end_struct) (tagged_data_input*, etch_structvalue*);
    etch_arrayvalue* (*start_array) (tagged_data_input*);
    int (*read_array_element) (tagged_data_input*, ETCH_ARRAY_ELEMENT**);
    int (*end_array) (tagged_data_input*, etch_arrayvalue*);

#endif


/** 
 *  i_binary_tdi: vtable for binary tagged data input interface
 */
struct i_binary_tdi
{
    etch_object object;

    etchparentinfo* inherits_from;

    /* i_tagdata_input */
	etchtdi_start_message         start_message;
    etchtdi_read_message          read_message;
	etchtdi_end_message           end_message;
	etchtdi_start_struct          start_struct;
    etchtdi_read_struct           read_struct;
    etchtdi_end_struct            end_struct;
    etchtdi_start_array           start_array;
    etchtdi_read_array            read_array;
    etchtdi_end_array             end_array;

    tagdata_get_native_type_code  get_native_type_code;
    tagdata_get_native_type       get_native_type;
    tagdata_get_custom_structtype get_custom_structtype;
    tagdata_check_value check_value;	
};

typedef struct i_binary_tdi i_binary_tdi;


/**
 * binary_tagged_data_input
 * binary tagged data input implementation
 */
typedef struct binary_tagged_data_input
{
    etch_object object;

    etch_object* impl;  /* must appear first to mask tagged_data_input */

    etch_value_factory*  vf;     /* not owned */
    etch_flexbuffer* flexbuf;    /* not owned */
    etch_validator*  vtor_int;   /* not owned */
    etch_object* static_nullobj;         /* owned */
    etch_object* static_eodmarker;       /* owned */
    etch_string* static_emptystring; /* owned */

} binary_tagged_data_input;


 binary_tagged_data_input* new_binary_tagdata_input(etch_value_factory*);
 tagged_data_input*        new_binary_tdi(etch_value_factory*);

struct etch_message* bintdi_read_message(tagged_data_input* tdix, 
    etch_flexbuffer* fbuf);

struct etch_message* bintdi_read_message_fromf(etch_value_factory* vf, 
    etch_flexbuffer* fbuf);

struct etch_message* bintdi_read_message_from(etch_value_factory* vf, byte* buf, 
    const int bufsize);

struct etch_message* bintdi_read_message_fromo(etch_value_factory* vf, byte* buf, 
    const int bufsize, const int msglen, const int offset);


#endif /* #ifndef ETCH_BINARY_TDI_H */

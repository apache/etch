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
 * etch_binary_tdo.h
 * binary tagged data output
 */

#ifndef ETCH_BINARY_TDO_H
#define ETCH_BINARY_TDO_H

#include "etch_tagdata_out.h"
#include "etch_flexbuffer.h"
#include "etch_message.h"
#include "etch_value_factory.h"

#ifdef __cplusplus
extern "C" {
#endif

struct etch_arrayvalue;

/** 
 *  i_binary_tdo: vtable for binary tagged data output interface
 */
typedef struct i_binary_tdo
{
    etch_object object;

    etchparentinfo* inherits_from; 

    /* i_tagdata_output */
	etchtdo_start_message         start_message;
    etchtdo_write_message         write_message;
	etchtdo_end_message           end_message;
	etchtdo_start_struct          start_struct;
    etchtdo_write_struct          write_struct;
    etchtdo_end_struct            end_struct;
    etchtdo_start_array           start_array;
    etchtdo_write_array           write_array;
    etchtdo_end_array             end_array;

    tagdata_get_native_type_code  get_native_type_code;
    tagdata_get_native_type       get_native_type;
    tagdata_get_custom_structtype get_custom_structtype;
    tagdata_check_value check_value;	
} i_binary_tdo;

/**
 * binary_tagged_data_output
 * binary tagged data input implementation
 */
typedef struct binary_tagged_data_output
{
    etch_object object;

    etch_object* impl;  /* must appear first to mask tagged_data_output */

    etch_value_factory*  vf;      /* not owned */
    etch_flexbuffer* flexbuf; /* usually owned */
    unsigned char is_flexbuf_owned;

    etch_validator*  vtor_int;    /* not owned */
    etch_validator*  vtor_eod;    /* not owned */
    etch_object* static_nullobj;          /* owned */
    etch_object* static_eodmarker;        /* owned */
    etch_string* static_emptystring;  /* owned */

} binary_tagged_data_output;

binary_tagged_data_output* new_binary_tagdata_output(etch_value_factory*, etch_flexbuffer*); 
tagged_data_output*        new_binary_tdo(etch_value_factory*);

int bintdo_get_bytes (etch_message*, etch_value_factory*, byte** out);
int bintdo_write_value (binary_tagged_data_output*, etch_validator*, etch_object*);
struct etch_arrayvalue* bintdo_to_arrayvalue (binary_tagged_data_output* tdo, struct etch_nativearray*);
struct etch_arrayvalue* normalize_etch_array(void* a, const int maxdim);

#ifdef __cplusplus
} //extern "C"
#endif

#endif /* #ifndef ETCH_BINARY_TDO_H */

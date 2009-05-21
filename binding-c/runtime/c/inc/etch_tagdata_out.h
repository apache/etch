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
 * etch_tagdata_out.h
 * tagged data output
 */

#ifndef ETCH_TAGDATA_OUT_H
#define ETCH_TAGDATA_OUT_H

#include "etchmsgutl.h"
#include "etch_tagdata.h"

struct etch_message; 
typedef struct etch_message etch_message;
struct etch_structvalue;
typedef struct etch_structvalue etch_structvalue;
struct etch_arrayvalue;
typedef struct etch_arrayvalue etch_arrayvalue;
struct etch_flexbuffer;
typedef struct etch_flexbuffer etch_flexbuffer;


/**
 * tagged_data_output
 * tagged data output implementation
 */
struct tagged_data_output
{
    unsigned int    hashkey;    
    unsigned short  obj_type;   
    unsigned short  class_id;   
    struct i_tagged_data_output* vtab;       
    int  (*destroy)(void*);     
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;    
    struct objmask* parent;     
    etchresult*     result;     
    unsigned int    refcount;       
    unsigned int    length;     
    unsigned char   is_null;   
    unsigned char   is_copy;   
    unsigned char   is_static;  
    unsigned char   reserved;

    objmask* impl;
};

typedef struct tagged_data_output tagged_data_output;


/**
 * i_tagged_data_output virtual function signatures
 */
typedef int (*etchtdo_start_message)(tagged_data_output*, etch_message*);  
typedef int (*etchtdo_write_message)(tagged_data_output*, etch_message*, etch_flexbuffer*);  
typedef int (*etchtdo_end_message)  (tagged_data_output*, etch_message*); 
 
typedef int (*etchtdo_start_struct) (tagged_data_output*, etch_structvalue*);  
typedef int (*etchtdo_write_struct) (tagged_data_output*, etch_structvalue*); 
typedef int (*etchtdo_end_struct)   (tagged_data_output*, etch_structvalue*); 

typedef int (*etchtdo_start_array)  (tagged_data_output*, etch_arrayvalue*);
typedef int (*etchtdo_write_array)  (tagged_data_output*, etch_arrayvalue*, etch_validator*); 
typedef int (*etchtdo_end_array)    (tagged_data_output*, etch_arrayvalue*);


/**
 * i_tagged_data_output
 */
struct i_tagged_data_output
{
    unsigned int    hashkey;    
    unsigned short  obj_type;   
    unsigned short  class_id;   
    struct i_tagged_data_output* vtab;       
    int  (*destroy)(void*);     
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;    
    struct objmask* parent;     
    etchresult*     result;     
    unsigned int    refcount;       
    unsigned int    length;     
    unsigned char   is_null;   
    unsigned char   is_copy;   
    unsigned char   is_static;  
    unsigned char   reserved;

    etchparentinfo* inherits_from; 

	etchtdo_start_message   start_message;
    etchtdo_write_message   write_message;
	etchtdo_end_message     end_message;
	etchtdo_start_struct    start_struct;
    etchtdo_write_struct    write_struct;
	etchtdo_end_struct      end_struct;
	etchtdo_start_array     start_array;
    etchtdo_write_array     write_array;
	etchtdo_end_array       end_array;
};

typedef struct i_tagged_data_output i_tagged_data_output;

tagged_data_output* new_tagged_data_output();


#endif /* #ifndef ETCH_TAGDATA_OUT_H */
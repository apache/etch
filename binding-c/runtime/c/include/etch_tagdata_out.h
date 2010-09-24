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
 * tagged_data_output
 * tagged data output implementation
 */
typedef struct tagged_data_output
{
    etch_object object;

    etch_object* impl;
} tagged_data_output;

/**
 * i_tagged_data_output virtual function signatures
 */
typedef int (*etchtdo_start_message)(tagged_data_output*, struct etch_message*);  
typedef int (*etchtdo_write_message)(tagged_data_output*, struct etch_message*, struct etch_flexbuffer*);  
typedef int (*etchtdo_end_message)  (tagged_data_output*, struct etch_message*); 
 
typedef int (*etchtdo_start_struct) (tagged_data_output*, struct etch_structvalue*);  
typedef int (*etchtdo_write_struct) (tagged_data_output*, struct etch_structvalue*); 
typedef int (*etchtdo_end_struct)   (tagged_data_output*, struct etch_structvalue*); 

typedef int (*etchtdo_start_array)  (tagged_data_output*, struct etch_arrayvalue*);
typedef int (*etchtdo_write_array)  (tagged_data_output*, struct etch_arrayvalue*, struct etch_validator*); 
typedef int (*etchtdo_end_array)    (tagged_data_output*, struct etch_arrayvalue*);


/**
 * i_tagged_data_output
 */
struct i_tagged_data_output
{
    etch_object object;

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

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCH_TAGDATA_OUT_H */

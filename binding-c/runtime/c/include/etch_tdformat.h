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
 * etch_tdformat.h
 * a factory used to construct tagged data processors for messagizer
 */

#ifndef ETCHTDFORMAT_H
#define ETCHTDFORMAT_H

#include "etch_tagdata_inp.h"
#include "etch_tagdata_out.h"
#include "etch_binary_tdi.h"
#include "etch_binary_tdo.h"
#include "etch_value_factory.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 
 *  tagdata_format_factory
 */
typedef struct tagdata_format_factory
{
    etch_object object;

    tagged_data_input*  (*new_tagdata_input)  (etch_value_factory*);
    tagged_data_output* (*new_tagdata_output) (etch_value_factory*);

} tagdata_format_factory;

typedef enum etchtdf  /* tagged data format IDs */
{   ETCH_TAGDATA_FORMAT_BINARY, 
    ETCH_TAGDATA_FORMAT_XML,
    ETCH_TAGDATA_FORMAT_COUNT  /* must be last in list */
} etchtdf;

const wchar_t* tdformat_names[ETCH_TAGDATA_FORMAT_COUNT];

const wchar_t* tdfname_binary;
const wchar_t* tdfname_xml;

int is_tdf_initialized;

tagdata_format_factory* get_format_factory(wchar_t* format_name);
int  etchtdf_name_to_id(wchar_t* format_name);

#ifdef __cplusplus
}
#endif

#endif  /* ETCHTDFORMAT_H */

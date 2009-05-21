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
 * perf_valufact.h
 * perf implementation of value factory
 */

#ifndef PERF_VALUFACT_H
#define PERF_VALUFACT_H

#include "etch_defvalufact.h"
typedef default_value_factory perf_value_factory;
typedef struct perfvf_statics perfvf_statics;

unsigned short CLASSID_PERF_VALUFACT_IMPL;
unsigned short CLASSID_SERIALIZER_PERF_POINT;


/**
 * perfvf_statics
 */
typedef struct perfvf_statics
{
    etch_type* _mt_etch_xmpl_perfserver_add;
	etch_type* _mt_etch_xmpl_perfclient_result_add; 
	etch_type* _mt_etch_xmpl_perfserver_sum;
	etch_type* _mt_etch_xmpl_perfclient_result_sum;
    etch_type* _mt_etch_xmpl_perfserver_report;
	etch_type* _mt_etch_xmpl_perfclient_result_report; 
	etch_type* _mt_etch_xmpl_perfserver_dist;
	etch_type* _mt_etch_xmpl_perfclient_result_dist;
    etch_type* _mt_etch_xmpl_perfserver_add2;
	etch_type* _mt_etch_xmpl_perfclient_result_add2; 
    etch_type* _mt_etch_xmpl_perfserver_report2;
	etch_type* _mt_etch_xmpl_perfclient_result_report2; 
	etch_type* _mt_etch_xmpl_perf_point;
	
	etch_field* _mf_x;
	etch_field* _mf_y;
    etch_field* _mf_values;
	etch_field* _mf_code;
	etch_field* _mf_a;
	etch_field* _mf_b;
	etch_field* _mf_ts;
	etch_field* _mf_ms;

    etch_serializer* serializer_point;   

    wchar_t* str_etch_perf_x;
    wchar_t* str_etch_perf_y;
    wchar_t* str_etch_perf_values;
    wchar_t* str_etch_perf_code;
    wchar_t* str_etch_perf_a;
    wchar_t* str_etch_perf_b;
    wchar_t* str_etch_perf_ts;
    wchar_t* str_etch_perf_ms;

    wchar_t* str_etch_perf_srv_add;
    wchar_t* str_etch_perf_cli_result_add;
    wchar_t* str_etch_perf_srv_sum;
    wchar_t* str_etch_perf_cli_result_sum;
    wchar_t* str_etch_perf_srv_report;
    wchar_t* str_etch_perf_srv_dist;
    wchar_t* str_etch_perf_cli_result_dist;
    wchar_t* str_etch_perf_srv_add2;
    wchar_t* str_etch_perf_cli_result_add2;
    wchar_t* str_etch_perf_srv_report2;
    wchar_t* str_etch_perf_point;

} perfvf_statics;


/* todo make this a true singleton */
perfvf_statics* _g_perf_statics;


/**
 * perf_value_factory_impl
 * perf extension of default value factory
 */
typedef struct perf_value_factory_impl
{
    unsigned int    hashkey;    
    unsigned short  obj_type;   
    unsigned short  class_id;   
    struct i_value_factory* vtab;       
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

    perfvf_statics  statics;
	
} perf_value_factory_impl;


perf_value_factory* new_perf_value_factory ();


#endif /* #ifndef PERF_VALUFACT_H */
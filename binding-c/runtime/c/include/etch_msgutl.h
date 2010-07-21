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
 * etchmsgutl.h -- includes common to the etch message components
 */

#ifndef ETCHMSGUTL_H
#define ETCHMSGUTL_H

#include "etch_object.h"
#include "etch_type.h"
#include "etch_field.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCH_ARRAYVALUE_DEFAULT_INITSIZE 32
#define ETCH_ARRAYVALUE_DEFAULT_DELTSIZE 0
#define ETCH_ARRAYVALUE_DEFAULT_READONLY TRUE
#define ETCH_ARRAYVALUE_DEFAULT_SYNCHRONIZED FALSE
#define ETCH_ARRAYVALUE_DEFAULT_CONTENT_TYPE ETCHARRAYLIST_CONTENT_OBJECT

#define ETCH_STRUCT_CONTENT_TYPE_OBJ 1
#define ETCH_STRUCT_DEFAULT_INITSIZE 64
#define ETCH_STRUCT_DEFAULT_READONLY_KEY TRUE
#define ETCH_STRUCT_DEFAULT_READONLY_VAL TRUE
#define ETCH_STRUCT_DEFAULT_TRACKED_MEM  TRUE
#define ETCH_STRUCT_DEFAULT_CONTENT_TYPE ETCH_STRUCT_CONTENT_TYPE_OBJ

/**
 * equate java array element to an object.
 * we do so to facilitate porting of the java code.
 */
typedef etch_object ETCH_ARRAY_ELEMENT;


/**
 * etch_struct_element
 */
typedef struct etch_struct_element
{
  etch_field* key;
  etch_object*    value;
} etch_struct_element;


etch_struct_element* new_struct_element(etch_field*, etch_object*, etch_struct_element*);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHMSGUTL_H */

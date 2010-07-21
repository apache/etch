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
 * etch_field.h -- defines the etch_field object.
 * An etch_field is an etch_id_name representing a field of a struct or message 
 * (i.e. a key for a value).
 */

#ifndef ETCHFIELD_H
#define ETCHFIELD_H

#include "etch_id_name.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * etch_field
 * We're simply typef'ing this for now, but we've left it open to later define
 * an etch_field struct having an etch_id_name as a member.
 */
typedef etch_id_name etch_field;

etch_field* new_field(const wchar_t* name);
etch_field* new_static_field(const wchar_t* name);
int destroy_static_field(etch_field*);

#define clone_field      clone_id_name
#define destroy_field    destroy_id_name
#define is_good_field    is_good_id_name
#define is_equal_fields  is_equal_id_names
#define compute_field_id compute_id_name_id
#define compute_field_id_from_widename compute_id_name_id_from_widename

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHFIELD_H */ 

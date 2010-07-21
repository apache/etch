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
 * etch_id_name.h 
 * etch_id_name is base class for etch_field and etch_type. it binds
 * together a type or field name with its associated id. The id is used
 * for certain operations, such as the key in a map, comparisons, and
 * binary encoding on the wire.  
*/

#ifndef _ETCH_ID_NAME_MAP_H_
#define _ETCH_ID_NAME_MAP_H_

#include "etch.h"
#include "etch_object.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct etch_id_name_map etch_id_name_map;

/**
 * creates a new instance of etch_id_name_map
 */
etch_status_t etch_id_name_map_create(etch_id_name_map** map);

/**
 * gets the IdName data which corresponds to the specified id.
 */
etch_status_t etch_id_name_map_get_by_id(etch_id_name_map* map, int32 id, void** data);

/**
 * gets the IdName data which corresponds to the specified id.
 */
etch_status_t etch_id_name_map_get_by_name(etch_id_name_map* map, const wchar_t* name, void** data);

/**
 * adds the id name data to the map.
 */
etch_status_t etch_id_name_map_add(etch_id_name_map* map, int32 id, const wchar_t* name, void* data);

/**
 * get count of the map.
 */
uint32 etch_id_name_map_count(etch_id_name_map* map);

// get values function

/**
 * destroy the etch_id_name_map instance
 */
etch_status_t etch_id_name_map_destroy(etch_id_name_map* map);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ETCH_ID_NAME_MAP_H_ */

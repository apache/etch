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

#include "etch_id_name_map.h"
#include "etch_hash.h"

struct etch_id_name_map
{
    etch_hashtable* ids;
    etch_hashtable* names;
};

etch_status_t etch_id_name_map_create(etch_id_name_map** map)
{
    return ETCH_ENOTIMPL;
}

etch_status_t etch_id_name_map_get_by_id(etch_id_name_map* map, int32 id, void** data)
{
    return ETCH_ENOTIMPL;
}

etch_status_t etch_id_name_map_get_by_name(etch_id_name_map* map, const wchar_t* name, void** data)
{
    return ETCH_ENOTIMPL;
}

etch_status_t etch_id_name_map_add(etch_id_name_map* map, int32 id, const wchar_t* name, void* data)
{
        return ETCH_ENOTIMPL;
}

uint32 etch_id_name_map_count(etch_id_name_map* map)
{
        return ETCH_ENOTIMPL;
}

etch_status_t etch_id_name_map_destroy(etch_id_name_map* map)
{
        return ETCH_ENOTIMPL;
}


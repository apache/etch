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
 * etch_object_types.h -- constants for internal object types.
 */

#include "etch.h"
#include "etch_objecttypes.h"

static const char* objtype_b_names[] = {
    "ETCHTYPEB_UNDEFINED",  // 0x00 also "ETCHTYPEB_NONE"
    "ETCHTYPEB_BYTE",       // 0x01
    "ETCHTYPEB_BOOL",       // 0x02
    "ETCHTYPEB_INT8",       // 0x03
    "ETCHTYPEB_INT16",      // 0x04
    "ETCHTYPEB_INT32",      // 0x05
    "ETCHTYPEB_INT64",      // 0x06
    "ETCHTYPEB_IEEE32",     // 0x07
};

const char* etch_object_type_get_name(objtype_b type)
{
    return objtype_b_names[type];
}
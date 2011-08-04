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

#ifndef __ETCHOBJECT_H__
#define __ETCHOBJECT_H__

#include "EtchConfig.h"

enum EtchObjectTypeIds {
    EOTID_INT32     = 0,
    EOTID_BOOL,
    EOTID_BYTE,
    EOTID_SHORT,
    EOTID_LONG,
    EOTID_DOUBLE,
    EOTID_FLOAT,
    EOTID_STRING,
    EOTID_DATE,
    EOTID_LIST,
    EOTID_NATIVE_ARRAY,
      
    EOTID_NATIVE_INT8,
    EOTID_NATIVE_INT16,
    EOTID_NATIVE_INT32,
    EOTID_NATIVE_INT64,
    EOTID_NATIVE_DOUBLE,
    EOTID_NATIVE_FLOAT,
    EOTID_NATIVE_LONG,
    EOTID_NATIVE_SHORT,
    EOTID_NATIVE_BOOL,
    EOTID_NATIVE_BYTE,
};

class EtchObject
{
public:
    /**
     * Constructor.
     */
    EtchObject(int32_t typeId);
    
    /**
     * Destructor.
     */
    virtual ~EtchObject();

    /**
     * Returns object type id.
     */
    int32_t getObjectTypeId();

private:
    int32_t m_typeId;
};

#endif

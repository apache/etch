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

#ifndef __ETCHSTRING_H__
#define __ETCHSTRING_H__

#include "common/EtchObject.h"

/**
 * String type.
 */
class EtchString :
    public EtchObject
{
public:

    /**
     * TypeId for EtchString.
     */
    static const int32_t TYPE_ID = EOTID_STRING;

    /**
     * Constructs the String.
     */
    EtchString();

    /**
     * Constructs the String.
     * @param string as c string
     */
    EtchString(const char* string);

    /**
     * Destructure.
     */
    virtual ~EtchString();

    /**
     * Set a new string.
     * @param string as c string
     */
    void set(const char* string);

    /**
     * Returns the amount of characters in the string.
     */
    int32_t lenght();

    /**
     * Returns c styled string.
     */
    const char* c_str();

private:
    char* m_data;
};

#endif

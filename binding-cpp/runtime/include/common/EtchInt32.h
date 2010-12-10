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

#ifndef __ETCHINT32_H__
#define __ETCHINT32_H__

#include "common/EtchObject.h"
#include "common/EtchError.h"

class EtchInt32 :
    public EtchObject
{
public:

    /**
     * TypeId for EtchString.
     */
    static const int32_t TYPE_ID = EOTID_INT32;

    /**
     * Constructs a EtchInt32 object.
     */
    EtchInt32();

    /**
     * Constructs a EtchInt32 object with given value.
     */
    EtchInt32(int32_t value);

    /**
     * Sets int32 value.
     */
    void set(int32_t value);

    /**
     * Returns int32 value.
     */
    int32_t get();

private:
    int32_t m_value;
};

#endif

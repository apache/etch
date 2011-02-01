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

#ifndef __ETCHDOUBLE_H__
#define __ETCHDOUBLE_H__

#include "common/EtchObject.h"
#include "common/EtchError.h"

class EtchDouble :
    public EtchObject
{
public:

    /**
     * TypeId for EtchDouble.
     */
    static const int32_t TYPE_ID = EOTID_DOUBLE;

    /**
     * Constructs a EtchDouble object.
     */
    EtchDouble();

    /**
     * Constructs a EtchDouble object with given value.
     */
    EtchDouble(double value);

    /**
     * Sets double value.
     */
    void set(double value);

    /**
     * Returns double value.
     */
    double get();

private:
    double m_value;
};

#endif

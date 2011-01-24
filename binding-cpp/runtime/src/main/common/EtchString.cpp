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

#include "common/EtchString.h"
#include "util/EtchUtil.h"

EtchString::EtchString()
    : EtchObject(EtchString::TYPE_ID)
    , m_data(NULL)
{
    //ETCH_LOG("EtchString: ", EtchLogLevel::Error, "dies ist ein Test");
}

EtchString::EtchString(const char* string)
    : EtchObject(EtchString::TYPE_ID)
    , m_data(NULL)
{
    if(string != NULL) {
        size_t len = strlen(string);
        m_data = new char[len + 1];
        etch_strcpy_s(m_data, len + 1, string);
    }
}

EtchString::~EtchString()
{
    if(m_data != NULL) {
        delete[] m_data;
        m_data = NULL;
    }
}

void EtchString::set(const char* string)
{
    if(m_data != NULL) {
        delete[] m_data;
        m_data = NULL;
    }
    if(string != NULL) {
        size_t len = strlen(string);
        m_data = new char[len + 1];
        etch_strcpy_s(m_data, len + 1, string);
    }
}

int32_t EtchString::lenght()
{
    return 0;
}

const char* EtchString::c_str()
{
    return m_data;
}

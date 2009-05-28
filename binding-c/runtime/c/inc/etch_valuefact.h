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
 * etch_valuefact.h 
 */

#ifndef ETCHVALFACT_H
#define ETCHVALFACT_H

#include "etch_structval.h"


/* 
 * value factory interface 
 */
struct i_value_factory
{
    unsigned short obj_type; 
    unsigned short class_id;  
    struct i_value_factory* vtab;

};

typedef struct i_value_factory i_value_factory;


/* 
 * value factory implementation 
 */
struct etch_value_factory
{
    unsigned short obj_type; 
    unsigned short class_id; 
    i_value_factory* vtab;
    void* impl;     
};

typedef struct etch_value_factory etch_value_factory;


/* 
 * constructors 
 */
etch_value_factory* new_etch_value_factory();
ETCH_VALUEFACTORY*  new_boxed_value_factory();



#endif /* #ifndef ETCHVALFACT_H*/ 
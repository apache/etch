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
 * etch_exception.h -- etch exception
 */

#ifndef ETCHEXCP_H
#define ETCHEXCP_H

#include "etch_object.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * create a new etch exception
 */
etch_exception* new_etch_exception(const excptype_t type);


/**
 * create a new builtin etch exception with errorcode
 */
etch_exception* new_etch_exception_from_errorcode(int errorcode);

/**
 * set message of the exception
 */
void etch_exception_set_message(etch_exception* ex, etch_string* mess);

/**
 * get message of the exception
 */
etch_string* etch_exception_get_message(etch_exception* ex);

/**
 * get exception type
 */
etch_status_t etch_exception_get_type(etch_exception* exception, excptype_t* type);

/**
 * error code of this exception
 */
uint32 etch_exception_get_errorcode(etch_exception* ex);

/**
 * destroy exception
 */
int destroy_etch_exception(void* excp);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHEXCP_H */

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
 * etch_errno.h -- error number file
 */

#include "etch.h"

#ifndef _ETCH_ERRNO_H_
#define _ETCH_ERRNO_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Type for specifying an error or status code.
 */
typedef int etch_status_t;

/* no error */
#define ETCH_SUCCESS        0
/* error */
#define ETCH_ERROR          1
/* invalid argument error */
#define ETCH_EINVAL         2
/* function not implemented */
#define ETCH_ENOTIMPL       3
/* not memory */
#define ETCH_ENOMEM         4
/* invalid state */
#define ETCH_EINVALSTATE    5
/* invalid state */
#define ETCH_EFILENOTFOUND  6
/* busy state */
#define ETCH_EBUSY          7
/* timeout */
#define ETCH_ETIMEOUT       8
/* index out of bound */
#define ETCH_EOUTOFBOUNDS   9
/* IO Error */
#define ETCH_EIO             10

#ifdef __cplusplus
} //extern "C"
#endif

#endif /* _ETCH_GENERAL_H_ */

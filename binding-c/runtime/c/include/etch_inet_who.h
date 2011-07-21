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
 * etch_inet_who.h 
 * subclass of etch_who holding an inet address.
 */

#ifndef ETCHINETWHO_H
#define ETCHINETWHO_H

#include "apr_network_io.h"
#include "etch_object.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct etch_inet_who {
    etch_object object;
} etch_inet_who;

etch_inet_who*  new_inet_who (apr_sockaddr_t*);

apr_sockaddr_t* inet_who_sockaddr (etch_inet_who*);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHINETWHO_H */

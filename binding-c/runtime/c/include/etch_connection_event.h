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
 * etch_connection.h
 * connection client and server classes - tcp
 */

#ifndef ETCH_CONNECTION_EVENT_H
#define ETCH_CONNECTION_EVENT_H

#include "etch_connection.h"
#include "etch_server.h"

#ifdef __cplusplus
extern "C" {
#endif

int etch_def_listener_on_event(etch_server*, etch_transport_connection*, const int e, int p1, void* p2);
int etch_def_connection_on_event(void*, const int e, int p1, void* p2);

#ifdef __cplusplus
}
#endif

#endif  /* ETCHTCPSERVER_H */

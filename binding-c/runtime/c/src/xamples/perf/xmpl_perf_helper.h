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
 * xmpl_perf_helper.h
 * transport helper for perf service
 */
#ifndef PERF_HELPER_H
#define PERF_HELPER_H

#include "etch_apr.h"
#include "etch_transport.h"
#include "etch_stub.h"
#include "xmpl_perf_client.h"
#include "xmpl_perf_server.h"
#include "xmpl_perf_client_stub.h"
#include "xmpl_perf_server_stub.h"
#include "xmpl_perf_remote_server.h"
#include "xmpl_perf_remote_client.h"
etch_runtime_exit_params exitparams;

void perf_client_cleanup (perf_remote_server*);
int  perf_runtime_init (const int is_client);
int  perf_runtime_exit ();

int  new_perf_resources (etch_server_factory*);

/* indicate if compiling an etch example as opposed to a regular etch client */
#define IS_ETCH_EXAMPLE TRUE 

/* client side user calls */
int etch_init();
int etch_exit();
perf_remote_server*  new_remote_server (wchar_t* uri, etch_client_factory*); 
perf_remote_server*  start_perf_client (wchar_t* uri, new_client_funcptr, const int waitms);
perf_client_impl*    get_perf_client_impl (perf_remote_server*);
perf_client_stub*    get_perf_client_stub (perf_remote_server*);
etch_stub*           get_perf_client_stubbase (perf_remote_server*);
int stop_perf_client (perf_remote_server*, const int waitdownms);

/* server side */
void* new_helper_accepted_server (etch_server_factory*, etch_session*);  
i_sessionlistener*  new_perf_listener (wchar_t* uri, main_new_server_funcptr);
int run_perf_listener (i_sessionlistener*, const int waitupms);


#endif /* PERF_HELPER_H */


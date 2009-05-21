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
 * xmpl_perf_client_main.h
 * client exe main() private header
 */

#ifndef ETCH_CLIENTMAIN_H
#define ETCH_CLIENTMAIN_H

#include "apr_time.h" /* some apr must be included first */
#include "etch_url.h"  
#include "etchlog.h"
#include "etchexcp.h"
#include "etchthread.h"
#include "etch_global.h"
#include "etch_encoding.h"
#include "etch_arrayval.h"
#include "xmpl_perf_helper.h"
#include "xmpl_perf_client_impl.h"
#include "xmpl_perf_remote_server.h"

#include <tchar.h>
#include <stdio.h>
#include <wchar.h>

/* example test declarations (see xmpl_perf_client_tests.c) */
int perf_xmpl_verify_server(perf_remote_server*);
int perf_xmpl_test_server (wchar_t*, new_client_funcptr, const int ms);


#if(0)

 IMPL_XXXX_CLIENT
 |  RemoteXxxxServer* server;
  - BASE_XXXX_CLIENT
    |- XXXX_CLIENT: XXXX (interfaces)
    |- /* client-directed methods if any */
    |- myfoo(); mybar(); etc. /* service XXXX interfaces */
    |- mydata;
     - OBJSESSION
       |  _sessionControl(), _sessionNotify(), _sessionQuery()
#endif


i_perf_server* new_server (etch_server_factory*);  

i_perf_client* new_perf_client (perf_remote_server*);


#endif /* ETCH_CLIENTMAIN_H */


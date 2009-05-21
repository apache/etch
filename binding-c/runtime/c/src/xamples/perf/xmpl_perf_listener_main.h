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
 * xmpl_perf_listener_main.h
 * server exe main() private header
 */

#ifndef ETCH_SERVERMAIN_H
#define ETCH_SERVERMAIN_H

#include "etch_apr.h"
#include "etch_url.h"  
#include "etchthread.h"
#include "etch_global.h"
#include "xmpl_perf_helper.h"
#include "xmpl_perf_server_impl.h"
#include "xmpl_perf_remote_client.h"
#include <tchar.h>
#include <stdio.h>

#if(0)

 IMPL_XXXX_SERVER
 |  RemotePerfClient* client;
 |  myfoo() { } etc; { /* service method implementations */ }
 |  mydata;
  - BASE_XXXX_SERVER
    |  myfoo(); mybar(); etc. /* service method empty stubs */  
    |- XXXX_SERVER: XXXX (interfaces)
    |- myfoo(); mybar(); etc. /* service method interfaces */
    |  mydata;
     - OBJSESSION
       |  _sessionControl(), _sessionNotify(), _sessionQuery()
#endif


i_perf_server* new_perf_server (etch_server_factory*, etch_session*);  


#endif /* ETCH_SERVERMAIN_H */


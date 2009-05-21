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
 * xmpl_perf_client_main.c 
 * implements a client session connection to the etch service server.
 */
#include "xmpl_perf_client_main.h"


/**
 * main   
 */
int _tmain (int argc, _TCHAR* argv[])
{
    int result = -1, is_waitkey = TRUE, waitms = 0;
    wchar_t* uri = argc > 1? argv[1]: L"tcp://127.0.0.1:4004"; 

    do
    {   perf_remote_server* remote = 0;
        if (0 != etch_init()) break; 

        #if(1)
        /* ensure server is functional */
        remote = start_perf_client (uri, new_perf_client, waitms);
        if (NULL == remote) break;

        result = perf_xmpl_verify_server (remote);

        /* stop and dispose of remote server */
        stop_perf_client (remote, waitms);
        ETCHOBJ_DESTROY(remote);
        if (0 != result) break; 
        #endif

        #if(1)    /* execute perf test suite */
        result = perf_xmpl_test_server (uri, new_perf_client, waitms);
        #endif

    } while(0);
    
    etch_exit ();
    return waitkey(is_waitkey, result);
}


/**
 * new_perf_client() (java binding's newPerfClient()).
 * callback constructor for client implementation object.
 * this callback address is passed to start_perf_client() in [main].
 * @param server the remote server. 
 * @remarks this callback must be supplied, i.e. its functionality cannot be 
 * defaulted, since the client implementation constructor new_perf_client_impl()
 * is not known to start_perf_client().
 */
i_perf_client* new_perf_client (perf_remote_server* server)
{
    perf_client_impl* client = new_perf_client_impl (server);
    return client? client->perf_client_base: NULL;
 }


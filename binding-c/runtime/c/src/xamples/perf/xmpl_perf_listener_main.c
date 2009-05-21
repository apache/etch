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
 * xmpl_perf_listener_main.c
 * establishes client session listeners on the server.
 */
#include "xmpl_perf_listener_main.h"


/**
 * main()
 * reference MainPerfListener.java. see serverexe.txt for callback map.    
 */
int _tmain(int argc, _TCHAR* argv[])
{
    int result = -1, is_waitkey = TRUE, waitupms = 4000;
    wchar_t* uri = argc > 1? argv[1]: L"tcp://127.0.0.1:4004";     

    if (0 == perf_runtime_init(INIT_ETCH_SERVER))  
    {                 
        i_sessionlistener* listener = new_perf_listener (uri, new_perf_server); 

        result = run_perf_listener (listener, waitupms); /* start server and block */

        /* at this point all connections are down and destroyed */
        ETCHOBJ_DESTROY(listener);  /* destroy_etch_listener() */
    }
    
    perf_runtime_exit ();
    return waitkey (is_waitkey, result);
}


/**
 * new_perf_server()
 * create an individual client's perf server implementation.
 * this is java binding's newPerfServer().
 * this is called back from helper.new_helper_accepted_server() (java's newServer).  
 * @param p parameter bundle. caller retains. 
 * @return the i_perf_server, whose thisx is the perf_server_impl.
 */
i_perf_server* new_perf_server (etch_server_factory* p, etch_session* session)
{
    perf_remote_client* client  = (perf_remote_client*) session->client;

    perf_server_impl* newserver = new_perf_server_impl (client);

    return newserver->perf_server_base;
}


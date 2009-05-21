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
 * xmpl_perf_client_stub.c 
 */

#include "apr_time.h" /* some apr must be included first */
#include "xmpl_perf_client_stub.h"
#include "xmpl_perf_valufact.h"
#include "etch_svcobj_masks.h"
#include "xmpl_perf_client.h"
#include "etch_global.h"
#include "etch_url.h"  

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

int destroy_my_client_stub (perf_client_stub*);


/* - - - - - - - - - - -
 * stub helper methods 
 * - - - - - - - - - - -
 */

/* these are the possibly threaded client side implementations of calls to the
 * service method logic in xxxx_client_impl. there is one such helper for each
 * client-directed method in the service. each assembles and validates method
 * parameters, invokes a call to the service logic, and transmits a result
 * message if indicated. each conforms to the opaque_stubhelper signature:
 * int stubhelper (void* stub, void* dsvc, void* obj, void* sender, void* msg); 
 * @param stub the invoking stub object 
 * @param dsvc delivery service - caller retains.
 * @param obj the service methods user logic, caller retains.
 * @param whofrom sender, caller relinquishes.
 * @param msg - the message, caller relinquishes. 
 * @return 0 success, -1 failure of the message_put or send. success means that  
 * the mechanism was successful, not necessarily that the service method logic
 * was successful. when the service method logic fails, the result object will
 * wrap an exception.
 *
 * NOTE CAREFULLY: at this writing, each stub helper method asserts its expected
 * parameters (halts execution on null), and otherwise always calls the implemented 
 * service method, which subsequently destroys whofrom and msg regardless of result. 
 * if this were changed to instead return some error indicator from the stub helper
 * method on detection of a null parameter, the stub helper method would need to 
 * first destroy the whofrom and msg parameters to fulfill the stub helper method
 * contract. 
 */


/**
 * perf_clientstub_run_nothing_
 * NO CLIENT-DIRECTED METHODS ARE DECLARED BY THIS SERVICE.
 * THIS IS AN EXAMPLE CLIENT_SIDE STUB HELPER FUNCTION.
 */
int perf_clientstub_run_nothing_ (etch_stub* stub, i_delivery_service* dsvc, 
    i_perf_client* client, etch_who* whofrom, etch_message* msg)
{
    perf_value_factory_impl* pvi = NULL;
    perf_value_factory* pvf = NULL;
    perf_client_impl* impl = NULL;

    /* objects specific to service.nothing_() */
    etch_field*  key_foo  = NULL;
    etch_int64*  val_foo  = NULL;
    etch_field*  key_bar  = NULL;
    etch_string* val_bar  = NULL;
    etch_int32* resultobj = NULL;

    etchstub_validate_args (stub, dsvc, msg, client, &pvf, &pvi, &impl);     

    key_foo = NULL; /* = pvi->statics._mf_foo; */
    key_bar = NULL; /* = pvi->statics._mf_bar; */
    ETCH_ASSERT(key_foo && key_bar);  

    /* nullarg asserts are initial tests only: server impl   
     * will generate exceptions on nullargs in the real world */
    val_foo = NULL; /* = (etch_int64*)  message_get (msg, key_foo); */
    val_bar = NULL; /* = (etch_string*) message_get (msg, key_bar); */
    ETCH_ASSERT(val_foo && val_bar); 

    /* execute the service method */
    resultobj = NULL; /* server->nothing_ (impl, val_foo, val_bar); */
    ETCH_ASSERT(resultobj); 

    /* transmit reply back to sender */
    return etchstub_send_reply (stub, dsvc, whofrom, msg, (void*) resultobj);
}


/* - - - - - - - - - 
 * constructors 
 * - - - - - - - - - 
 */

/**
 * new_perf_client_stub.
 * called from perf_remote_server* perfhelper.new_remote_server().
 * @param p client parameter bundle
 */
perf_client_stub* new_perf_client_stub (etch_client_factory* p)   
{
    perf_client_stub* mystub = NULL;
    i_delivery_service* ids = p->dsvc;
    i_perf_client* client = p->iclient;
    ETCH_ASSERT(is_etch_ideliverysvc(ids)); 
    ETCH_ASSERT(is_etch_client_base(client)); 

    mystub = new_clientstub_init (client, sizeof(perf_client_stub), 
        destroy_my_client_stub, ids, p->qpool, p->fpool, p);

    mystub->class_id  = get_dynamic_classid_unique(&CLASSID_STUBCLIENT_PERF);
    mystub->server_id = p->server_id;  

    /* initialize custom methods and data here */
    mystub->my_example_obj = etch_malloc(128, 0); /* example custom alloc */

    /* set stub helper methods */
    /* - - -  no client-directed methods declared in this service  - - - */

    return mystub;
}


/**
 * destroy_my_client_stub()
 * perf_client_stub user-allocated memory destructor.
 * called back from private object destructor destroy_stub_object().
 * if you explicitly allocate memory in the client stub object, destroy it here.
 */
int destroy_my_client_stub (perf_client_stub* mystub)
{
   /* free custom memory allocations here */
    etch_free(mystub->my_example_obj); /* free example alloc */

    return 0;
}




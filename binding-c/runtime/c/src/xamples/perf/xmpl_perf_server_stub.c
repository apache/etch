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
 * xmpl_perf_server_stub.c 
 */

#include "apr_time.h" /* some apr must be included first */
#include "xmpl_perf_server_stub.h"
#include "xmpl_perf_valufact.h"
#include "xmpl_perf_server.h"
#include "etch_global.h"
#include "etchexcp.h"
#include "etch_url.h"  
#include "etchlog.h"

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

char* ETCHSTBI = "STBI";
int destroy_my_server_stub (perf_server_stub*);


/* - - - - - - - - - - -
 * stub helper methods 
 * - - - - - - - - - - -
 */

/* these are the possibly threaded server side implementations of calls to the
 * service method logic in xxxx_server_impl. there is one such helper for each
 * server-directed method in the service. each assembles and validates method
 * parameters, invokes a call to the service logic, and transmits a result
 * message if indicated. each conforms to the opaque_stubhelper signature:
 * int stubhelper (void* stub, void* dsvc, void* obj, void* sender, void* msg); 
 * @param stub the invoking stub object 
 * @param dsvc delivery service - caller retains.
 * @param server the service methods user logic, caller retains.
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
 * perfstub_run_add
 */
int perfstub_run_add (etch_stub* stub, i_delivery_service* dsvc, 
    i_perf_server* server, etch_who* whofrom, etch_message* msg)
{
    perf_value_factory_impl* pvi = NULL;
    perf_value_factory* pvf = NULL;
    perf_server_impl* impl = NULL;

    /* objects specific to perf.add() */
    etch_field* key_x = NULL;
    etch_int32* val_x = NULL;
    etch_field* key_y = NULL;
    etch_int32* val_y = NULL;
    etch_int32* resultobj = NULL;

    etchstub_validate_args (stub, dsvc, msg, server, &pvf, &pvi, &impl);     

    key_x = pvi->statics._mf_x;
    key_y = pvi->statics._mf_y;
    ETCH_ASSERT(key_x && key_y);  

    /* nullarg asserts are initial tests only: server impl   
     * should generate exceptions on nullargs in the real world */
    val_x = (etch_int32*) message_get (msg, key_x);
    val_y = (etch_int32*) message_get (msg, key_y); 
    ETCH_ASSERT(val_x && val_y); 

    /* execute the service method */
    resultobj = server->add (impl, val_x, val_y);

    /* transmit reply back to sender */
    return etchstub_send_reply (stub, dsvc, whofrom, msg, (void*) resultobj);
}


/**
 * perfstub_run_sum
 */
int perfstub_run_sum (etch_stub* stub, i_delivery_service* dsvc, 
    i_perf_server* server, etch_who* whofrom, etch_message* msg)
{
    perf_value_factory_impl* pvi = NULL;
    perf_value_factory* pvf = NULL;
    perf_server_impl* impl = NULL;

    /* objects specific to perf.sum() */
    etch_field* key_a = NULL;
    etch_arraytype* val_a = NULL;
    etch_int32* resultobj = NULL;

    etchstub_validate_args (stub, dsvc, msg, server, &pvf, &pvi, &impl);     

    key_a = pvi->statics._mf_values;
    ETCH_ASSERT(key_a);  

    /* nullarg asserts are initial tests only: server impl   
     * should generate exceptions on nullargs in the real world */
    val_a = (etch_arraytype*) message_get (msg, key_a);
    ETCH_ASSERT(val_a); 

    /* execute the service method */
    resultobj = server->sum (impl, val_a);
    ETCH_ASSERT(resultobj); 

    /* transmit reply back to sender */
    return etchstub_send_reply (stub, dsvc, whofrom, msg, (void*) resultobj);
}


/**
 * perfstub_run_report
 */
int perfstub_run_report (etch_stub* stub, i_delivery_service* dsvc, 
    i_perf_server* server, etch_who* whofrom, etch_message* msg)
{
    perf_value_factory_impl* pvi = NULL;
    perf_value_factory* pvf = NULL;
    perf_server_impl* impl = NULL;
    int  result = 0;

    /* objects specific to perf.report() */
    etch_field* key_code = NULL;
    etch_int32* val_code = NULL;
    etch_field*  key_msg = NULL;
    etch_string* val_msg = NULL;
    etch_exception* excp = NULL;

    etchstub_validate_args (stub, dsvc, msg, server, &pvf, &pvi, &impl);     

    key_code = pvi->statics._mf_code;
    key_msg  = builtins._mf_msg;
    ETCH_ASSERT(key_code && key_msg);  

    /* nullarg asserts are initial tests only: the server implementation   
     * should generate exceptions on null args in the real world */
    val_code = (etch_int32*)  message_get (msg, key_code);
    val_msg  = (etch_string*) message_get (msg, key_msg);
    ETCH_ASSERT(val_code && val_msg); 

    /* execute the service method. for oneway messages such as this one,
     * a non-null return value will always be an exception object. */
    excp = server->report (impl, val_code, val_msg);
    
    if (is_etch_exception(excp)) /* transmit exception back to sender */
        result = etchstub_send_reply (stub, dsvc, whofrom, msg, (void*) excp);

    return result;
}


/**
 * perfstub_run_dist
 */
int perfstub_run_dist (etch_stub* stub, i_delivery_service* dsvc, 
    i_perf_server* server, etch_who* whofrom, etch_message* msg)
{
    perf_value_factory_impl* pvi = NULL;
    perf_value_factory* pvf = NULL;
    perf_server_impl* impl = NULL;

    /* objects specific to perf.dist() */
    etch_field* key_a = NULL;
    perf_point* val_a = NULL;
    etch_field* key_b = NULL;
    perf_point* val_b = NULL;
    perf_point* resultobj = NULL;

    etchstub_validate_args (stub, dsvc, msg, server, &pvf, &pvi, &impl);     

    key_a = pvi->statics._mf_a;
    key_b = pvi->statics._mf_b;
    ETCH_ASSERT(key_a && key_b);  

    /* nullarg asserts are initial tests only: server impl   
     * should generate exceptions on nullargs in the real world */
    val_a = (perf_point*) message_get (msg, key_a);
    val_b = (perf_point*) message_get (msg, key_b);
    ETCH_ASSERT(val_a && val_b); 

    /* execute the service method */
    resultobj = server->dist (impl, val_a, val_b);
    ETCH_ASSERT(resultobj);   
    
    /* transmit reply back to sender */
    return etchstub_send_reply (stub, dsvc, whofrom, msg, (void*) resultobj);
}


/**
 * perfstub_run_add2
 */
int perfstub_run_add2 (etch_stub* stub, i_delivery_service* dsvc, 
    i_perf_server* server, etch_who* whofrom, etch_message* msg)
{
    perf_value_factory_impl* pvi = NULL;
    perf_value_factory* pvf = NULL;
    perf_server_impl* impl = NULL;

    /* objects specific to perf.add2() */
    etch_field* key_date = NULL;
    etch_date*  val_date = NULL;
    etch_field* key_ms = NULL;
    etch_int64* val_ms = NULL;
    etch_date*  resultobj = NULL;

    etchstub_validate_args (stub, dsvc, msg, server, &pvf, &pvi, &impl);     

    key_date = pvi->statics._mf_ts;
    key_ms   = pvi->statics._mf_ms;
    ETCH_ASSERT(key_date && key_ms);  

    /* nullarg asserts are initial tests only: server impl   
     * should generate exceptions on nullargs in the real world */
    val_date = (etch_date*)  message_get (msg, key_date);
    val_ms   = (etch_int64*) message_get (msg, key_ms);
    ETCH_ASSERT(val_date && val_ms); 

    /* execute the service method */
    resultobj = server->add2 (impl, val_date, val_ms);
    ETCH_ASSERT(resultobj);   
    
    /* transmit reply back to sender */
    return etchstub_send_reply (stub, dsvc, whofrom, msg, (void*) resultobj);
}


/**
 * perfstub_run_report2
 */
int perfstub_run_report2 (etch_stub* stub, i_delivery_service* dsvc, 
    i_perf_server* server, etch_who* whofrom, etch_message* msg)
{
    perf_value_factory_impl* pvi = NULL;
    perf_value_factory* pvf = NULL;
    perf_server_impl* impl = NULL;
    int  result = 0;

    /* objects specific to perf.report2() */
    etch_field* key_date = NULL;
    etch_date*  val_date = NULL;
    etch_field* key_code = NULL;
    etch_int32* val_code = NULL;
    etch_field*  key_msg = NULL;
    etch_string* val_msg = NULL;
    etch_exception* excp = NULL;

    etchstub_validate_args (stub, dsvc, msg, server, &pvf, &pvi, &impl);     

    key_date = pvi->statics._mf_ts;
    key_code = pvi->statics._mf_code;
    key_msg  = builtins._mf_msg;
    ETCH_ASSERT(key_date && key_code && key_msg);  

    /* nullarg asserts are initial tests only: server impl   
     * should generate exceptions on nullargs in the real world */
    val_date = (etch_date*)   message_get (msg, key_date);
    val_code = (etch_int32*)  message_get (msg, key_code);
    val_msg  = (etch_string*) message_get (msg, key_msg);
    ETCH_ASSERT(val_date && val_code && val_msg); 

    /* execute the service method. for oneway messages such as this one,
     * a non-null return value will always be an exception object. */
    excp = server->report2 (impl, val_date, val_code, val_msg);
    
    if (is_etch_exception(excp)) /* transmit exception back to sender */
        result = etchstub_send_reply (stub, dsvc, whofrom, msg, (void*) excp);

    return result;
}


/* - - - - - - - - - 
 * constructors 
 * - - - - - - - - - 
 */

/**
 * new_perf_server_stub()
 * @param p serv factory parameter bundle, caller retains. 
 *
 * this constructor is called on the server via callback from the listener 
 * socket accept handler <transportfactory>_session_accepted, via the new_server 
 * function pointer to perf_helper.new_helper_accepted_server().
 * java binding passes this constructor the delivery service, however we pass the
 * etch_server_factory parameter bundle, (i_sessionlistener.server_params),
 * i_sessionlistener being the set session interface of etch_tcp_server. 
 */
perf_server_stub* new_perf_server_stub (etch_server_factory* p, etch_session* session)
{
    i_delivery_service* ids = session->ds;
    perf_value_factory* pvf = (perf_value_factory*) p->in_valufact;
    perf_value_factory_impl* pvi = (perf_value_factory_impl*) pvf->impl;
    etch_threadpool *qp = p->qpool, *fp = p->fpool;

    perf_server_stub* mystub = new_serverstub_init (session->server,  
        sizeof(perf_server_stub), destroy_my_server_stub, ids, qp, fp, p);

    mystub->class_id   = get_dynamic_classid_unique(&CLASSID_STUBSERVER_PERF);
    mystub->session_id = session->session_id;  

    /* initialize service-specific methods and data here. this facility may
     * likely prove unecessary, as this is generated code, in which case we 
     * can remove any associated comments from code and headers. */
    mystub->my_example_obj = etch_malloc(128, 0); /* example custom alloc */

    /* set stub "helper" methods (run() procedures for each message type) */
    etchtype_set_type_stubhelper(pvi->statics._mt_etch_xmpl_perfserver_add, 
        perfstub_run_add);
    etchtype_set_type_stubhelper(pvi->statics._mt_etch_xmpl_perfserver_sum, 
        perfstub_run_sum);
    etchtype_set_type_stubhelper(pvi->statics._mt_etch_xmpl_perfserver_report, 
        perfstub_run_report);
    etchtype_set_type_stubhelper(pvi->statics._mt_etch_xmpl_perfserver_dist, 
        perfstub_run_dist);
    etchtype_set_type_stubhelper(pvi->statics._mt_etch_xmpl_perfserver_add2, 
        perfstub_run_add2);
    etchtype_set_type_stubhelper(pvi->statics._mt_etch_xmpl_perfserver_report2, 
        perfstub_run_report2);

    return mystub;
}


/**
 * destroy_my_server_stub()
 * perf_server_stub user-allocated memory destructor.
 * called back from private object destructor destroy_stub_object().
 * if you explicitly allocate memory in the server stub object, destroy it here.
 */
int destroy_my_server_stub (perf_server_stub* mystub)
{
    /* free custom memory allocations here. this facility may
     * likely prove unecessary, as this is generated code, in which case we 
     * can remove any associated comments from code and headers. */
    etch_free(mystub->my_example_obj); /* free example alloc */

    return 0;
}


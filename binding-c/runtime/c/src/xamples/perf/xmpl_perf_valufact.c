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
 * xmpl_perf_valufact.c 
 * perf service value factory
 */

#include "xmpl_perf_valufact.h"
#include "xmpl_perf.h"
#include "etch_serializer.h"
#include "etch_global.h"
#include "etchexcp.h"
#include "etchutl.h"
#include "etchmap.h"

#if(0)
____________________________________________________________________________________
1.  WHAT TO DO WITH TYPES, FIELDS?
    a.  possibility 1: create a new types map from the default map
        1.  this works because the default types are marked static and so can have 
            benign destructor calls.
        2.  it is somewhat confusing and wasteful.
        3.  can be per-instance or per_service.
        4.  no concurrent access from other perfs
        5.  most foolproof 
    b.  possibility 2: add perf types to default types map
        1.  entails specifically removing each perf entry from the default map
        2.  if multiple instances of perf service, would need to either ignore  
            failres to insert, or keep a global perf_instantiations count. 
    c.  possibility 3: keep two maps, default and perf
        1.  would need to code a new lookup to look in perf map first, then default.
        2.  less efficient with time, more efficient with storage.
        3.  easier to create and tear down, per-instance, no collisions    

     we should instead instantiate an impl for the default vf
     then instantiate a default vf which IS the perf vf
     OR -- don't do a defvf impl, thus the defvf won't destroy the parent.
     defvf should never need to reference parent.
     if we DO need a defvf impl, it should not destroy the defvf of course.

     SO, do we use the default type maps, and add perf types to them?
     or do we instantiate the maps and pass them to default vf constructor.
     if we let the defvf instantiate maps, it will own them and destroy them. 
____________________________________________________________________________________    
#endif

/* constructors */
int  destroy_perf_value_factory_impl (perf_value_factory_impl*);
etch_arraylist* perfvf_get_types (perf_value_factory*);
int  perfvf_init (perf_value_factory*);
void perfvf_free_statics();

/* initializers */
void perfvf_instantiate_constants();
void perfvf_init_fields (perfvf_statics*);
void perfvf_init_types (default_value_factory*, perfvf_statics*);
void perfvf_init_parameters (perfvf_statics*);
int  perfvf_init_serializers (default_value_factory*, perfvf_statics*);

/* point serializer */
etch_serializer* new_perf_point_serializer(etch_type*, etch_field*); 
objmask* etchserializer_perf_point_export_value(etch_serializer*, objmask*);
objmask* etchserializer_perf_point_import_value(etch_serializer*, objmask*); 


/* - - - - - - - - - - - - - - - - - - - -
 * constructors/destructors
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * new_perf_value_factory()
 * constructor for perf_value_factory.
 * this value factory object is a default_value_factory having an impl of
 * xxxx_value_factory_impl, where xxxx is the service name. the impl contains 
 * the types, fields, and data unique to the service. the default_value_factory
 * implements the types, fields, and data common to all services.
 */
perf_value_factory* new_perf_value_factory ()
{
    vf_idname_map* typemap = NULL;
    class_to_type_map* c2tmap = NULL;
    perf_value_factory_impl* impl = NULL;

    default_value_factory* pvf = new_default_value_factory (NULL, NULL);
    ETCH_ASSERT(pvf);

    typemap = pvf->types;
    c2tmap  = pvf->class_to_type;
    ETCH_ASSERT(typemap && c2tmap);

    /* note that the vf destructor is the default value factory destructor,
     * which in turn invokes the destructor on its impl object 
     */
    impl = (perf_value_factory_impl*) new_object (sizeof(perf_value_factory_impl), 
       ETCHTYPEB_VALUEFACTIMP, get_dynamic_classid_unique(&CLASSID_PERF_VALUFACT_IMPL));

    impl->destroy = destroy_perf_value_factory_impl; 
    pvf->impl = (objmask*) impl;
 
    perfvf_init (pvf);
    
    return pvf; 
}


/**
 * destroy_perf_value_factory_impl()
 * destructor for the perf value factory extension
 */
int destroy_perf_value_factory_impl (perf_value_factory_impl* impl)  
{   
    if (NULL == impl) return -1;
    if (impl->refcount > 0 && --impl->refcount > 0) return -1;  

    if (!is_etchobj_static_content(impl))
    {   
        perfvf_free_statics(impl);
    }

   return destroy_objectex((objmask*) impl);
}


/**
 * perfvf_init()
 * initialize a service value factory object with fields and types.
 */
int perfvf_init (default_value_factory* pvf)
{
    int  result = 0;
    perf_value_factory_impl* impl = (perf_value_factory_impl*) pvf->impl;
    perfvf_statics* data = impl? &impl->statics: NULL;
    ETCH_ASSERT(impl && data); 
    memset(data, 0, sizeof(perfvf_statics));

    perfvf_init_fields(data);
    perfvf_init_types(pvf, data);
    perfvf_init_parameters(data);
    result = perfvf_init_serializers (pvf, data);

    /* todo make this a real singleton - don't re-instantiate where not necessary */
    _g_perf_statics = data;

    return result;
}


/* - - - - - - - - - - - - - - - - - - - -
 * vf class methods
 * - - - - - - - - - - - - - - - - - - - -
 */

/* 
 * perfvf_free_statics()
 * frees memory for etch-global quasi-static builtin objects,
 * and for the validators cache and its validator content.
 * it should be invoked at etch teardown, after last vf is destroyed.
 * unit tests will show memory leaks unless they invoke this post-test.
 */

void perfvf_free_statics (perf_value_factory_impl* impl)
{
    perfvf_statics* data = impl? &impl->statics: NULL;
    ETCH_ASSERT(impl && data); 

    destroy_static_type (data->_mt_etch_xmpl_perfserver_add);
    destroy_static_type (data->_mt_etch_xmpl_perfclient_result_add);
    destroy_static_type (data->_mt_etch_xmpl_perfserver_sum);
    destroy_static_type (data->_mt_etch_xmpl_perfclient_result_sum);
    destroy_static_type (data->_mt_etch_xmpl_perfserver_report);
    destroy_static_type (data->_mt_etch_xmpl_perfserver_dist);
    destroy_static_type (data->_mt_etch_xmpl_perfclient_result_dist);
    destroy_static_type (data->_mt_etch_xmpl_perfserver_add2);
    destroy_static_type (data->_mt_etch_xmpl_perfclient_result_add2);
    destroy_static_type (data->_mt_etch_xmpl_perfserver_report2);
    destroy_static_type (data->_mt_etch_xmpl_perf_point);

    destroy_static_field(data->_mf_x);
    destroy_static_field(data->_mf_y);
    destroy_static_field(data->_mf_a);
    destroy_static_field(data->_mf_b);
    destroy_static_field(data->_mf_ms);
    destroy_static_field(data->_mf_ts);
    destroy_static_field(data->_mf_code);
    destroy_static_field(data->_mf_values);

    etch_free (data->str_etch_perf_x);
    etch_free (data->str_etch_perf_y);
    etch_free (data->str_etch_perf_a);
    etch_free (data->str_etch_perf_b);
    etch_free (data->str_etch_perf_ms);
    etch_free (data->str_etch_perf_ts);
    etch_free (data->str_etch_perf_code);
    etch_free (data->str_etch_perf_values);

    etch_free (data->str_etch_perf_srv_add);
    etch_free (data->str_etch_perf_cli_result_add);
    etch_free (data->str_etch_perf_srv_sum);
    etch_free (data->str_etch_perf_cli_result_sum);
    etch_free (data->str_etch_perf_srv_report);
    etch_free (data->str_etch_perf_srv_dist);
    etch_free (data->str_etch_perf_cli_result_dist);
    etch_free (data->str_etch_perf_srv_add2);
    etch_free (data->str_etch_perf_cli_result_add2);
    etch_free (data->str_etch_perf_srv_report2);
    etch_free (data->str_etch_perf_point);
}


/**
 * perfvf_init_fields()
 * instantiate field objects
 */
void perfvf_init_fields (perfvf_statics* p)
{
    p->str_etch_perf_x       = new_wchar(L"x");  /* TODO strings should ideally */
    p->str_etch_perf_y       = new_wchar(L"y");  /* be global to the class */
    p->str_etch_perf_a       = new_wchar(L"a");
    p->str_etch_perf_b       = new_wchar(L"b");
    p->str_etch_perf_ms      = new_wchar(L"ms");
    p->str_etch_perf_ts      = new_wchar(L"ts");
    p->str_etch_perf_code    = new_wchar(L"code");
    p->str_etch_perf_values  = new_wchar(L"values");

    p->_mf_x      = new_static_field(p->str_etch_perf_x);
    p->_mf_y      = new_static_field(p->str_etch_perf_y);
    p->_mf_a      = new_static_field(p->str_etch_perf_a);
    p->_mf_b      = new_static_field(p->str_etch_perf_b);
    p->_mf_ms     = new_static_field(p->str_etch_perf_ms);
    p->_mf_ts     = new_static_field(p->str_etch_perf_ts);
    p->_mf_code   = new_static_field(p->str_etch_perf_code);
    p->_mf_values = new_static_field(p->str_etch_perf_values);
}


/**
 * perfvf_init_types()
 * instantiate type objects
 */
void perfvf_init_types (default_value_factory* pvf, perfvf_statics* p)
{
    etch_type* restype = NULL;

    /* instantiate type name strings */
    p->str_etch_perf_srv_add         = new_wchar(L"etch_perf_srv_add");
    p->str_etch_perf_cli_result_add  = new_wchar(L"etch_perf_cli_result_add");
    p->str_etch_perf_srv_sum         = new_wchar(L"etch_perf_srv_sum");
    p->str_etch_perf_cli_result_sum  = new_wchar(L"etch_perf_cli_result_sum");
    p->str_etch_perf_srv_report      = new_wchar(L"etch_perf_srv_report");
    p->str_etch_perf_srv_dist        = new_wchar(L"etch_perf_srv_dist");
    p->str_etch_perf_cli_result_dist = new_wchar(L"etch_perf_cli_result_dist");
    p->str_etch_perf_srv_add2        = new_wchar(L"etch_perf_srv_add2");
    p->str_etch_perf_cli_result_add2 = new_wchar(L"etch_perf_cli_result_add2");
    p->str_etch_perf_srv_report2     = new_wchar(L"etch_perf_srv_report2");
    p->str_etch_perf_point           = new_wchar(L"etch_perf_point");

    /* instantiate type objects */
    p->_mt_etch_xmpl_perfserver_add         = new_static_type(p->str_etch_perf_srv_add);
    ETCH_ASSERT(p->_mt_etch_xmpl_perfserver_add);
    p->_mt_etch_xmpl_perfclient_result_add  = new_static_type(p->str_etch_perf_cli_result_add);
    ETCH_ASSERT(p->_mt_etch_xmpl_perfclient_result_add);
    p->_mt_etch_xmpl_perfserver_sum         = new_static_type(p->str_etch_perf_srv_sum);
    ETCH_ASSERT(p->_mt_etch_xmpl_perfserver_sum);
    p->_mt_etch_xmpl_perfclient_result_sum  = new_static_type(p->str_etch_perf_cli_result_sum);
    ETCH_ASSERT(p->_mt_etch_xmpl_perfclient_result_sum);
    p->_mt_etch_xmpl_perfserver_report      = new_static_type(p->str_etch_perf_srv_report);
    ETCH_ASSERT(p->_mt_etch_xmpl_perfserver_report);
    p->_mt_etch_xmpl_perfserver_dist        = new_static_type(p->str_etch_perf_srv_dist);
    ETCH_ASSERT(p->_mt_etch_xmpl_perfserver_dist);
    p->_mt_etch_xmpl_perfclient_result_dist = new_static_type(p->str_etch_perf_cli_result_dist);
    ETCH_ASSERT(p->_mt_etch_xmpl_perfclient_result_dist);
    p->_mt_etch_xmpl_perfserver_add2        = new_static_type(p->str_etch_perf_srv_add2);
    ETCH_ASSERT(p->_mt_etch_xmpl_perfserver_add2);
    p->_mt_etch_xmpl_perfclient_result_add2 = new_static_type(p->str_etch_perf_cli_result_add2);
    ETCH_ASSERT(p->_mt_etch_xmpl_perfclient_result_add2);
    p->_mt_etch_xmpl_perfserver_report2     = new_static_type(p->str_etch_perf_srv_report2);
    ETCH_ASSERT(p->_mt_etch_xmpl_perfserver_report2);
    p->_mt_etch_xmpl_perf_point             = new_static_type(p->str_etch_perf_point);
    ETCH_ASSERT(p->_mt_etch_xmpl_perf_point);

    /* add types to vf */
    restype = pvf->vtab->add_type(pvf, p->_mt_etch_xmpl_perfserver_add);
    ETCH_ASSERT(restype);
    restype = pvf->vtab->add_type(pvf, p->_mt_etch_xmpl_perfclient_result_add);
    ETCH_ASSERT(restype);
    restype = pvf->vtab->add_type(pvf, p->_mt_etch_xmpl_perfserver_sum);
    ETCH_ASSERT(restype);
    restype = pvf->vtab->add_type(pvf, p->_mt_etch_xmpl_perfclient_result_sum);
    ETCH_ASSERT(restype);
    restype = pvf->vtab->add_type(pvf, p->_mt_etch_xmpl_perfserver_report);
    ETCH_ASSERT(restype);
    restype = pvf->vtab->add_type(pvf, p->_mt_etch_xmpl_perfserver_dist);
    ETCH_ASSERT(restype);
    restype = pvf->vtab->add_type(pvf, p->_mt_etch_xmpl_perfclient_result_dist);
    ETCH_ASSERT(restype);
    restype = pvf->vtab->add_type(pvf, p->_mt_etch_xmpl_perfserver_add2);
    ETCH_ASSERT(restype);
    restype = pvf->vtab->add_type(pvf, p->_mt_etch_xmpl_perfclient_result_add2);
    ETCH_ASSERT(restype);
    restype = pvf->vtab->add_type(pvf, p->_mt_etch_xmpl_perfserver_report2);
    ETCH_ASSERT(restype);
    restype = pvf->vtab->add_type(pvf, p->_mt_etch_xmpl_perf_point);
    ETCH_ASSERT(restype);

    /* set type response fields */
    etchtype_set_response_field(p->_mt_etch_xmpl_perfclient_result_add,  builtins._mf_result);
    etchtype_set_response_field(p->_mt_etch_xmpl_perfclient_result_sum,  builtins._mf_result);
    etchtype_set_response_field(p->_mt_etch_xmpl_perfclient_result_dist, builtins._mf_result);
    etchtype_set_response_field(p->_mt_etch_xmpl_perfclient_result_add2, builtins._mf_result);

    /* set message result types */
    etchtype_set_result_type (p->_mt_etch_xmpl_perfserver_add,  p->_mt_etch_xmpl_perfclient_result_add); 
    etchtype_set_result_type (p->_mt_etch_xmpl_perfserver_sum,  p->_mt_etch_xmpl_perfclient_result_sum);
    etchtype_set_result_type (p->_mt_etch_xmpl_perfserver_dist, p->_mt_etch_xmpl_perfclient_result_dist); 
    etchtype_set_result_type (p->_mt_etch_xmpl_perfserver_add2, p->_mt_etch_xmpl_perfclient_result_add2); 

    etchtype_set_timeout (p->_mt_etch_xmpl_perfserver_add,  0); 
    etchtype_set_timeout (p->_mt_etch_xmpl_perfserver_sum,  0); 
    etchtype_set_timeout (p->_mt_etch_xmpl_perfserver_dist, 0); 
    etchtype_set_timeout (p->_mt_etch_xmpl_perfserver_add2, 0); 
}


/**
 * perfvf_init_parameters()
 * initialize service method parameters
 */
void perfvf_init_parameters (perfvf_statics* p)
{
    unsigned short classid_perf_point = get_dynamic_classid_unique(&CLASSID_PERF_POINT);

   /* parameters for add() */
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_add, 
        clone_field(p->_mf_x), (objmask*) etchvtor_int32_get(0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_add, 
        clone_field(p->_mf_y), (objmask*) etchvtor_int32_get(0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_add, 
        clone_field(builtins._mf__message_id), (objmask*) etchvtor_int64_get(0));

    /* parameters for _result_add() */   
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_add, 
        clone_field(builtins._mf_result), (objmask*) etchvtor_int32_get(0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_add, 
        clone_field(builtins._mf__message_id), (objmask*) etchvtor_int64_get(0));
    /* TODO: this is runtime exception validator in java binding, however we 
     * don't have one currently. hopefully we will determine that we can make do  
     * with a single exception type, and not have to declare a type for each 
     * brand of exception */
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_add, 
        clone_field(builtins._mf_result), (objmask*) etchvtor_exception_get(0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_add, 
        clone_field(builtins._mf__in_reply_to), (objmask*) etchvtor_int64_get(0));

    /* parameters for sum() */
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_sum, 
        clone_field(p->_mf_values), (objmask*) etchvtor_int32_get(1));
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_sum, 
        clone_field(builtins._mf__message_id), (objmask*) etchvtor_int64_get(0));

    /* parameters for _result_sum() */   
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_sum, 
        clone_field(builtins._mf_result), (objmask*) etchvtor_int32_get(0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_sum, 
        clone_field(builtins._mf__message_id), (objmask*) etchvtor_int64_get(0));
    /* TODO: see comments at _result_add() */
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_sum, 
        clone_field(builtins._mf_result), (objmask*) etchvtor_exception_get(0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_sum, 
        clone_field(builtins._mf__in_reply_to), (objmask*) etchvtor_int64_get(0));

    /* parameters for report() */
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_report, 
        clone_field(p->_mf_code), (objmask*) etchvtor_int32_get(0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_report, 
        clone_field(builtins._mf_msg), (objmask*) etchvtor_string_get(0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_report, 
        clone_field(builtins._mf__message_id), (objmask*) etchvtor_int64_get(0));

    /* parameters for point */
    etchtype_put_validator(p->_mt_etch_xmpl_perf_point, 
        clone_field(p->_mf_x), (objmask*) etchvtor_int32_get(0));
    etchtype_put_validator(p->_mt_etch_xmpl_perf_point, 
        clone_field(p->_mf_y), (objmask*) etchvtor_int32_get(0));   

    /* parameters for dist */
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_dist,  
        clone_field(p->_mf_a), (objmask*) etchvtor_custom_get(ETCHTYPEB_USER, 
            classid_perf_point, p->_mt_etch_xmpl_perf_point, 0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_dist,   
        clone_field(p->_mf_b), (objmask*) etchvtor_custom_get(ETCHTYPEB_USER, 
            classid_perf_point, p->_mt_etch_xmpl_perf_point, 0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_dist, 
        clone_field(builtins._mf__message_id), (objmask*) etchvtor_int64_get(0));

    /* parameters for _result_dist */
    /* TODO see comments at parameters for dist */
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_dist,  
        clone_field(builtins._mf_result), (objmask*) etchvtor_custom_get(ETCHTYPEB_USER, 
            classid_perf_point, p->_mt_etch_xmpl_perf_point, 0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_dist, 
        clone_field(builtins._mf__message_id), (objmask*) etchvtor_int64_get(0));
    /* TODO: see comments at _result_add() */
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_dist, 
        clone_field(builtins._mf_result), (objmask*) etchvtor_exception_get(0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_dist, 
        clone_field(builtins._mf__in_reply_to), (objmask*) etchvtor_int64_get(0));

    /* parameters for add2() */
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_add2,  /* TODO implement custom validators */
        clone_field(p->_mf_ts), (objmask*) etchvtor_custom_get(ETCHTYPEB_PRIMITIVE, CLASSID_DATE, 
            builtins._mt__etch_datetime, 0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_add2, 
        clone_field(p->_mf_ms), (objmask*) etchvtor_int64_get(0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_add2, 
        clone_field(builtins._mf__message_id), (objmask*) etchvtor_int64_get(0));

    /* parameters for _result_add2() */
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_add2,  /* TODO implement custom validators */
        clone_field(builtins._mf_result), (objmask*) etchvtor_custom_get(ETCHTYPEB_PRIMITIVE, CLASSID_DATE, 
            builtins._mt__etch_datetime, 0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_add2, 
        clone_field(builtins._mf__message_id), (objmask*) etchvtor_int64_get(0));
    /* TODO: see comments at _result_add() */
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_add2,   
        clone_field(builtins._mf_result), (objmask*) etchvtor_exception_get(0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfclient_result_add2, 
        clone_field(builtins._mf__in_reply_to), (objmask*) etchvtor_int64_get(0));

    /* parameters for report2() */
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_report2, 
        clone_field(p->_mf_ts), (objmask*) etchvtor_custom_get(ETCHTYPEB_PRIMITIVE, CLASSID_DATE, 
            builtins._mt__etch_datetime, 0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_report2, 
        clone_field(p->_mf_code), (objmask*) etchvtor_int32_get(0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_report2, 
        clone_field(builtins._mf_msg), (objmask*) etchvtor_string_get(0));
    etchtype_put_validator(p->_mt_etch_xmpl_perfserver_report2, 
        clone_field(builtins._mf__message_id), (objmask*) etchvtor_int64_get(0));
}


/**
 * perfvf_init_serializers()
 */
int perfvf_init_serializers (default_value_factory* pvf, perfvf_statics* p)
{
    int  result = 0;
    etch_serializer* serializer = NULL; 
    class_to_type_map* c2tmap = pvf->class_to_type;
    const unsigned short classid_perf_point = get_dynamic_classid_unique(&CLASSID_PERF_POINT);

    /* note that etch_serializer_init takes care of the class to type, setting
     * of component type, instantiation of import export helper and installing
     * it to the type. 
     */

    /* serializer for point */
    result = etch_serializer_init (p->_mt_etch_xmpl_perf_point, p->str_etch_perf_point, 
        ETCHMAKECLASS(ETCHTYPEB_USER, classid_perf_point), 
            c2tmap, NULL, new_perf_point_serializer);

    return result;
}


/* - - - - - - - - - - - - - - - - - - - -
 * serializers
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * etchserializer_perf_point_export_value() 
 * export valueof a perf_point
 * @param objval a perf_point, caller owns it and presumably will
 * destroy it upon return from this method.
 * @return the exported disposable structvalue object. caller must cast it.
 */
objmask* etchserializer_perf_point_export_value(etch_serializer* thisx, objmask* objval)
{
    const int THISINITSIZE = 2;
    etch_structvalue* expstruct = NULL;
    const unsigned short classid_perf_point = get_dynamic_classid_unique(&CLASSID_PERF_POINT);
    if (!is_etch_objparams(objval, ETCHTYPEB_USER, classid_perf_point)) return NULL;

    expstruct = new_structvalue((etch_type*) thisx->type, THISINITSIZE);

    /* wrap value and assign wrapper object to the struct */
    structvalue_put(expstruct, clone_field(_g_perf_statics->_mf_x), 
       (objmask*) new_int32(((perf_point*)objval)->x));
    /* wrap value and assign wrapper object to the struct */
    structvalue_put(expstruct, clone_field(_g_perf_statics->_mf_y), 
       (objmask*) new_int32(((perf_point*)objval)->y));

    return (objmask*) expstruct; /* caller owns this structvalue */
}


/**
 * etchserializer_perf_point_import_value() 
 * import value for a perf_point.
 * @param objval an etch_structvalue of appropriate type for the perf_point.
 * caller retains ownership of this object as with all imports.
 * @return an opaque etch object containing the imported perf_point. 
 * caller owns and must destroy the returned object.  
 */
objmask* etchserializer_perf_point_import_value (etch_serializer* thisx, objmask* objval) 
{
    perf_point* outobj = NULL;
    etch_int32* valobj_x = NULL;
    etch_int32* valobj_y = NULL;
    etch_structvalue* instruct = NULL;
    if (!is_etch_struct(objval)) return NULL;

    instruct = (etch_structvalue*) objval;
    if (!structvalue_is_type(instruct, thisx->type)) return NULL;

    /* fetch the x value wrapper out of the struct. struct owns it */
    valobj_x = (etch_int32*) structvalue_get(instruct, _g_perf_statics->_mf_x);
    if (!is_etch_int32(valobj_x)) return NULL;

    /* fetch the y value wrapper out of the struct. struct owns it */
    valobj_y = (etch_int32*) structvalue_get(instruct, _g_perf_statics->_mf_y);
    if (!is_etch_int32(valobj_y)) return NULL;

    outobj = new_perf_point(valobj_x->value, valobj_y->value);

    return (objmask*) outobj;  /* caller owns this object */
}



/**
 * new_perf_point_serializer() 
 * etch_serializer_excp constructor - conforms to typedef etch_serializer_ctor
 * @param type  - not owned
 * @param field - not owned
 */
etch_serializer* new_perf_point_serializer(etch_type* type, etch_field* field) 
{
    etch_serializer* newobj = new_etch_serializer(ETCH_DEFSIZE);

    newobj->class_id = get_dynamic_classid_unique(&CLASSID_SERIALIZER_PERF_POINT);
    newobj->type  = type;  /* not owned */
    newobj->field = field; /* not owned */

    newobj->export_value = etchserializer_perf_point_export_value;
    newobj->import_value = etchserializer_perf_point_import_value;

    return newobj;
}









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
 * xmpl_perf.c 
 */

#include "apr_time.h" /* some apr must be included first */
#include "xmpl_perf.h"
#include "etch_global.h"
#include "etch_url.h"  

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

etch_int32* perf_def_add    (void* perfserver, etch_int32*, etch_int32*);
etch_int32* perf_def_sum    (void* perfserver, etch_arraytype*);
void*       perf_def_report (void* perfserver, etch_int32*, etch_string*);
perf_point* perf_def_dist   (void* p, perf_point*, perf_point*);
etch_date*  perf_def_add2   (void* p, etch_date*, etch_int64*); 
void*       perf_def_report2(void* p, etch_date*, etch_int32*, etch_string*);
int destroy_perf_service_interface (i_perf*);


/* - - - - - - - - - - - - - -  
 * constructors
 * - - - - - - - - - - - - - -  
 */

/**
 * new_perf_point()
 * point object constructor.
 */
perf_point* new_perf_point (const int x, const int y) 
{
    perf_point* point = (perf_point*) new_object (sizeof(perf_point), 
        ETCHTYPEB_USER, get_dynamic_classid_unique(&CLASSID_PERF_POINT));

    point->x = x;
    point->y = y;

    return point;
}


/**
 * is_perf_point()
 */
int is_perf_point(void* x) 
{   return x && ((objmask*)x)->class_id == CLASSID_PERF_POINT;
}


/**
 * new_perf_service_interface
 */
i_perf* new_perf_service_interface ()
{
    i_perf* isvc = (i_perf*) new_object (sizeof(i_perf), ETCHTYPEB_SVCINTERFACE, 
        get_dynamic_classid_unique(&CLASSID_PERF_SERVICE_INTERFACE));

    isvc->destroy = destroy_perf_service_interface;

    isvc->add     = perf_def_add;
    isvc->sum     = perf_def_sum;
    isvc->report  = perf_def_report;
    isvc->dist    = perf_def_dist;
    isvc->add2    = perf_def_add2;
    isvc->report2 = perf_def_report2;

    isvc->point   = new_perf_point(0,0);

    return isvc;
}


/**
 * destroy_perf_service_interface()
 * i_perf destructor.
 */
int destroy_perf_service_interface (i_perf* isvc)
{
    if (NULL == isvc) return -1;
    if (isvc->refcount > 0 && --isvc->refcount > 0) return -1;  

    if (!is_etchobj_static_content(isvc))
    {    
        etch_free(isvc->point);
    }
            
    return destroy_objectex((objmask*)isvc);
}


/* - - - - - - - - - - - - - -  
 * service method stubs
 * - - - - - - - - - - - - - -  
 */

etch_int32* perf_def_add (void* p, etch_int32* x, etch_int32* y)
{
    ETCHOBJ_DESTROY(x);
    ETCHOBJ_DESTROY(y);
    return NULL;
}


etch_int32* perf_def_sum (void* perfserver, etch_arraytype* x)
{
    ETCHOBJ_DESTROY(x);
    return NULL;
}


void* perf_def_report (void* perfserver, etch_int32* code, etch_string* x)
{ 
    ETCHOBJ_DESTROY(code);  
    ETCHOBJ_DESTROY(x);  
    return NULL;  
}


perf_point* perf_def_dist (void* p, perf_point* p1, perf_point* p2)
{
    etch_free(p1);
    etch_free(p2);
    return NULL;
}


etch_date* perf_def_add2 (void* p, etch_date* x, etch_int64* y)
{
    ETCHOBJ_DESTROY(x);
    ETCHOBJ_DESTROY(y);
    return NULL;
}


void* perf_def_report2 (void* p, etch_date* ts, etch_int32* n, etch_string* x)
{
    ETCHOBJ_DESTROY(ts);
    ETCHOBJ_DESTROY(n);
    ETCHOBJ_DESTROY(x);
    return NULL;
}

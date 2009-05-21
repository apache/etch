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
 * xmpl_perf_server_impl.c
 * your custom implementation of perf_server. 
 * add your implementations of messages from the client here.   
 * this code is generated in stub form, and manually augmented. 
 */

#include "apr_time.h" /* some apr must be included first */
#include "xmpl_perf_server_impl.h"
#include "etch_global.h"
#include "etch_arrayval.h"
#include "etch_binary_tdo.h"
#include "etchexcp.h"
#include "etch_url.h" 
#include "etchlog.h" 

#include <tchar.h>
#include <stdio.h>
#include <conio.h>
char* ETCHSIMP = "SIMP";

/* generated signatures */
int destroy_perf_server_implex (perf_server_impl*);
perf_server_impl* init_perf_server_impl (perf_remote_client*, etch_destructor);

etch_int32* perf_impl_add    (void* perfserver, etch_int32*, etch_int32*);
etch_int32* perf_impl_sum    (void* perfserver, void* array);
void*       perf_impl_report (void* perfserver, etch_int32*, etch_string*);
perf_point* perf_impl_dist   (void* thisx, perf_point*, perf_point*);
etch_date*  perf_impl_add2   (void* thisx, etch_date*, etch_int64*); 
void*       perf_impl_report2(void* thisx, etch_date*, etch_int32*, etch_string*);
int         perf_objsession_notify (i_objsession*, etch_event*);


/* - - - - - - - -    
 * instantiation
 * - - - - - - - -   
 */

/**
 * new_perf_server_impl()
 * perf_server_impl constructor.
 * add your custom initialization and virtual method overrides here.
 */
perf_server_impl* new_perf_server_impl (perf_remote_client* client) 
{
    perf_server_impl* serv  /* allocate object and assign default virtuals */
        = init_perf_server_impl (client, destroy_perf_server_implex); 
    i_perf_server* iserv = serv->perf_server_base;

    serv->class_id = get_dynamic_classid_unique(&CLASSID_PERFSERVERBASE_IMPL);

    /* * *  add your custom construction/initialization here  * * */
    serv->t0 = etch_system_nanotime();
    serv->exampleobj = etch_malloc(100,0);
    /* end custom construction/initialization */

    /* * *  add your virtual method overrides here  * * */
    /* todo avoid using and initializing two objects maybe don't put in impl */
    serv->add     = iserv->add     = perf_impl_add;
    serv->sum     = iserv->sum     = perf_impl_sum;;
    serv->report  = iserv->report  = perf_impl_report;
    serv->dist    = iserv->dist    = perf_impl_dist;
    serv->add2    = iserv->add2    = perf_impl_add2;
    serv->report2 = iserv->report2 = perf_impl_report2;
    /* end virtual method overrides */

    serv->_session_notify = serv->iobjsession->_session_notify = perf_objsession_notify;

    return serv;
}


/**
 * destroy_perf_server_implex()
 * destructor for any user allocated memory.
 * this code is invoked by the private perf_server_impl destructor,
 * via perf_server.destroyex(). add code here to destroy any memory you  
 * may have allocated for your custom perf_server implementation.
 */
int destroy_perf_server_implex (perf_server_impl* thisx)
{
    /* * *  add your custom destruction here  * * */
    etch_free(thisx->exampleobj);
    /* end custom destruction */

    return 0;
}


/* - - - - - - - - - - - - - - - -  
 * service method implementations
 * - - - - - - - - - - - - - - - -  
 */

/**
 * perf_impl_add()
 * execution of perf.add()
 * @param x an etch_int32 retained by the message value factory.
 * @param y an etch_int32 retained by the message value factory.
 * @return an etch_int32 whose value is the sum of x and y values.
 */
etch_int32* perf_impl_add (perf_server_impl* thisx, etch_int32* x, etch_int32* y)
{
    int is_argerr = 0;
    etch_int32* resultobj = new_int32(0);
    etchlog(ETCHSIMP, ETCHLOG_XDEBUG, "execute perf.add\n");
    thisx->adds++;
    ETCH_ASSERT(x && y); 
    is_argerr = x->is_null || y->is_null;
    if (x->value == FAKEBADDATA_INT32 || y->value == FAKEBADDATA_INT32) 
        is_argerr = TRUE;  /* artificial constant for negative test */

    if (is_argerr)  
        etch_throw (resultobj, EXCPTYPE_ILLEGALARG, L"bad data", 0);
    else
        resultobj->value = x->value + y->value; 
   
    return resultobj;
}


/**
 * perf_impl_sum()
 * execution of perf.sum()
 * @param a an etch_arrayvalue or etch_nativearray retained by the message's
 * value factory.
 * @return an etch_int32 whose value is the sum of the items in the array.
 */
etch_int32* perf_impl_sum (perf_server_impl* thisx, void* a)
{
    int is_argerr = 0;
    etch_int32* resultobj = new_int32(0);
    etchlog(ETCHSIMP, ETCHLOG_XDEBUG, "execute perf.sum\n");
    thisx->sums++;
    ETCH_ASSERT(a);

    do
    {   etch_arrayvalue* av = normalize_etch_array(a, 1);  
        etch_iterator iterator;
        if (is_argerr = (av == NULL || av->is_null)) break;
       
        arrayvalue_set_iterator(av, &iterator);

        while(iterator.has_next(&iterator))
        {
            etch_int32* thisitem = (etch_int32*) iterator.current_value;
            if (!is_etch_int32(thisitem) || thisitem->value == FAKEBADDATA_INT32)
            {   is_argerr = TRUE;
                break;
            }

            resultobj->value += thisitem->value;
            iterator.next(&iterator);
        }

    } while(0);

    if (is_argerr) etch_throw (resultobj, EXCPTYPE_ILLEGALARG, NULL, 0);

    #if(0)
    etchlog(ETCHSIMP, ETCHLOG_INFO, "perf_impl_sum test exception\n");
    etch_throw (resultobj, EXCPTYPE_ILLEGALARG, L"test exception", ETCHEXCP_COPYTEXT);
    #endif

    return resultobj;
}


/**
 * perf_impl_report()
 * execution of perf.report()
 * @param code an integer object retained by the message value factory.
 * @param x an etch_string retained by the message value factory.
 * @return null on success, an exception object on failure.
 */
void* perf_impl_report (perf_server_impl* thisx, etch_int32* code, etch_string* x)
{
    int is_argerr = 0;
    void* resultobj = NULL;
    etchlog(ETCHSIMP, ETCHLOG_XDEBUG, "execute perf.report\n");
    thisx->reports++;
    ETCH_ASSERT(code && x);
    is_argerr = code->is_null || x->is_null;
    /* artificial constants for negative tests */
    if (code->value == FAKEBADDATA_INT32) is_argerr = TRUE;
    if (0 == wcscmp(x->v.valw, FAKEBADDATA_STRING)) is_argerr = TRUE;   

    if (is_argerr)
        resultobj = new_etch_exception(EXCPTYPE_ILLEGALARG, NULL, 0);
     
    return resultobj;
}


/**
 * perf_impl_dist()
 * execution of perf.dist()
 * @param p1 a perf_point retained by the message value factory.
 * @param p2 a perf_point retained by the message value factory.
 * @return a perf_point.
 */
perf_point* perf_impl_dist (perf_server_impl* thisx, perf_point* p1, perf_point* p2)
{
    int is_argerr = 0;
    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    etchlog(ETCHSIMP, ETCHLOG_XDEBUG, "execute perf.dist\n");
    thisx->dists++; 
    ETCH_ASSERT(p1 && p2);
    is_argerr = !is_perf_point(p1) || !is_perf_point(p2);
    if (p1->x == FAKEBADDATA_INT32 || p1->y == FAKEBADDATA_INT32
     || p2->x == FAKEBADDATA_INT32 || p2->y == FAKEBADDATA_INT32)
        is_argerr = TRUE; /* artificial constant for negative tests */

    if (is_argerr)
        return (perf_point*) etch_throw (new_perf_point(0,0), 
            EXCPTYPE_ILLEGALARG, NULL, 0);

    x1 = p1->x; y1 = p1->y;  
    x2 = p2->x; y2 = p2->y; 

    return new_perf_point(x2 - x1, y2 - y1);
}


/**
 * perf_impl_add2()
 * execution of perf.add2()
 * @param objdate an etch_date retained by the message value factory.
 * @param objms an etch_int64 retained by the message value factory.
 * @return an etch_date*.
 */
etch_date* perf_impl_add2 (perf_server_impl* thisx, etch_date* objdate, etch_int64* objms)
{
    int  is_argerr = 0;
    etch_date* result_date = new_date();
    etchlog(ETCHSIMP, ETCHLOG_XDEBUG, "execute perf.add2\n");
    thisx->add2s++;
    ETCH_ASSERT(objdate && objms);
    is_argerr = objdate->is_null || objms->is_null;
    if (objdate->value == FAKEBADDATA_DATE || objms->value == FAKEBADDATA_INT64)
        is_argerr = TRUE; /* artificial constants for negative tests */

    if (is_argerr) 
        etch_throw (result_date, EXCPTYPE_ILLEGALARG, NULL, 0);
    else
        result_date->value = objdate->value + (objms->value / 1000);

    return result_date;
}


/**
 * perf_impl_report2()
 * execution of perf.report2()
 * @param ts an etch_date retained by the message value factory.    
 * @param n an integer object retained by the message value factory.
 * @param x an etch_string retained by the message value factory.
 * @return null on success, an exception object on failure.
 */
void* perf_impl_report2 (perf_server_impl* thisx, etch_date* ts, etch_int32* n, etch_string* x)
{
    int   is_argerr = 0;
    void* resultobj = NULL;
    etchlog(ETCHSIMP, ETCHLOG_XDEBUG, "execute perf.report2\n");
    thisx->report2s++;
    ETCH_ASSERT(ts && n && x);
    is_argerr = ts->is_null || n->is_null || x->is_null;
    /* artificial constants for negative tests */
    if (ts->value == FAKEBADDATA_DATE || n->value == FAKEBADDATA_INT32
    || (0 == wcscmp(x->v.valw, FAKEBADDATA_STRING))) is_argerr = TRUE;   

    if (is_argerr)
        resultobj = new_etch_exception(EXCPTYPE_ILLEGALARG, NULL, 0);

    return resultobj;
}
  

/* - - - - - - - - - - - - - - - - - - -
 * session interface method overrides
 * - - - - - - - - - - - - - - - - - - -
 */

int perf_objsession_notify (i_objsession* isession, etch_event* evt)
{
    int result = -1, evtype = evt? evt->value: 0;
    perf_server_impl* perfobj = (perf_server_impl*) isession->thisx;

    if (ETCHEVT_SESSION_DOWN == evtype)  
    {
        const int total 
            = perfobj->adds  + perfobj->sums  + perfobj->reports
            + perfobj->dists + perfobj->add2s + perfobj->report2s;

        int64 t0 = perfobj->t0, t1 = etch_system_nanotime();

        double dtime = (t1 - t0) / (double) 1000000000;
        double drate = total / dtime;

        char* mask 
            = "\nperf server %d down - adds %d sums %d reports %d dists %d " 
              "add2s %d report2s %d total %d %d time %7.3f rate %7.3f\n";

        printf(mask, perfobj->perf_server_base->server_id,
               perfobj->adds,  perfobj->sums,  perfobj->reports,
               perfobj->dists, perfobj->add2s, perfobj->report2s, 
               total, dtime, drate);

        result = 0;
    }

    ETCHOBJ_DESTROY(evt);
    return result;
}
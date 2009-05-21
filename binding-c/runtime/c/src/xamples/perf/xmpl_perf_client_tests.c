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
 * xmpl_perf_client_tests.c 
 * test code invoked from [main]
 */

#include "xmpl_perf_client_main.h"

char* ETCHTEST = "TEST";


/**
 * perftestobj
 * perftest "class" instance data
 */
typedef struct perftestobj
{
    unsigned sig;
    char* descr;
    wchar_t* uri;
    int   waitms;
    int   runtime;
    int   count;
    int   threads;
    int   iterations;
    new_client_funcptr new_client; /* user's main() client constructor */

} perftestobj;

int testerrors;
etch_objsession_objinfo notifyinfo;
typedef int (*perftest_threadproc) (etch_threadparams*);
#define EXCEPTION_EXPECTED 1
#define NOTIFY_EXPECTED    2
#define PERFTESTOBJ_SIGNATURE 0xabaddeed


/* - - - - - - - - - - - - - - -  
 * support methods for tests
 * - - - - - - - - - - - - - - -  
 */

/**
 * perf_xmpl_log_startmsg()
 * log start of individual test. 
 */
void perf_xmpl_log_startmsg (char* msgdescr)
{
    etchlog(ETCHTEST, ETCHLOG_INFO, "begin perf test message %s\n", msgdescr);
}

/**
 * perf_xmpl_log_endmsg()
 * log end of individual test.  
 */
void perf_xmpl_log_endmsg (char* msgdescr, const int result)
{
    etchlog(ETCHTEST, ETCHLOG_INFO, "end message %s result %d\n", msgdescr, result);
}


/**
 * perf_xmpl_log_startmsgex()
 * log start of individual test. 
 */
void perf_xmpl_log_startmsgex (char* msgdescr, const int count)
{
    etchlog(ETCHTEST, ETCHLOG_INFO, "begin %s count %d\n", msgdescr, count);
}

/**
 * perf_xmpl_log_endmsgex()
 * log end of individual test.  
 */
void perf_xmpl_log_endmsgex (char* msgdescr, const int result, const int count)
{
    etchlog(ETCHTEST, ETCHLOG_INFO, "end %s count %d result %d\n", msgdescr, count, result);
}

/**
 * perf_xmpl_log_objerr()
 * log unexpected test response object type.  
 */
void perf_xmpl_log_objerr (char* msgdescr)
{
    etchlog(ETCHTEST, ETCHLOG_ERROR, "response for %s was not an expected object type\n", msgdescr);
    testerrors++;
}


/**
 * perf_xmpl_log_resulterr()
 * log unexpected response object value. 
 */
void perf_xmpl_log_resulterr (char* msgdescr)
{
    etchlog(ETCHTEST, ETCHLOG_ERROR, "result value for %s was not the expected value\n", msgdescr);
    testerrors++;
}


/**
 * perf_xmpl_log_resultok()
 * log expected response received  
 */
void perf_xmpl_log_resultok (char* msgdescr, char* resultdescr)
{
    etchlog(ETCHTEST, ETCHLOG_INFO, "%s got expected result '%s'\n", msgdescr, resultdescr);
}


/**
 * perf_xmpl_log_resultokex()
 * log expected response received  
 */
void perf_xmpl_log_resultokex (char* msgdescr)
{
    etchlog(ETCHTEST, ETCHLOG_INFO, "%s got expected result\n", msgdescr);
}


/**
 * perf_xmpl_validate_response_object()
 * test response object for exception and log result.  
 */
int perf_xmpl_validate_response_object (objmask* responseobj, const int is_excp_expected)
{
    int result = -1;

    if (is_excp_expected == NOTIFY_EXPECTED)
    {
       /* we need to let the receive thread run here, otherwise the session_notify()
        * delivering the exception has not been called yet. this is a race condition
        * in the java code, subsequently translated here. todo come up with a synch-
        * ronization mechanism that will force the remote call to complete through
        * either mailbox post and read, or session_notify(). */
        etch_sleep(35);  /* fyi 20ms is not enough */

        if (notifyinfo.is_exception) 
        {  
            /* exception from 1-way message came back via overridden session_notify() */
            etchlog(ETCHTEST, ETCHLOG_ERROR, "user got expected exception response\n");
            result = 0;
        }
        else
            etchlog(ETCHTEST, ETCHLOG_ERROR, "missing session_notify exception\n");
    }
    else
    if (NULL == responseobj) 
        etchlog(ETCHTEST, ETCHLOG_ERROR, "user got null response\n");
    else
    if (is_exception(responseobj))
    {   
        etchexception* x = get_exception_from (responseobj);

        if (NULL == x)
            etchlog(ETCHTEST, ETCHLOG_ERROR, "user got malformed exception\n");
        else
        {   char* txta = x->ansitext? x->ansitext: "<no text>";
            if (is_excp_expected) 
            {   etchlog(ETCHTEST, ETCHLOG_ERROR, "user got expected exception '%s'\n", txta);
                result = 0;
            }
            else 
                etchlog(ETCHTEST, ETCHLOG_ERROR, "user got exception response '%s'\n", txta);
        }
    }
    else result = is_excp_expected? -1: 0;

    return result;
}


/**
 * perf_xmpl_wait_seconds()
 * sleep for nseconds, displaying a dot either every second or every five seconds. 
 */
void perf_xmpl_wait_seconds (int nseconds)
{
    int is_showdot = 0;
    const int is_longwait = nseconds > 20;
    if (nseconds <= 0) return;
    etchlog(ETCHTEST, ETCHLOG_INFO, "sleeping %d seconds per config ", nseconds);

    while(nseconds--) 
    { etch_sleep(1000); 
      is_showdot = is_longwait? nseconds % 5 == 0: TRUE; 
      if (is_showdot) printf("."); 
    }

    printf("\n");
}


etch_session_notify  old_session_notify;

void my_reset_notifyinfo() { memset(&notifyinfo, 0, sizeof(etch_objsession_objinfo)); }


/**
 * etchsession_my_session_notify()
 * override for client impl session_notify.
 * this override will receive notifications of unsolicited messages from server,
 * in particular exceptions returned from one-way messages. 
 * @param obj caller i_objsession*.
 * @param evt some etch object or null. relinquished and destroyed here.
 */
int my_session_notify  (void* obj, void* evt)
{
    etchlog(ETCHTEST, ETCHLOG_DEBUG, "user received session_notify\n");
    etchsession_get_objinfo (&notifyinfo, evt);
    return old_session_notify (obj, evt);  /* destroys evt object */
}


/**
 * override_client_session_notify()
 * override the client's default session_notify to be our own callback,
 * which saves the properties of the notified object prior to destroying the object.
 */
void my_override_client_session_notify (perf_remote_server* rs)
{
    etchlog(ETCHTEST, ETCHLOG_DEBUG, "user overrides session_notify\n");
    old_session_notify = perf_remote_set_session_notify (rs, my_session_notify);
    assert(old_session_notify);
}


/**
 * restore_client_session_notify()
 * restore the client's default session_notify to be the default callback,
 */
void my_restore_client_session_notify (perf_remote_server* rs)
{
    assert(old_session_notify);
    etchlog(ETCHTEST, ETCHLOG_DEBUG, "user restores session_notify\n");
    perf_remote_set_session_notify (rs, old_session_notify);
}


/**
 * perftest_get_params()
 * extract the perftest params from the etch thread params and validate 
 */
perftestobj* perftest_get_params(void* p)
{
    perftestobj* outobj = NULL;
    if (p && ((etch_threadparams*) p)->signature == ETCH_THREADPARAMS_SIGNATURE)
        outobj = ((etch_threadparams*) p)->data;
    assert(outobj && outobj->sig == PERFTESTOBJ_SIGNATURE);
    return outobj;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   
 * perftest_threadprocs for each server-directed service method test
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
 */

/* - - - - - - - - -  
 * perf.add()
 * - - - - - - - - -  
 */

/**
 * perftest_threadproc_add
 * perftest thread procedure for specific perf test "add"
 */
int perftest_threadproc_add (etch_threadparams* tp)
{
    perftestobj* p = perftest_get_params(tp);
    int result = 0, iterations = p->iterations;
    char* thistest = "perf.add";

    /* create parameter bundle with callback to editable client constructor */
    etch_client_factory* impl_factory = new_client_factory (NULL, NULL, p->new_client);

    /* instantiate a remote server, which invokes client constructor */  
    perf_remote_server* remote = new_remote_server (p->uri, impl_factory);
    assert(remote);

    /* redirect session notifications to here in order to catch exceptions
     * returned from one-way messages. */
    my_override_client_session_notify (remote);

    /* wait for server to come up */ 
    result = remote->remote_base->start_waitup (remote->remote_base, p->waitms);
    assert(0 == result);
    
    while(iterations--)  /* execute perf.add() n times */
    {
        etch_int32* resultobj = remote->add (remote, new_int32(1000000000), new_int32(2000000000));

        if (0 == (result = perf_xmpl_validate_response_object ((objmask*)resultobj, 0)))
            if  (is_etch_int32(resultobj))
                 if  (resultobj->value == 3000000000)
                      perf_xmpl_log_resultokex(thistest);
                 else perf_xmpl_log_resulterr(thistest);
            else perf_xmpl_log_objerr(thistest);

        ETCHOBJ_DESTROY(resultobj);
    }

    /* stop and dispose of remote server */
    /* note that if we omit the stop_waitdown() here, behavior is unchanged */
    result = remote->remote_base->stop_waitdown (remote->remote_base, p->waitms);
    assert(0 == result);

    ETCHOBJ_DESTROY(remote);

    return 0;
}


/**
 * perftest_threadproc_add_async
 * perftest thread procedure for specific perf test "add"
 */
int perftest_threadproc_add_async (etch_threadparams* tp)
{
    perftestobj* p = perftest_get_params(tp);
    int result = 0, iterations = p->iterations;

    /* create parameter bundle with callback to editable client constructor */
    etch_client_factory* impl_factory = new_client_factory (NULL, NULL, p->new_client);

    /* instantiate a remote server, which invokes client constructor */
    perf_remote_server* remote = new_remote_server (p->uri, impl_factory);
    assert(remote);

    /* redirect session notifications to here in order to catch exceptions
     * returned from one-way messages. */
    my_override_client_session_notify (remote);

    /* wait for server to come up */ 
    result = remote->remote_base->start_waitup (remote->remote_base, p->waitms);
    assert(0 == result);
    
    while(iterations--)  
    {   
        etch_int32 *resultobj = NULL;
        i_mailbox* mbox = NULL;

        mbox = remote->async_begin_add (remote, new_int32(1000000000), new_int32(2000000000));
        assert(mbox);   
         
        resultobj = remote->async_end_add (remote, mbox); 
        assert(resultobj); 
        
        ETCHOBJ_DESTROY(resultobj);
        perf_remote_dispose_mailbox (remote, &mbox);
    }

    /* dispose of server */
    result = remote->remote_base->stop_waitdown (remote->remote_base, p->waitms);
    assert(0 == result);
    ETCHOBJ_DESTROY(remote);

    return 0;
}


/* - - - - - - - - -  
 * perf.sum()
 * - - - - - - - - -  
 */

/**
 * perftest_threadproc_sum
 * perftest thread procedure for specific perf test "sum"
 */
int perftest_threadproc_sum (etch_threadparams* tp)
{
    perftestobj* p = perftest_get_params(tp);
    int result = 0, iterations = p->iterations;
    int values[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const int numdims = 1, itemsize = sizeof(int), itemcount = sizeof(values)/itemsize;
    char* thistest = "perf.sum";

    /* create parameter bundle with callback to editable client constructor */
    etch_client_factory* impl_factory = new_client_factory (NULL, NULL, p->new_client);

    /* instantiate a remote server, which invokes client constructor */
    perf_remote_server* remote = new_remote_server (p->uri, impl_factory);
    assert(remote);

    /* redirect session notifications to here in order to catch exceptions
     * returned from one-way messages. */
    my_override_client_session_notify (remote);

    /* wait for server to come up */ 
    result = remote->remote_base->start_waitup (remote->remote_base, p->waitms);
    assert(0 == result);
    
    while(iterations--)  /* execute perf.sum() n times */
    {   
        etch_nativearray* a = new_nativearray_from (values, CLASSID_ARRAY_INT32, 
            itemsize, numdims, itemcount, 0, 0);  

        etch_int32* resultobj = remote->sum (remote, (etch_arraytype*) a);

        if (0 == (result = perf_xmpl_validate_response_object ((objmask*)resultobj, 0)))
            if  (is_etch_int32(resultobj))
                 if  (resultobj->value == 36)
                      perf_xmpl_log_resultokex(thistest);
                 else perf_xmpl_log_resulterr(thistest);
            else perf_xmpl_log_objerr(thistest);

        ETCHOBJ_DESTROY(resultobj);
    }

    /* stop and dispose of remote server */
    /* note that if we omit the stop_waitdown() here, behavior is unchanged */
    result = remote->remote_base->stop_waitdown (remote->remote_base, p->waitms);
    assert(0 == result);

    ETCHOBJ_DESTROY(remote);

    return 0;
}


/* - - - - - - - - -  
 * perf.report()
 * - - - - - - - - -  
 */

/**
 * perftest_threadproc_report
 * perftest thread procedure for specific perf test "sum"
 */
int perftest_threadproc_report (etch_threadparams* tp)
{
    perftestobj* p = perftest_get_params(tp);
    int result = 0, iterations = p->iterations;
    const int msgcode = 23;
    wchar_t*  msgtext = L"this message describes the specifics of code 23";
    char* thistest = "perf.report";

    /* create parameter bundle with callback to editable client constructor */
    etch_client_factory* impl_factory = new_client_factory (NULL, NULL, p->new_client);

    /* instantiate a remote server, which invokes client constructor */
    perf_remote_server* remote = new_remote_server (p->uri, impl_factory);
    assert(remote);

    /* redirect session notifications to here in order to catch exceptions
     * returned from one-way messages. */
    my_override_client_session_notify (remote);

    /* wait for server to come up */ 
    result = remote->remote_base->start_waitup (remote->remote_base, p->waitms);
    assert(0 == result);
    
    while(iterations--)  /* execute perf.report() n times */
    {
        etch_int32* resultobj = remote->report (remote, new_int32(msgcode), new_stringw(msgtext));   

        if (0 == (result = perf_xmpl_validate_response_object ((objmask*)resultobj, 0)))
            if  (is_etch_int32(resultobj))
                 if  (resultobj->value == 0)
                      perf_xmpl_log_resultokex(thistest);
                 else perf_xmpl_log_resulterr(thistest);
            else perf_xmpl_log_objerr(thistest);

        ETCHOBJ_DESTROY(resultobj);
    }       

    /* dispose of server */
    result = remote->remote_base->stop_waitdown(remote->remote_base, p->waitms);
    assert(0 == result);

    ETCHOBJ_DESTROY(remote);

    return 0;
}


/* - - - - - - - - -  
 * perf.dist()
 * - - - - - - - - -  
 */

/**
 * perftest_threadproc_dist
 * perftest thread procedure for specific perf test "dist"
 */
int perftest_threadproc_dist (etch_threadparams* tp)
{
    perftestobj* p = perftest_get_params(tp);
    int result = 0, iterations = p->iterations;
    const int a1 = 1, a2 = 2, b1 = 1000000000, b2 = 2000000000;
    char* thistest = "perf.dist";

    /* seed parameter bundle with callback to editable client constructor */
    etch_client_factory* impl_factory = new_client_factory (NULL, NULL, p->new_client);

    /* instantiate a remote server, which invokes client constructor */
    perf_remote_server* remote = new_remote_server (p->uri, impl_factory);
    assert(remote);

    /* redirect session notifications to here in order to catch exceptions
     * returned from one-way messages. */
    my_override_client_session_notify (remote);

    /* wait for server to come up */ 
    result = remote->remote_base->start_waitup (remote->remote_base, p->waitms);
    assert(0 == result);
    
    while(iterations--)  /* execute perf.dist() n times */
    {
        perf_point* resultobj = remote->dist (remote, new_perf_point(a1,a2), new_perf_point(b1,b2));

        if (0 == (result = perf_xmpl_validate_response_object ((objmask*)resultobj, 0)))
            if  (is_perf_point(resultobj))
                 if  (resultobj->x && resultobj->y)  /* todo check actual expected values here */
                      perf_xmpl_log_resultokex(thistest);
                 else perf_xmpl_log_resulterr(thistest);
            else perf_xmpl_log_objerr(thistest);

        ETCHOBJ_DESTROY(resultobj);
    }

    /* dispose of server */
    /* note that if we omit the stop_waitdown() here, behavior is unchanged */
    result = remote->remote_base->stop_waitdown(remote->remote_base, p->waitms);
    assert(0 == result);

    ETCHOBJ_DESTROY(remote);

    return 0;
}


/* - - - - - - - - -  
 * perf.add2()
 * - - - - - - - - -  
 */

/**
 * perftest_threadproc_add2
 * perftest thread procedure for specific perf test "add2"
 */
int perftest_threadproc_add2 (etch_threadparams* tp)
{
    perftestobj* p = perftest_get_params(tp);
    int result = 0, iterations = p->iterations;
    const int64 adjsecs = 365*24*60*60, adjms = adjsecs * 1000; /* 1 year */
    char* thistest = "perf.add2";

    /* seed parameter bundle with callback to editable client constructor */
    etch_client_factory* impl_factory = new_client_factory (NULL, NULL, p->new_client);

    /* instantiate a remote server, which invokes client constructor */
    perf_remote_server* remote = new_remote_server (p->uri, impl_factory);
    assert(remote);

    /* redirect session notifications to here in order to catch exceptions
     * returned from one-way messages. */
    my_override_client_session_notify (remote);

    /* wait for server to come up */ 
    result = remote->remote_base->start_waitup (remote->remote_base, p->waitms);
    assert(0 == result);
    
    while(iterations--)  /* execute perf.dist() n times */
    {   
        etch_date* resultobj = remote->add2 (remote, new_date(), new_int64(adjms));

        if (0 == (result = perf_xmpl_validate_response_object ((objmask*)resultobj, 0)))
            if  (is_etch_date(resultobj))
                 if  (resultobj->value)  /* todo check actual expected value here */
                      perf_xmpl_log_resultokex(thistest);
                 else perf_xmpl_log_resulterr(thistest);
            else perf_xmpl_log_objerr(thistest);

        ETCHOBJ_DESTROY(resultobj);
    }

    /* dispose of server */
    result = remote->remote_base->stop_waitdown(remote->remote_base, p->waitms);
    assert(0 == result);

    ETCHOBJ_DESTROY(remote);

    return 0;
}


/* - - - - - - - - -  
 * perf.report2()
 * - - - - - - - - -  
 */

/**
 * perftest_threadproc_report2
 * perftest thread procedure for specific perf test "report2"
 */
int perftest_threadproc_report2 (etch_threadparams* tp)
{
    perftestobj* p = perftest_get_params(tp);
    int result = 0, iterations = p->iterations;
    const int msgcode = 23;
    wchar_t*  msgtext = L"this message describes the specifics of code 23";
    char* thistest = "perf.report2";

    /* seed parameter bundle with callback to editable client constructor */
    etch_client_factory* impl_factory = new_client_factory (NULL, NULL, p->new_client);

    /* instantiate a remote server, which invokes client constructor */
    perf_remote_server* remote = new_remote_server (p->uri, impl_factory);
    assert(remote);

    /* redirect session notifications to here in order to catch exceptions
     * returned from one-way messages. */
    my_override_client_session_notify (remote);

    /* wait for server to come up */ 
    result = remote->remote_base->start_waitup (remote->remote_base, p->waitms);
    assert(0 == result);
    
    while(iterations--)  /* execute perf.dist() n times */
    {
        etch_int32* resultobj = remote->report2 (remote, 
            new_date(), new_int32(msgcode), new_stringw(msgtext));

        if (0 == (result = perf_xmpl_validate_response_object ((objmask*)resultobj, 0)))
            if  (is_etch_int32(resultobj))
                 if  (resultobj->value == 0)
                      perf_xmpl_log_resultokex(thistest);
                 else perf_xmpl_log_resulterr(thistest);
            else perf_xmpl_log_objerr(thistest);

        ETCHOBJ_DESTROY(resultobj);
    }

    /* dispose of server */
    result = remote->remote_base->stop_waitdown(remote->remote_base, p->waitms);
    assert(0 == result);

    ETCHOBJ_DESTROY(remote);

    return 0;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * thread runners: execute specified test threadproc on specified number of threads
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * perftest_run_unthreaded()
 * general perftest main thread runner.
 */
void perftest_run_unthreaded (perftestobj* p, perftest_threadproc runner) 
{
    etch_threadparams threadparams; etch_init_threadparams(&threadparams);
    threadparams.data = p;
    etchlog(ETCHTEST, ETCHLOG_INFO, "RUNNING %s on MAIN THREAD\n", p->descr);

    runner(&threadparams);  
}


/**
 * perftest_run_threads()
 * general perftest concurrent test runner.
 * @param p the test data object, not owned here.
 */
void perftest_run_threads (perftestobj* p, perftest_threadproc runner) 
{
    int i = 0;
    etch_thread* newthread = 0;
    etch_threadpool* pool = new_threadpool(ETCH_THREADPOOLTYPE_FREE, p->threads); 
    pool->is_free_data = FALSE; /* prevent thread from destroying the perftestobj */
    etchlog(ETCHTEST, ETCHLOG_INFO, "RUNNING %s ON %d THREADS\n", p->descr, p->threads);

    for(; i < p->threads; i++) /* launch the requested number of tests */
    {   /* note that the pool threads are started immediately by default. 
         * if we wanted to instead start them explicitly, we would have 
         * set pool->is_manual_start true, above, and later called 
         * newthread->start(newthread);
         */
        etchlog(ETCHTEST, ETCHLOG_INFO, "STARTING %s ON THREAD %d\n", p->descr, i);
        newthread = pool->run (pool, runner, p);
        assert(newthread);
    }

    /* wait for all active pool threads to exit, and finally free memory */
    pool->destroy(pool);    
}


/**
 * perftest_run_one()
 * general perftest single test runner.
 */
double perftest_run_one (perftestobj* p, const int n, perftest_threadproc run)
{
    int64 t0 = etch_system_nanotime(), t1 = 0;
    p->iterations = n;

    if (p->threads <= 1)
        perftest_run_unthreaded(p, run);
    else
        perftest_run_threads(p, run);

    t1 = etch_system_nanotime();

    return (t1 - t0) / (double) 1000000000;
}  


/* - - - - - - - - - - - - - - - - - - - - - - - - 
 * test runner: per-service-method test execution
 * - - - - - - - - - - - - - - - - - - - - - - - - 
 */

/**
 * run_perftest()
 * combines java perftest constructor and java perftest.run() 
 */
int run_perftest (char* descr, wchar_t* uri, const int waitms, const int runtime, 
    const int count, const int threads, perftest_threadproc testproc, 
    new_client_funcptr new_perfclient)
{
    char* mask = NULL;
    perftestobj perftest; 
    etch_arraylist* list = NULL;
    double time = 0.0, sum = 0.0, r = 0.0;
    int  result = 0, k = 0, oldn = 0, n = 1, i = 0, times = 0;
    memset (&perftest, 0, sizeof(perftestobj));
    perftest.sig   = PERFTESTOBJ_SIGNATURE;
    perftest.count = count;
    perftest.descr = descr;
    perftest.uri   = uri;
    perftest.runtime = runtime;
    perftest.threads = threads;
    perftest.waitms  = waitms;
    perftest.new_client = new_perfclient;

    /* use this to temporarily test run_one */
    /* perftest_run_one (&perftest, n, testproc); */

    mask = "%s %d took %f -- trying %d to get >= 1 second\n";
    while(time < 1.0)
    {
        if (time > 0.1)
            etchlog(ETCHTEST, ETCHLOG_INFO, mask, descr, oldn, time, n);
        oldn = n;
        time = perftest_run_one (&perftest, n, testproc);
        etchlog(ETCHTEST, ETCHLOG_INFO, "****** run_one time result is %f\n", time);
        n *= 2;
        if (++times > 10) // failsafe loop exit
        {  etchlog(ETCHTEST, ETCHLOG_INFO, "exiting run loop after max 10 times\n");
           break;
        }
    }
    n = oldn;

#if(0)
    k = 2;
    n = (int) ((k * n) / time);
    mask = "%s %d took %f -- trying %d for %d seconds\n";
    etchlog(ETCHTEST, ETCHLOG_INFO, mask, descr, oldn, time, n, k);
    oldn = n;
    time = perftest_run_one(&perftest, n, testproc);

    k = 4;
    n = (int) ((k * n) / time);
    mask = "%s %d took %f -- trying %d for %d seconds\n";
    etchlog(ETCHTEST, ETCHLOG_INFO, mask, descr, oldn, time, n, k);
    oldn = n;
    time = perftest_run_one(&perftest, n, testproc);

    n = (int) ((runtime * n) / time);
    mask = "%s %d took %f -- using %d for %d seconds\n";
    etchlog(ETCHTEST, ETCHLOG_INFO, mask, descr, oldn, time, n, runtime);

    list = new_arraylist(count, 0);
    list->content_type = ETCHARRAYLIST_CONTENT_OBJECT;

    mask = "%s %d/%d\t%d\t%d\t%f\t%f\t%f\n";
    for(i = 0; i < count; i++)
    {
        time = perftest_run_one(&perftest, n, testproc);
        sum += (r = n / time);
        arraylist_add(list, new_double(r));
        etchlog(ETCHTEST, ETCHLOG_INFO, mask, descr, i, count, threads, 
            n, time, r, r*threads);
    }
#endif

    /* todo: calculate and display min, max, media, mean */
    ETCHOBJ_DESTROY(list);
    return 0;
}


/**
 * perf_xmpl_verify_server()
 * ensure server is working.  
 * @return count of failed tests.
 */
int perf_xmpl_verify_server (perf_remote_server* remote)
{ 
    char* thistest = NULL;
    int testresult = 0;

    /* redirect session notifications to here in order to catch exceptions
     * returned from one-way messages. */
    my_override_client_session_notify (remote);

    #if(1)  

    do  /* perf.add() */
    {   etch_int32* addresult;
        perf_xmpl_log_startmsg (thistest = "perf.add 1");

        /* execute the remote add() call 2 + 3 */
        /* arguments objects are relinquished, result object is assumed */
        addresult = remote->add (remote, new_int32(2), new_int32(3));  

        if (0 == (testresult = perf_xmpl_validate_response_object ((objmask*)addresult, 0)))
            if  (is_etch_int32(addresult))
                 if  (addresult->value == 5)
                      perf_xmpl_log_resultok(thistest, "2 + 3 = 5");
                 else perf_xmpl_log_resulterr(thistest);
            else perf_xmpl_log_objerr(thistest);
                    
        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(addresult);
    } while(0);

    #endif

    #if(1)  

    do  /* perf.add() negative test */
    {   etch_int32* addresult;
        perf_xmpl_log_startmsg (thistest = "perf.add 2");
        /* this test sends a bad object type (float) to perf.add().
         * the result of this test will differ depending on whether validation
         * is enabled on output. if so, an illegal arg exception should be thrown
         * at the client which user will see. if not, serialization will fail at
         * the server due to the validation error, server will not return a
         * perf.add result, and user should see a timeout exception.
         */

        /* execute the remote add() call with invalid object types */
        /* arguments objects are relinquished, result object is assumed */
        addresult = remote->add (remote, (etch_int32*) new_int32(2), (etch_int32*) new_float(3));

        if (0 != (testresult = perf_xmpl_validate_response_object((objmask*)addresult, EXCEPTION_EXPECTED)))
            perf_xmpl_log_resulterr(thistest);
                    
        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(addresult);
    } while(0);

    #endif


    #if(1) 

    do  /* perf.add() negative test */
    {   etch_int32* addresult;
        perf_xmpl_log_startmsg (thistest = "perf.add 3");
        /* this test sends a certain integer value to perf.add() which the server
         * implementation is coded to interpret as bad data for which it should
         * throw an exception. the exception should be serialized back across the
         * wire and user should see the exception.        
         */

        /* execute the remote add() call with invalid object types */
        /* arguments objects are relinquished, result object is assumed */
        addresult = remote->add (remote, (etch_int32*) 
            new_int32(FAKEBADDATA_INT32),(etch_int32*) new_int32(3));

        if  (0 != (testresult = perf_xmpl_validate_response_object((objmask*)addresult, EXCEPTION_EXPECTED)))
             perf_xmpl_log_resulterr(thistest);
                    
        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(addresult);
    } while(0);

    #endif


    #if(1)  

    do   /* perf.sum() nativearray */
    {   const int ITEMSIZE = sizeof(int), NUMDIMS = 1, NUMITEMS = 3;
        const int DIM0 = NUMITEMS, DIM1 = 0, DIM2 = 0;
        int ival0 = 1, ival1 = 2, ival2 = 3;

        etch_int32* sumresult;
        etch_nativearray* myarray; 
        perf_xmpl_log_startmsg (thistest = "perf.sum 1");

        myarray = new_nativearray(CLASSID_ARRAY_INT32, sizeof(int), NUMDIMS, DIM0, DIM1, DIM2);
        myarray->put1(myarray, &ival0, 0);
        myarray->put1(myarray, &ival1, 1);
        myarray->put1(myarray, &ival2, 2);

        /* execute the remote sum() call */
        /* arguments objects are relinquished, result object is assumed */
        sumresult = remote->sum (remote, (etch_arraytype*) myarray);

        if  (0 != (testresult = perf_xmpl_validate_response_object((objmask*)sumresult, 0)))
            if  (is_etch_int32(sumresult))
                 if  (sumresult->value == 6)
                      perf_xmpl_log_resultok(thistest, "sum(1,2,3)=6");
                 else perf_xmpl_log_resulterr(thistest);
            else perf_xmpl_log_objerr(thistest);
                    
        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(sumresult);
    } while(0);

    #endif  


    #if(1)  

    do   /* perf.sum() arrayvalue */
    {   etch_int32* sumresult;
        etch_arrayvalue* myarray; 
        perf_xmpl_log_startmsg (thistest = "perf.sum 2");

        myarray = new_arrayvalue (ETCH_XTRNL_TYPECODE_INT, NULL, 1, 4, 4, FALSE, FALSE);
        arrayvalue_add(myarray, (etch_object*) new_int32(1));
        arrayvalue_add(myarray, (etch_object*) new_int32(2));
        arrayvalue_add(myarray, (etch_object*) new_int32(3));

        /* execute the remote sum() call */
        /* arguments objects are relinquished, result object is assumed */
        sumresult = remote->sum (remote, (etch_arraytype*) myarray);

        if  (0 == (testresult = perf_xmpl_validate_response_object((objmask*)sumresult, 0)))
            if  (is_etch_int32(sumresult))
                 if  (sumresult->value == 6)
                      perf_xmpl_log_resultok(thistest, "sum(1,2,3)=6");
                 else perf_xmpl_log_resulterr(thistest);
            else perf_xmpl_log_objerr(thistest);
                    
        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(sumresult);
    } while(0);

    #endif  


    #if(1)  

    do   /* perf.sum() negative - bad array object */
    {   etch_int32 *sumresult, *not_an_array = new_int32(0);
        perf_xmpl_log_startmsg (thistest = "perf.sum 3");

        /* execute the remote sum() call  
         * arguments objects are relinquished, result object is assumed  
         * if validate on write is on, we expect a validation error at the client.
         * if validate on write is off, we expect no result from server due to the
         * resulting deserialization error, and a timeout exception at the client 
         */
        sumresult = remote->sum (remote, (etch_arraytype*) not_an_array);

        if (0 != (testresult = perf_xmpl_validate_response_object((objmask*)sumresult, EXCEPTION_EXPECTED)))
            perf_xmpl_log_resulterr(thistest);
             
        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(sumresult);
    } while(0);

    #endif  


    #if(1)  

    do   /* perf.sum() negative - bad array value */
    {   const int ITEMSIZE = sizeof(int), NUMDIMS = 1, NUMITEMS = 3;
        const int DIM0 = NUMITEMS, DIM1 = 0, DIM2 = 0;
        int ival0 = 1, ival1 = FAKEBADDATA_INT32, ival2 = 3;  /* <=== BAD DATA */

        etch_int32* sumresult;
        etch_nativearray* myarray; 
        perf_xmpl_log_startmsg (thistest = "perf.sum 4");

        myarray = new_nativearray(CLASSID_ARRAY_INT32, sizeof(int), NUMDIMS, DIM0, DIM1, DIM2);
        myarray->put1(myarray, &ival0, 0);
        myarray->put1(myarray, &ival1, 1);
        myarray->put1(myarray, &ival2, 2);

        /* execute the remote sum() call  
         * arguments objects are relinquished, result object is assumed  
         * we expect to get an illegal argument exception  back from the server here.
         */
        sumresult = remote->sum (remote, (etch_arraytype*) myarray);

        if (0 != (testresult = perf_xmpl_validate_response_object((objmask*)sumresult, EXCEPTION_EXPECTED)))
            perf_xmpl_log_resulterr(thistest);
                    
        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(sumresult);
    } while(0);

    #endif  


    #if(1)  
                                          
    do  /* perf.report() */
    {   objmask* represult = NULL;
        perf_xmpl_log_startmsg (thistest = "perf.report 1");

        /* execute the remote report() call.
         * arguments objects are relinquished, result object is assumed.  
         * this service method has no return type, however c binding always   
         * returns an object shell in order to host a possible exception. 
         */
        represult  = remote->report (remote, new_int32(18), new_stringw(L"starting"));
 
        testresult = perf_xmpl_validate_response_object ((objmask*)represult, 0);
       
        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(represult);

    } while(0);

    #endif

     
    #if(1)  
                                          
    do  /* perf.report() negative - bad parameter object */
    {   objmask* represult = NULL;
        perf_xmpl_log_startmsg (thistest = "perf.report 2");
        my_reset_notifyinfo();

        /* execute the remote report() call.
         * arguments objects are relinquished, result object is assumed.  
         * this service method has no return type, however c binding always   
         * returns an object shell in order to host a possible exception. 
         */                                 /* double is bad param here */
        represult = remote->report (remote, (etch_int32*) new_double(0), new_stringw(L"it works!"));

        if (0 != (testresult = perf_xmpl_validate_response_object((objmask*)represult, EXCEPTION_EXPECTED)))
            perf_xmpl_log_resulterr(thistest);
 
        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(represult);

    } while(0);

    #endif


    #if(1)  
                                          
    do  /* perf.report() negative - bad data */
    {   objmask* represult = NULL;
        perf_xmpl_log_startmsg (thistest = "perf.report 3");
        my_reset_notifyinfo();

        /* execute the remote report() call.
         * arguments objects are relinquished, result object is assumed.  
         * this service method has no return type, however c binding always   
         * returns an object shell in order to host a possible exception. 
         */                                             /* FAKEBADDATA_STRING is bad data */
        represult = remote->report (remote, new_int32(18), new_stringw(FAKEBADDATA_STRING));
                               
        if (0 != (testresult = perf_xmpl_validate_response_object((objmask*)represult, NOTIFY_EXPECTED)))
            perf_xmpl_log_resulterr(thistest);
       
        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(represult);

    } while(0);

    #endif


    #if(1)

    do  /* perf.dist() */
    {   perf_point* distresult;
        perf_xmpl_log_startmsg (thistest = "perf.dist 1");

        /* execute the remote dist() call dist(point(1,2), point(3,5)) */
        /* arguments objects are relinquished, result object is assumed */
        distresult = remote->dist (remote, new_perf_point(1,2), new_perf_point(3,5));

        if  (0 == (testresult = perf_xmpl_validate_response_object((objmask*)distresult, 0)))
            if  (is_perf_point(distresult))
                 if  (distresult->x == 2 && distresult->y == 3)
                      perf_xmpl_log_resultok (thistest, "dist((1,2),(3,5)) = (2,3)");
                 else perf_xmpl_log_resulterr(thistest);
            else perf_xmpl_log_objerr(thistest);

        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(distresult);
    } while(0);

    #endif


   #if(1)

    do  /* perf.dist() negative - bad parameter type */
    {   perf_point* distresult;
        perf_xmpl_log_startmsg (thistest = "perf.dist 2");     

        /* execute the remote dist() call dist(point(1,2), point(3,5)) */
        /* arguments objects are relinquished, result object is assumed */
        distresult = remote->dist (remote, new_perf_point(1,2), (perf_point*) new_structvalue(NULL, 0));
 
        if  (0 != (testresult = perf_xmpl_validate_response_object((objmask*)distresult, EXCEPTION_EXPECTED)))
             perf_xmpl_log_resulterr(thistest);
                    
        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(distresult);
    } while(0);

    #endif


    #if(1)

    do  /* perf.dist() negative - bad parameter value */
    {   perf_point* distresult;
        perf_xmpl_log_startmsg (thistest = "perf.dist 3");     

        /* execute the remote dist() call dist(point(1,2), point(3,5)) */
        /* arguments objects are relinquished, result object is assumed */  
        distresult = remote->dist (remote, new_perf_point(1,2), new_perf_point(1, FAKEBADDATA_INT32));
 
        if  (0 != (testresult = perf_xmpl_validate_response_object((objmask*)distresult, EXCEPTION_EXPECTED)))
             perf_xmpl_log_resulterr(thistest);
                    
        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(distresult);
    } while(0);

    #endif


    #if(1)

    do  /* perf.add2() */
    {   etch_date* add2result;
        const int64 adjms = 3600000;
        etch_date*  now = new_date();
        /* for this test, save date value for verify below, since date object relinquished */
        const time_t oldtime = now->value; 
        perf_xmpl_log_startmsg (thistest = "perf.add2 1");

        /* execute the remote add2() call add2(date, msecs) */
        /* arguments objects are relinquished, result object is assumed */
        add2result = remote->add2(remote, now, new_int64(adjms));

        if  (0 == (testresult = perf_xmpl_validate_response_object((objmask*)add2result, 0)))
            if  (is_etch_date(add2result))
                 if (add2result && !is_exception(add2result))
                 {   const time_t newtime = add2result->value; /* time_t is in seconds */
                     const int64  adjsecs = adjms / 1000;  /* so convert ms to seconds */
                     const int64  chktime = newtime - adjsecs; 
                     if  (chktime == oldtime) 
                          perf_xmpl_log_resultok (thistest, "for add2(now, 3600000ms)");
                     else perf_xmpl_log_resulterr(thistest);
                 }
                 else perf_xmpl_log_resulterr(thistest);
            else perf_xmpl_log_objerr(thistest);

        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(add2result);
    } while(0);

    #endif


    #if(1)

    do  /* perf.add2() negative - bad parameter type */
    {   etch_date* add2result;
        perf_xmpl_log_startmsg (thistest = "perf.add2 2");

        /* execute the remote add2() */
        /* arguments objects are relinquished, result object is assumed */
        add2result = remote->add2(remote, (etch_date*) new_double(0), new_int64(0));

        if  (0 != (testresult = perf_xmpl_validate_response_object((objmask*)add2result, EXCEPTION_EXPECTED)))
             perf_xmpl_log_resulterr(thistest);

        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(add2result);
    } while(0);

    #endif


    #if(1)

    do  /* perf.add2() negative - bad data value */
    {   etch_date* add2result, *bad_date = new_date();
        bad_date->value = FAKEBADDATA_DATE;
        perf_xmpl_log_startmsg (thistest = "perf.add2 3");

        /* execute the remote add2() */
        /* arguments objects are relinquished, result object is assumed */
        add2result = remote->add2(remote, bad_date, new_int64(0));

        if  (0 != (testresult = perf_xmpl_validate_response_object((objmask*)add2result, EXCEPTION_EXPECTED)))
             perf_xmpl_log_resulterr(thistest);

        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(add2result);
    } while(0);

    #endif


    #if(1)

    do  /* perf.report2() */
    {   objmask* represult = NULL;
        perf_xmpl_log_startmsg (thistest = "perf.report2 1");
        my_reset_notifyinfo();

        /* execute the remote report() call.
         * arguments objects are relinquished, result object is assumed.  
         * this service method has no return type, however c binding always   
         * returns an object shell in order to host a possible exception. 
         */
        represult  = remote->report2 (remote, new_date(), new_int32(18), new_stringw(L"x"));
 
        testresult = perf_xmpl_validate_response_object ((objmask*)represult, 0);
       
        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(represult);

    } while(0);

    #endif


    #if(1)

    do  /* perf.report2() negative - bad parameter type */
    {   objmask* represult = NULL;
        perf_xmpl_log_startmsg (thistest = "perf.report2 2");
        my_reset_notifyinfo();

        /* execute the remote report() call.
         * arguments objects are relinquished, result object is assumed.  
         * this service method has no return type, however c binding always   
         * returns an object shell in order to host a possible exception. 
         */
        represult = remote->report2 (remote, new_date(), new_int32(18), (etch_string*) new_int32(0));
 
        if (0 != (testresult = perf_xmpl_validate_response_object((objmask*)represult, EXCEPTION_EXPECTED)))
            perf_xmpl_log_resulterr(thistest);
       
        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(represult);

    } while(0);

    #endif


    #if(1)

    do  /* perf.report2() negative - bad data value */
    {   objmask* represult = NULL;
        perf_xmpl_log_startmsg (thistest = "perf.report2 3");
        my_reset_notifyinfo();

        /* execute the remote report() call.
         * arguments objects are relinquished, result object is assumed.  
         * this service method has no return type, however c binding always   
         * returns an object shell in order to host a possible exception. 
         */
        represult = remote->report2 (remote, new_date(), new_int32(18), new_stringw(FAKEBADDATA_STRING));
 
        if (0 != (testresult = perf_xmpl_validate_response_object((objmask*)represult, NOTIFY_EXPECTED)))
            perf_xmpl_log_resulterr(thistest);
       
        perf_xmpl_log_endmsg (thistest, testresult);
        ETCHOBJ_DESTROY(represult);

    } while(0);

    #endif


    /* we sleep briefly here since if the last message was one-way, we will have
     * not waited for a result, and if we now close the client socket before the 
     * server has had a chance to run the message, the server would detect the 
     * closed socket and shut down the client session, possibly prior to running
     * the message implementation, and that test would therefore be incomplete.
     * NOTE that if we are stepping though the server in the debugger, we should
     * set config.sleepSecondsPriorClientExit to a sufficient number of seconds  
     * such that the client socket will not close while doing so.
     */
    etch_sleep(500);
    perf_xmpl_wait_seconds (config.sleepSecondsPriorClientExit);  

    if (testerrors)
        etchlog(ETCHTEST, ETCHLOG_ERROR, "perf_xmpl_verify_server %d PERF TESTS FAILED\n", testerrors);
    else
        etchlog(ETCHTEST, ETCHLOG_INFO, "perf_xmpl_verify_server ALL PERF TESTS PASS\n");

    my_restore_client_session_notify (remote);
    return testerrors;
}


/* - - - - - - - - - - - - - - - - - - - - - 
 * call from main() to run all tests
 * - - - - - - - - - - - - - - - - - - - - - 
 */

/**
 * perf_xmpl_test_server()
 * run tests  
 */
int perf_xmpl_test_server (wchar_t* uri, new_client_funcptr user_ctor, const int waitms)
{  
    const int runtime = 60, trials = 3, is_full = FALSE;
    int i = 0, result = 0;
    int threads[] = { 1, 2 };
    /*  threads[] = { 1, 2, 3, 4, 5, 6, 7, 8, 16, 32 }; */
    const int numthreadcounts = sizeof(threads) / sizeof(int);

    /* since this test presumably runs after the standard generated code in [main],
     * display any memory leaks now before starting the test, such that leaks from 
     * the regular run can be isolated from any leaks occurring during this test, 
     * the latter shown with the normal cumulative leak display at etch shutdown. 
     */
    // check_etch_heap_bytes (TRUE, TRUE);

    etchlog(ETCHTEST, ETCHLOG_INFO, "start perf server test for %d iterations\n",
            numthreadcounts);
    
    for(i = 0; i < numthreadcounts; i++)
    {
        const int numthreads = threads[i];
        etchlog(ETCHTEST, ETCHLOG_INFO, "begin test iteration %d using %d threads ...\n",
                i+1, numthreads);

        #if(1)
        etchlog (ETCHTEST, ETCHLOG_INFO, "run perf add test ...\n");
        result = run_perftest ("add", uri, waitms, runtime, trials, numthreads, 
            perftest_threadproc_add, user_ctor);

        #endif

        #if(0)
        etchlog (ETCHTEST, ETCHLOG_INFO, "run perf sum test ...\n");
        result = run_perftest ("sum", uri, waitms, runtime, trials, numthreads, 
            perftest_threadproc_sum, user_ctor);
        #endif

        #if(0)
        etchlog (ETCHTEST, ETCHLOG_INFO, "run perf report test ...\n");
        result = run_perftest ("report", uri, waitms, runtime, trials, numthreads, 
            perftest_threadproc_report, user_ctor);
        #endif

        #if(0)
        etchlog (ETCHTEST, ETCHLOG_INFO, "run perf dist test ...\n");
        result = run_perftest ("dist", uri, waitms, runtime, trials, numthreads, 
            perftest_threadproc_dist, user_ctor);
        #endif

        #if(0)
        etchlog (ETCHTEST, ETCHLOG_INFO, "run perf add2 test ...\n");
        result = run_perftest ("add2", uri, waitms, runtime, trials, numthreads, 
            perftest_threadproc_add2, user_ctor);
        #endif

        #if(0)
        etchlog (ETCHTEST, ETCHLOG_INFO, "run perf report2 test ...\n");
        result = run_perftest ("report2", uri, waitms, runtime, trials, numthreads, 
            perftest_threadproc_report2, user_ctor);
        #endif
    }

    etchlog (ETCHTEST, ETCHLOG_INFO, "end perf server test\n");
    return 0;
}


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
 * xmpl_perf_remote_server.c
 * generated remote procedure calls.
 */

#include "apr_time.h" /* some apr must be included first */
#include "xmpl_perf_remote_server.h"
#include "etch_plainmboxmgr.h"
#include "etch_svcobj_masks.h"
#include "etch_global.h"
#include "etchexcp.h"
#include "etch_url.h" 
#include "etchlog.h" 

#include <tchar.h>
#include <stdio.h>
#include <conio.h>
char* ETCHREMS = "REMS";

i_mailbox*  perf_remote_add_begin (perf_remote_server*, etch_int32*, etch_int32*);
etch_int32* perf_remote_add_end   (perf_remote_server*, i_mailbox*);
etch_int32* perf_remote_add       (perf_remote_server*, etch_int32*, etch_int32*);
i_mailbox*  perf_remote_sum_begin (perf_remote_server*, etch_arraytype*);
etch_int32* perf_remote_sum_end   (perf_remote_server*, i_mailbox*);
etch_int32* perf_remote_sum       (perf_remote_server*, etch_arraytype*);
i_mailbox*  perf_remote_dist_begin(perf_remote_server*, perf_point*, perf_point*);
perf_point* perf_remote_dist_end  (perf_remote_server*, i_mailbox*);
perf_point* perf_remote_dist      (perf_remote_server*, perf_point*, perf_point*);
i_mailbox*  perf_remote_add2_begin(perf_remote_server*, etch_date* objdate, etch_int64*);
etch_date*  perf_remote_add2_end  (perf_remote_server*, i_mailbox*);
etch_date*  perf_remote_add2      (perf_remote_server*, etch_date* objdate, etch_int64*);
void* perf_remote_report  (perf_remote_server*, etch_int32*, etch_string*);
void* perf_remote_report2 (perf_remote_server*, etch_date*, etch_int32*, etch_string*);
int destroy_perf_remote_server (perf_remote_server*);


/* - - - - - - - - - -   
 * instantiation etc.
 * - - - - - - - - - -  
 */

/**
 * new_perf_remote_server()
 * perf_remote_server constructor.
 * @param thisx owner, optional, null in practice.
 * @param ids delivery service interface, caller retains.
 * @param vf service specific value factory, caller retains.
 */
perf_remote_server* new_perf_remote_server (void* thisx, i_delivery_service* ids, etch_value_factory* vf)
{
    perf_remote_server* rs = (perf_remote_server*) new_object (sizeof(perf_remote_server), 
        ETCHTYPEB_REMOTESERVER, get_dynamic_classid_unique(&CLASSID_PERFREMOTESERVER));

    rs->destroy = destroy_perf_remote_server;

    rs->remote_base = new_perf_remote (thisx, ids, vf, NULL);
    rs->remote_base->remote_type = ETCH_REMOTETYPE_SERVER;

    /* 2/3/09 now passing perf interface not owned by server_base */
    rs->server_base = new_perf_remote_server_base (thisx, rs->remote_base->iperf);  

    rs->vf = (perf_value_factory*) rs->remote_base->vf;

    rs->add  = perf_remote_add;
    rs->sum  = perf_remote_sum;
    rs->dist = perf_remote_dist;
    rs->add2 = perf_remote_add2;
    rs->report  = perf_remote_report;
    rs->report2 = perf_remote_report2;

    rs->async_begin_add = perf_remote_add_begin;
    rs->async_end_add   = perf_remote_add_end;
    rs->async_begin_sum = perf_remote_sum_begin;
    rs->async_end_sum   = perf_remote_sum_end;
    rs->async_begin_dist= perf_remote_dist_begin;
    rs->async_end_dist  = perf_remote_dist_end;
    rs->async_begin_add2= perf_remote_add2_begin;
    rs->async_end_add2  = perf_remote_add2_end;

    return rs;
}


/**
 * destroy_perf_server_base()
 * i_perf_server destructor.
 */
int destroy_perf_remote_server (perf_remote_server* thisx)
{
    if (NULL == thisx) return -1;
    if (thisx->refcount > 0 && --thisx->refcount > 0) return -1;  

    if (!is_etchobj_static_content(thisx))
    {    
        ETCHOBJ_DESTROY(thisx->remote_base);
        ETCHOBJ_DESTROY(thisx->server_base);
        ETCHOBJ_DESTROY(thisx->client_factory);
    }
            
    return destroy_objectex((objmask*)thisx);
}


/**
 * perf_remote_dispose_mailbox()
 * dispose of mailbox after use.
 * this is the common means of disposing of a mailbox when we're done with it.
 * this would intuitively be part of base class code but we don't have one.
 * @param thisx the remote server this.
 * @param pibox pointer to pointer to the mailbox interface, passed indirectly
 * such that this method can null out the caller's mailbox reference.
 * @return 0 if mailbox was successfullly closed, otherwise -1. 
 * caller's i_mailbox reference is nulled out regardless of result.
 */
int perf_remote_dispose_mailbox (perf_remote_server* thisx, i_mailbox** pibox)
{
    int result = 0;
    i_mailbox* ibox = 0;
    if (!pibox || !*pibox) return -1;
    ibox = *pibox;
    *pibox = NULL;  /* null out caller's reference */
    
    if (0 != (result = ibox->close_read (ibox)))
    {  
        /* we should not need this failsafe unregister if close_read() 
         * is reliable, since close_read() will do the unregister */
        i_mailbox_manager* imgr = ibox->manager(ibox);
        etchlog(ETCHREMS, ETCHLOG_ERROR, "could not close mailbox %x\n", ibox);
        if (imgr) result = imgr->unregister(imgr, ibox);
    }
 
    /* mailbox manager does not destroy the unregistered mailbox since it is    
     * owned by whoever registered it, that being us, so we destroy it here. 
     * debug heap issue note: this is/was the spot.
     */
    ibox->destroy(ibox);   
    return result;
}


/**
 * perf_remote_resultobj()
 * check result of remote call and throw exception if indicated.
 * @param obj the result object of the remote call.
 * @param result the result code.
 * @param idtext textual indicator of the message in progress.
 * @return a non-null objmask* based object possibly wrapping an exception.
 * @remarks using this method to return the result from all remote calls 
 * ensures both that a null object is never returned to the application,
 * and that an exception is always present for a bad result code. 
 */
void* perf_remote_resultobj (void* obj, int result, wchar_t* caller)
{
    objmask* resultobj = obj? obj: (objmask*) new_int32(result);

    /* if bad result and exception not already present, throw one */
    if ((result || !obj) && !is_exception(resultobj))
        etch_throw(resultobj, EXCPTYPE_INTERNALERR, caller, ETCHEXCP_COPYTEXT);

    return resultobj;
}


/**
 * perf_remote_get_stubbase()
 * convenience to return stub base object from remote server object.
 */
etch_stub* perf_remote_get_stubbase (perf_remote_server* rs)
{
    etch_stub* stub = NULL;
    xxxx_either_stub* perf_clistub = (xxxx_either_stub*) rs->client_factory->stub;
    stub = perf_clistub? perf_clistub->stub_base: NULL;
    return stub;
}


/**
 * perf_remote_set_session_notify()
 * convenience to override remote server's session_notify().
 * @return the session_notify function that was overridden.
 */
etch_session_notify perf_remote_set_session_notify 
    (perf_remote_server* rs, etch_session_notify newfunc)
{
    etch_session_notify oldfunc = NULL;
    etch_stub* stub = perf_remote_get_stubbase(rs);
    if (NULL == stub || NULL == stub->impl_callbacks) return NULL;
    oldfunc = stub->impl_callbacks->_session_notify;
    stub->impl_callbacks->_session_notify = newfunc;
    return oldfunc;
}


/**
 * perf_remote_set_session_control()
 * convenience to override remote server's session_control().
 * @return the session_control function that was overridden.
 */
etch_session_control perf_remote_set_session_control 
    (perf_remote_server* rs, etch_session_control newfunc)
{
    etch_session_control oldfunc = NULL;
    etch_stub* stub = perf_remote_get_stubbase(rs);
    if (NULL == stub || NULL == stub->impl_callbacks) return NULL;
    oldfunc = stub->impl_callbacks->_session_control;
    stub->impl_callbacks->_session_control = newfunc;
    return oldfunc;
}


/**
 * perf_remote_set_session_query()
 * convenience to override remote server's session_query().
 * @return the session_query function that was overridden.
 */
etch_session_query perf_remote_set_session_query 
    (perf_remote_server* rs, etch_session_query newfunc)
{
    etch_session_query oldfunc = NULL;
    etch_stub* stub = perf_remote_get_stubbase(rs);
    if (NULL == stub || NULL == stub->impl_callbacks) return NULL;
    oldfunc = stub->impl_callbacks->_session_query;
    stub->impl_callbacks->_session_query = newfunc;
    return oldfunc;
}


/* - - - - - - - - - - - - - - - - - - - -  
 * remote procedure call implementations
 * - - - - - - - - - - - - - - - - - - - - 
 */
 
/* - - - - - - - - - - -  
 * perf.add()
 * - - - - - - - - - - -  
 */

/**
 * perf_remote_add_begin()
 * perf.add async start 
 * @param thisx this.
 * @param x etch_int32* caller relinquishes.
 * @param y etch_int32* caller relinquishes.
 * @return mailbox to receive async result. caller owns it. it may be null.
 * @remarks note that we use the putc version of message.put(), wherein we pass
 * a *reference* to the value object's pointer, and message.putc() nulls out  
 * the reference. this permits us to break when a putc() error occurs, without  
 * leaking the un-put parameters, since prior to exit we can destroy each 
 * parameter which remains non-null.
 * @remarks note also that we don't bother to clone the value keys, since they  
 * are protected objects, and while message_put will appear to destroy them,
 * this will have no effect.
 */
i_mailbox* perf_remote_add_begin (perf_remote_server* thisx, etch_int32* x, etch_int32* y)
{
    int result = 0;
    i_mailbox* mbox = NULL;
    etch_message* msg = NULL;
    etch_type* msgtype = _g_perf_statics->_mt_etch_xmpl_perfserver_add;

    do
    {   msg = thisx->remote_base->new_message (thisx->remote_base, msgtype);
        if (!msg) break;

        result = message_putc (msg, _g_perf_statics->_mf_x, &x);
        if (0 != result) break;  /* see putc() comments above */
        result = message_putc (msg, _g_perf_statics->_mf_y, &y);
        if (0 != result) break;      

        /* fyi msg memory is relinquished here regardless of result */ 
        result = thisx->remote_base->begin_call (thisx->remote_base, msg, &mbox);
        msg = NULL;

    } while(0);

    /* destroy any unrelinquished objects */
    ETCHOBJ_DESTROY(x); 
    ETCHOBJ_DESTROY(y);
    ETCHOBJ_DESTROY(msg); 
    return mbox;
}


/**
 * perf_remote_add_end()
 * add async end (read result from mailbox and return result)
 * @param thisx this.
 * @param mbox caller relinquishes
 * @return etch_int32* result of add, caller owns.
 */
etch_int32* perf_remote_add_end (perf_remote_server* thisx, i_mailbox* ibox)
{
    etch_int32* resobj = NULL;
    etch_type* restype = _g_perf_statics->_mt_etch_xmpl_perfclient_result_add;

    const int result = thisx->remote_base->end_call 
        (thisx->remote_base, ibox, restype, &resobj);

    return resobj;
}


/**
 * perf_remote_add()
 * perf.add remote method call.
 * instantiates a mailbox, sends perf.add message, waits for result to arrive
 * in mailbox, disposes mailbox, and returns object containing add() result.
 * @param thisx this.
 * @param x etch_int32* caller relinquishes.
 * @param y etch_int32* caller relinquishes.
 * @return etch_int32* result of add, caller owns.
 */
etch_int32* perf_remote_add (perf_remote_server* thisx, etch_int32* x, etch_int32* y)
{
    int result = -1;
    etch_int32* resultobj = NULL;

    i_mailbox* mbox = perf_remote_add_begin (thisx, x, y);
    
    if (mbox)
    {   // TESTING NOTE we can comment out the add_end() to no ill effect
        resultobj = perf_remote_add_end (thisx, mbox);  
    
        result = perf_remote_dispose_mailbox (thisx, &mbox);   
    }

    return perf_remote_resultobj (resultobj, result, L"perf.add");
}

 
/* - - - - - - - - - - -  
 * perf.sum()
 * - - - - - - - - - - -  
 */

/**
 * perf_remote_sum_begin()
 * sum async start 
 * @param thisx this.
 * @param an etch_nativearray or etch_arrayvalue, caller relinquishes.
 * @return a mailbox, caller owns it. may be null.
 */
i_mailbox* perf_remote_sum_begin (perf_remote_server* thisx, etch_arraytype* a)
{
    int result = 0;
    i_mailbox* mbox = NULL;
    etch_message* msg = NULL;
    etch_type* msgtype = _g_perf_statics->_mt_etch_xmpl_perfserver_sum;

    do
    {   msg = thisx->remote_base->new_message (thisx->remote_base, msgtype);
        if (!msg) break;

        result = message_putc (msg, _g_perf_statics->_mf_values, &a);
        if (0 != result) break;  /* see earlier putc() comments */
    
        /* fyi msg memory is relinquished here regardless of result */ 
        result = thisx->remote_base->begin_call (thisx->remote_base, msg, &mbox);
        msg = NULL;

    } while(0);

    /* destroy any unrelinquished objects */
    ETCHOBJ_DESTROY(a); 
    ETCHOBJ_DESTROY(msg); 
    return mbox;
}


/**
 * perf_remote_sum_end()
 * sum async end (read result from mailbox and return result)
 * @param thisx this.
 * @param mbox caller relinquishes
 * @return etch_int32* result of sum, caller owns.
 */
etch_int32* perf_remote_sum_end (perf_remote_server* thisx, i_mailbox* mbox)
{
    etch_int32* resobj = NULL;
    etch_type*  restype = _g_perf_statics->_mt_etch_xmpl_perfclient_result_add;

    const int result = thisx->remote_base->end_call 
        (thisx->remote_base, mbox, restype, &resobj);
     
    return resobj;
}


/**
 * perf_remote_sum()
 * perf.sum remote method call.
 * instantiates a mailbox, sends perf.sum message, waits for result to arrive
 * in mailbox, disposes mailbox, and returns object containing sum() result.
 * @param thisx this.
 * @param a an etch_nativearray or etch_arrayvalue, caller relinquishes.
 * @return etch_int32* result of sum, caller owns.
 */
etch_int32* perf_remote_sum (perf_remote_server* thisx, etch_arraytype* a)
{
    etch_int32* resultobj = NULL;

    i_mailbox* mbox = perf_remote_sum_begin (thisx, a);
    
    if (mbox) 
    {   resultobj = perf_remote_sum_end (thisx, mbox);  
    
        perf_remote_dispose_mailbox (thisx, &mbox);
    }

    return perf_remote_resultobj (resultobj, 0, L"perf.sum");
}

 
/* - - - - - - - - - - -  
 * perf.report()
 * - - - - - - - - - - -  
 */

/**
 * perf_remote_report_begin()
 * report() execution (one-way, no async result ergo no mailbox)
 * @param thisx this.
 * @param code an etch integer, caller relinquishes.
 * @param x etch_string caller relinquishes.
 * @return etch_int32 wrapping the remote send result code, and if that result  
 * was non-zero, also wrapping an exception object. 
 */
void* perf_remote_report_begin (perf_remote_server* thisx, etch_int32* code, etch_string* x)
{
    int result = 0;
    etch_message* msg = NULL;
    etch_int32* resultobj = NULL;
    etch_type* msgtype = _g_perf_statics->_mt_etch_xmpl_perfserver_report;

    do
    {   msg = thisx->remote_base->new_message (thisx->remote_base, msgtype);
        if (!msg) break;

        result = message_putc (msg, _g_perf_statics->_mf_code, &code);
        if (0 != result) break;  /* see earlier putc() comments */
        result = message_putc (msg, builtins._mf_msg, &x);
        if (0 != result) break; 

        /* fyi msg memory is relinquished here regardless of result */ 
        resultobj = thisx->remote_base->sendex (thisx->remote_base, msg);
        msg = NULL;
      
    } while(0);

    /* destroy any unrelinquished objects */
    ETCHOBJ_DESTROY(code); 
    ETCHOBJ_DESTROY(x); 
    ETCHOBJ_DESTROY(msg); 
    return resultobj;
}


/**
 * perf_remote_report_end()
 * async result handler not used since perf.report is a one-way send-and-forget method.
 */
void* perf_remote_report_end (perf_remote_server* thisx, i_mailbox* mbox)
{
    return NULL;
}


/**
 * perf_remote_report()
 * perf.report remote 1-way method call.
 * sends perf.report message and does not wait for a result. if server implementation
 * code throws an exception, the exception arrives via session_notify override if any.
 * @param code an integer object, caller relinquishes.
 * @param x etch_string caller relinquishes.
 * @return integer transport result code cast to void*.
 */
void* perf_remote_report (perf_remote_server* thisx, etch_int32* code, etch_string* x)
{
    /* perf.report is a one-way, send-and-forget message so we only do a begin call */
    void* resultobj = perf_remote_report_begin (thisx, code, x);
    
    return perf_remote_resultobj (resultobj, 0, L"perf.report");
}


/* - - - - - - - - - - -  
 * perf.dist()
 * - - - - - - - - - - -  
 */

/**
 * perf_remote_dist_begin()
 * dist async start 
 * @param thisx this.
 * @param p1 etch_point* caller relinquishes.
 * @param p2 etch_point* caller relinquishes.
 * @return mailbox to receive async result. caller owns it. may be null.
 */
i_mailbox* perf_remote_dist_begin (perf_remote_server* thisx, perf_point* p1, perf_point* p2)
{
    i_mailbox* mbox = NULL;
    etch_message* msg = NULL;
    int result = -1, is_argerr = !p1 || !p2;
    etch_type* msgtype = _g_perf_statics->_mt_etch_xmpl_perfserver_dist;

    do
    {   msg = thisx->remote_base->new_message (thisx->remote_base, msgtype);
        if (!msg) break;

        result = message_putc (msg, _g_perf_statics->_mf_a, &p1);
        if (0 != result) break; /* see earlier putc() comments */
        result = message_putc (msg, _g_perf_statics->_mf_b, &p2);
        if (0 != result) break;       

        /* fyi msg memory is relinquished here regardless of result */ 
        result = thisx->remote_base->begin_call (thisx->remote_base, msg, &mbox);
        msg = NULL;

    } while(0);

    /* destroy any unrelinquished objects */
    ETCHOBJ_DESTROY(p1);
    ETCHOBJ_DESTROY(p2);
    ETCHOBJ_DESTROY(msg);
    return mbox;
}


/**
 * perf_remote_dist_end()
 * dist async end (read result from mailbox and return result)
 * @param thisx this.
 * @param mbox caller relinquishes
 * @return etch_point* result of dist, caller owns.
 */
perf_point* perf_remote_dist_end (perf_remote_server* thisx, i_mailbox* mbox)
{
    perf_point* resobj = NULL;
    etch_type*  restype = _g_perf_statics->_mt_etch_xmpl_perfclient_result_dist;

    const int result = thisx->remote_base->end_call 
        (thisx->remote_base, mbox, restype, &resobj);

    return resobj;
}


/**
 * perf_remote_dist()
 * perf.dist remote method call.
 * instantiates a mailbox, sends perf.dist message, waits for result to arrive
 * in mailbox, disposes mailbox, and returns object containing dist() result.
 * @param thisx this.
 * @param x etch_int32* caller relinquishes.
 * @param y etch_int32* caller relinquishes.
 * @return etch_int32* result of dist, caller owns.
 */
perf_point* perf_remote_dist (perf_remote_server* thisx, perf_point* p1, perf_point* p2)
{
    perf_point* resultobj = NULL;
    int result = -1;

    i_mailbox* mbox = perf_remote_dist_begin (thisx, p1, p2);
    
    if (mbox) 
    {   resultobj = perf_remote_dist_end (thisx, mbox);  
    
        result = perf_remote_dispose_mailbox (thisx, &mbox);
    }

    return perf_remote_resultobj (resultobj, result, L"perf.dist");
}


/* - - - - - - - - - - -  
 * perf.add2()
 * - - - - - - - - - - -  
 */

/**
 * perf_remote_add2_begin()
 * add2 async start 
 * @param thisx this.
 * @param objdate etch_date* caller relinquishes.
 * @param objms etch_int64* caller relinquishes.
 * @return mailbox to receive async result. caller owns it. may be null.
 */
i_mailbox* perf_remote_add2_begin (perf_remote_server* thisx, etch_date* objdate, etch_int64* objms)
{
    int result = 0;
    i_mailbox* mbox = NULL;
    etch_message* msg = NULL;
    etch_type* msgtype = _g_perf_statics->_mt_etch_xmpl_perfserver_add2;

    do
    {   msg = thisx->remote_base->new_message (thisx->remote_base, msgtype);
        if (!msg) break;

        result = message_putc (msg, _g_perf_statics->_mf_ts, &objdate);
        if (0 != result) break;  /* see earlier putc() comments */
        result = message_putc (msg, _g_perf_statics->_mf_ms, &objms);
        if (0 != result) break;        

        /* fyi msg memory is relinquished here regardless of result */ 
        result = thisx->remote_base->begin_call (thisx->remote_base, msg, &mbox);
        msg = NULL;

    } while(0);

    /* destroy any unrelinquished objects */
    ETCHOBJ_DESTROY(objdate);
    ETCHOBJ_DESTROY(objms); 
    ETCHOBJ_DESTROY(msg);
    return mbox;
}


/**
 * perf_remote_add2_end()
 * add2 async end (read result from mailbox and return result)
 * @param thisx this.
 * @param mbox caller relinquishes
 * @return etch_point* result of add2, caller owns.
 */
etch_date* perf_remote_add2_end (perf_remote_server* thisx, i_mailbox* mbox)
{
    int result = 0;
    etch_date* resobj = NULL;
    etch_type* restype = _g_perf_statics->_mt_etch_xmpl_perfclient_result_add2;

    result = thisx->remote_base->end_call (thisx->remote_base, mbox, restype, &resobj);

    return resobj;
}


/**
 * perf_remote_add2()
 * perf.add2 remote method call.
 * instantiates a mailbox, sends perf.add2 message, waits for result to arrive
 * in mailbox, disposes mailbox, and returns object containing add2() result.
 * @param thisx this.
 * @param objdate etch_date* caller relinquishes.
 * @param objms etch_int64* caller relinquishes.
 * @return etch_date* result of add2, caller owns.
 */
etch_date* perf_remote_add2 (perf_remote_server* thisx, etch_date* objdate, etch_int64* objms)
{
    etch_date* resultobj = NULL;
    int result = -1;

    i_mailbox* mbox = perf_remote_add2_begin (thisx, objdate, objms);
    
    if (mbox)  
    {
        resultobj = perf_remote_add2_end (thisx, mbox);  
    
        result = perf_remote_dispose_mailbox (thisx, &mbox);
    }

    return perf_remote_resultobj (resultobj, result, L"perf.add2");
}

 
/* - - - - - - - - - - -  
 * perf.report2()
 * - - - - - - - - - - -  
 */

/**
 * perf_remote_report2_begin()
 * report2() execution (one-way, no async result ergo no mailbox)
 * @param thisx this.
 * @param ts etch_date*, caller relinquishes.
 * @param n an integer object, caller relinquishes.
 * @param x etch_string caller relinquishes.
 * @return etch_int32 wrapping the remote send result code, and if that result  
 * was non-zero, also wrapping an exception object. 
 */
void* perf_remote_report2_begin (perf_remote_server* thisx, etch_date* ts, etch_int32* n, etch_string* x)
{
    int result = 0;
    etch_message* msg = NULL;
    etch_int32* resultobj = NULL;
    etch_type* msgtype = _g_perf_statics->_mt_etch_xmpl_perfserver_report2;

    do
    {   msg = thisx->remote_base->new_message (thisx->remote_base, msgtype);
        if (!msg) break;

        result = message_putc (msg, _g_perf_statics->_mf_ts, &ts);
        if (0 != result) break;  /* see earlier putc() comments */
        result = message_putc (msg, _g_perf_statics->_mf_code, &n);
        if (0 != result) break;  
        result = message_putc (msg, builtins._mf_msg, &x);
        if (0 != result) break; 

        /* msg memory is relinquished here regardless of result */ 
        resultobj = thisx->remote_base->sendex (thisx->remote_base, msg);
        msg = NULL;

    } while(0);

    /* destroy any unrelinquished objects */
    ETCHOBJ_DESTROY(ts); 
    ETCHOBJ_DESTROY(n); 
    ETCHOBJ_DESTROY(x); 
    ETCHOBJ_DESTROY(msg);
    return resultobj;
}


/**
 * perf_remote_report2_end()
 * not used.
 */
void* perf_remote_report2_end (perf_remote_server* thisx, i_mailbox* mbox)
{
    return NULL;
}


/**
 * perf_remote_report2()
 * perf.report2 remote 1-way method call.
 * sends perf.report2 message and does not wait for a result. if server implementation
 * code throws an exception, the exception arrives via session_notify override if any.  
 * @param ts etch_date*, caller relinquishes
 * @param n an integer object, caller relinquishes.
 * @param x etch_string caller relinquishes.
 * @return integer transport result code cast to void*.
 */
void* perf_remote_report2 (perf_remote_server* thisx, etch_date* ts, etch_int32* n, etch_string* x)
{
    /* perf.report2 is a one-way, send-and-forget message so we only do a begin call */
    void* resultobj = perf_remote_report2_begin (thisx, ts, n, x);
    
    return perf_remote_resultobj (resultobj, 0, L"perf.report2");
}

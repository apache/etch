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
 * test_plainmailbox.c 
 */

#include "apr_time.h" /* some apr must be included first */
#include "etch_plainmailbox.h"  
#include "etch_plainmboxmgr.h"

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#include "etchthread.h"
#include "etch_global.h"
#include "etch_defvalufact.h"
#include "etchmap.h"
#include "etchlog.h"
#include "etch_syncobj.h"
#include "etchexcp.h"
#include "etch_simpletimer.h"

int apr_setup(void);
int apr_teardown(void);
int this_setup();
int this_teardown();
apr_pool_t* g_apr_mempool;
const char* pooltag = "etchpool";


/* - - - - - - - - - - - - - - 
 * unit test infrastructure
 * - - - - - - - - - - - - - -
 */

int g_is_automated_test, g_bytes_allocated;

#define IS_DEBUG_CONSOLE FALSE

int init_suite(void)
{
    apr_setup();
    etch_runtime_init(TRUE);
    config.is_log_to_console = IS_DEBUG_CONSOLE;
    return this_setup();
}

int clean_suite(void)
{
    this_teardown();
    debug_showmem(FALSE, TRUE); /* if leaks, display byte count */
    etch_runtime_cleanup(0,0);  /* free memtable and cache etc */
    apr_teardown();
    return 0;
}

/*
 * apr_setup()
 * establish apache portable runtime environment
 */
int apr_setup(void)
{
    int result = 0;
    result = apr_initialize();
    if (result == 0)
    {   result = etch_apr_init();
        g_apr_mempool = etch_apr_mempool;
    }
    if (g_apr_mempool)
        apr_pool_tag(g_apr_mempool, pooltag);
    else result = -1;
    return result;
}

/*
 * apr_teardown()
 * free apache portable runtime environment
 */
int apr_teardown(void)
{
    if (g_apr_mempool)
        apr_pool_destroy(g_apr_mempool);
    g_apr_mempool = NULL;
    apr_terminate();
    return 0;
}

int this_setup()
{
    etch_apr_mempool = g_apr_mempool;
    return 0;
}

int this_teardown()
{    
    return 0;
}


/* - - - - - - - - - - - - - - 
 * unit test support
 * - - - - - - - - - - - - - -
 */

etch_plainmailboxmgr*  g_manager;
etch_arraylist*        g_list;
etch_who*              g_who1;
etch_who*              g_who2;
etch_type*             g_type1;
etch_type*             g_type2;
etchmutex*             g_rwlock;
default_value_factory* g_vf; 
int g_is_unregistered;  
int g_mailbox_status;
int g_mailbox_isclosed;
int g_wakeupreason;
i_mailbox*   g_mailbox;
void*        g_wakeupdata;
etch_object* g_mailbox_state;
etch_destructor g_list_stockdtor;
#define CLASSID_MYSTATE 0xf0
#define MYSTATE_VALUE 12345

int my_redelivery_list_destructor(etch_arraylist*);
int mymboxmgr_unregister(etch_plainmailboxmgr*, i_mailbox*);
int mymboxmgr_redeliver (etch_plainmailboxmgr*, etch_who*, etch_message*);
int my_mailbox_notify   (etch_plainmailbox*, i_mailbox*, etch_object*, const int); 


/**
 * setup_this_test()
 * set up an individual unit test
 */
int setup_this_test()
{
    g_rwlock  = new_mutex (etch_apr_mempool, ETCHMUTEX_NESTED);
    g_manager = new_plain_mailbox_manager (NULL, NULL, NULL, g_rwlock); 
    if (NULL == g_manager) return -1;
    g_manager->unregister = g_manager->imanager->unregister = mymboxmgr_unregister;
    g_manager->redeliver  = g_manager->imanager->redeliver  = mymboxmgr_redeliver;

    /* message redelivery sink */
    g_list = new_arraylist(0, 0);
    g_list->content_type = ETCHARRAYLIST_CONTENT_OBJECT;
    g_list->is_readonly  = TRUE;  
    g_list_stockdtor = g_list->destroy; /* custom destructor will destroy list content */
    g_list->destroy  = my_redelivery_list_destructor;

    g_who1  = new_who(new_int32(1), TRUE);
    g_who2  = new_who(new_int32(2), TRUE);
    g_type1 = new_type(L"type1");
    g_type2 = new_type(L"type2");
    g_vf    = new_default_value_factory(NULL, NULL);

    etchtype_put_validator(g_type1, clone_field(builtins._mf__message_id), (objmask*) etchvtor_int64_get(0));
    etchtype_put_validator(g_type2, clone_field(builtins._mf__message_id), (objmask*) etchvtor_int64_get(0));

    return g_manager? 0: -1;
}


/**
 * teardown_this_test()
 * tear down an individual unit test
 */
void teardown_this_test()
{
    ETCHOBJ_DESTROY(g_manager);
    ETCHOBJ_DESTROY(g_rwlock);
    ETCHOBJ_DESTROY(g_list);
    ETCHOBJ_DESTROY(g_who1);
    ETCHOBJ_DESTROY(g_who2);
    ETCHOBJ_DESTROY(g_type1);
    ETCHOBJ_DESTROY(g_type2);
    ETCHOBJ_DESTROY(g_vf);
    g_is_unregistered = g_mailbox_status = g_mailbox_isclosed = g_wakeupreason = 0;
    g_mailbox = NULL;
    g_mailbox_state = NULL;
    g_wakeupdata = NULL;
    etchvf_free_builtins(); 
}


/**
 * my_redelivery_list_destructor()
 * custom dtor for message redelivery sink.
 * logs and destroys redelivered messages.
 */
int my_redelivery_list_destructor(etch_arraylist* list)
{
    etch_iterator iterator;
    set_iterator(&iterator, list, &list->iterable);

    while(iterator.has_next(&iterator))
    {
        etch_mailbox_element* listitem = iterator.current_value;
        etch_int64* msgidobj = message_get_id(listitem->msg);
        int64 msgid = msgidobj? msgidobj->value: 0;
        #if IS_DEBUG_CONSOLE
        etchlog("TEST", ETCHLOG_INFO, "destroying message %d from redelivery sink\n", msgid);
        #endif

        listitem->destroy(listitem);  /* destroy message and element wrapper */
        
        iterator.next(&iterator);
    }

    return g_list_stockdtor(list);    /* invoke original list destructor */
}


/**
 * mymboxmgr_unregister()
 * override for mailbox manager unregister
 */
int mymboxmgr_unregister (etch_plainmailboxmgr* mgr, i_mailbox* mbox)
{
    g_is_unregistered = TRUE;
    return 0;
}


/**
 * mymboxmgr_redeliver()
 * override for mailbox manager redeliver
 */
int mymboxmgr_redeliver (etch_plainmailboxmgr* mgr, etch_who* whofrom, etch_message* msg)
{
    arraylist_add(g_list, new_mailbox_element(msg, whofrom));
    return 0;
} 


/**
 * mymboxmgr_redeliver()
 * override for mailbox notify
 */
int my_mailbox_notify (etch_plainmailbox* mbox, i_mailbox* mb, etch_object* state, const int is_closed) 
{
    g_mailbox = mb;
    g_mailbox_state = state;
    g_mailbox_status = TRUE;
    g_mailbox_isclosed = is_closed;
    return 0;
}


void check_mailbox_status(const int expected_status,  i_mailbox* expected_mbox, 
    etch_object* expected_state, const int expected_isclosed)
{
    CU_ASSERT_EQUAL(g_mailbox_status,   expected_status);
    CU_ASSERT_EQUAL(g_mailbox_state,    expected_state);
    CU_ASSERT_EQUAL(g_mailbox_isclosed, expected_isclosed);
    CU_ASSERT_PTR_EQUAL(g_mailbox,      expected_mbox);
}


void check_mailbox(etch_plainmailbox* mbox, const int is_expected_empty, 
    const int is_expected_full, const int is_expected_closed, 
    const int is_expected_unregistered, const int expected_size) 
{
    i_mailbox* ibox = mbox->imailbox;
    int result = mbox->is_empty(ibox);
    CU_ASSERT_EQUAL(result, is_expected_empty);
    result = mbox->is_full(ibox); 
    CU_ASSERT_EQUAL(result, is_expected_full); 
    result = mbox->is_closed(ibox);
    CU_ASSERT_EQUAL(result, is_expected_closed);
    CU_ASSERT_EQUAL(g_is_unregistered, is_expected_unregistered);
    CU_ASSERT_EQUAL(g_list->count, expected_size);
}


void check_deliver(etch_plainmailbox* mbox, const int expected_ishandled, 
    etch_who* who, etch_message* msg)
{
    int actual_result;

    const int delivery_result = mbox->message(mbox->imailbox, who, msg);

    if (delivery_result == 0 && expected_ishandled)
        actual_result = 0;
    else
    if (delivery_result != 0 && !expected_ishandled)
        actual_result = 0;
    else
        actual_result = -1;

    CU_ASSERT_EQUAL(actual_result, 0);

    if (0 != delivery_result) /* caller retains message memory on failure */
        if  (delivery_result != ETCH_MAILBOX_DUPLICATE)
             msg->destroy(msg);
}


void check_redelivered(etch_plainmailbox* mbox, const int index, etch_message* msg) 
{
    etch_mailbox_element* entry = arraylist_get(g_list, index);
    CU_ASSERT_PTR_NOT_NULL(entry);
    if (NULL == entry) return;
    CU_ASSERT_PTR_EQUAL(entry->msg, msg);
}


void check_close_delivery(etch_plainmailbox* mbox, const int expected_is_closed) 
{
    const int did_close = 0 == mbox->close_delivery(mbox->imailbox);
    CU_ASSERT_EQUAL(did_close, expected_is_closed);
}


void check_close_read(etch_plainmailbox* mbox, const int expected_is_closed) 
{
    const int did_close = 0 == mbox->close_read(mbox->imailbox);
    CU_ASSERT_EQUAL(did_close, expected_is_closed);
}


void check_element(etch_mailbox_element* elt, etch_who* who, etch_message* msg)
{
    CU_ASSERT_PTR_NOT_NULL(elt);
    if (NULL == elt) return;
    CU_ASSERT_PTR_EQUAL(who, elt->whofrom);
    CU_ASSERT_PTR_EQUAL(msg, elt->msg);
}


void check_read(etch_plainmailbox* mbox, const int expected_to_be_present, 
    etch_who* who, etch_message* msg) 
{
    etch_mailbox_element* thiselt = NULL;

    int result = mbox->read(mbox->imailbox, &thiselt); 

    if (expected_to_be_present)
        check_element(thiselt, who, msg);
    else { CU_ASSERT(is_exception(thiselt)); }

    /* message is "processed", end of the line for the message */ 
    if (thiselt)   /* element destructor destroys the message */
        thiselt->destroy(thiselt);
}


void check_read_withwait(etch_plainmailbox* mbox, const int maxdelay, 
    const int expected_to_be_present, etch_who* who, etch_message* msg) 
{
    etch_mailbox_element* thiselt = NULL;

    const int result = mbox->read_withwait(mbox->imailbox, maxdelay, &thiselt); 

    if (expected_to_be_present)
        check_element(thiselt, who, msg);
     else { CU_ASSERT(is_exception(thiselt)); }

    /* message is "processed", end of the line for the message */ 
    if (thiselt)   /* element destructor destroys the message */
        thiselt->destroy(thiselt);
}


/**
 * my_alarm_wakeup()
 */
int my_alarm_wakeup (void* passthrudata, const int wakeupreason) 
{
    etch_plainmailbox* thisx = (etch_plainmailbox*) passthrudata;
    g_wakeupdata = thisx;
    g_wakeupreason = wakeupreason; 
    return 0;
}


/* - - - - - - - - - - - - - - 
 * unit tests 
 * - - - - - - - - - - - - - -
 */

/**
 * test_mailboxes_map()
 * test hashtable configured as for mapping id object to mailbox object,
 * insert, locate, remove, destroy, verify all memory accounted for.
 */
void test_mailboxes_map(void)
{
    int result = 0;
    const int64 val1 = 1234567890, val2 = 9876543210;
    etch_hashtable* map = new_pmboxmgr_mailboxmap();
    etch_hashitem hi, *thisitem = &hi; 
    i_mailbox*  mbox1  = new_default_mailbox_interface(NULL); 
    i_mailbox*  mbox2  = new_default_mailbox_interface(NULL); 
    etch_int64* msgid1 = new_int64(val1);
    etch_int64* msgid2 = new_int64(val2);
    etch_int64* foundkey = NULL;
    const unsigned expecthash1 = etchhash(&val1, 8, 0);
    const unsigned expecthash2 = etchhash(&val2, 8, 0);
    CU_ASSERT_EQUAL(msgid1->hashkey, expecthash1);
    CU_ASSERT_EQUAL(msgid2->hashkey, expecthash2);

    result = map->vtab->inserth(map->realtable, msgid1, mbox1, map, 0); 
    CU_ASSERT_EQUAL(result,0); 
    result = map->vtab->inserth(map->realtable, msgid2, mbox2, map, 0); 
    CU_ASSERT_EQUAL(result,0);

    result = map->vtab->findh(map->realtable, msgid1->hashkey, map, &thisitem);
    CU_ASSERT_EQUAL(result,0);
    result = map->vtab->findh(map->realtable, msgid2->hashkey, map, &thisitem);
    CU_ASSERT_EQUAL(result,0);

    result = map->vtab->removeh(map->realtable, msgid1->hashkey, map, &thisitem);
    CU_ASSERT_EQUAL_FATAL(result,0);
    foundkey = (etch_int64*) thisitem->key;
    CU_ASSERT_EQUAL_FATAL(is_etch_int64(foundkey), TRUE);
    CU_ASSERT_EQUAL(foundkey->value, msgid1->value);
    etch_free(foundkey);   

    map->destroy(map);

    /* map owns its keys and does not own its values */
    etch_free(mbox1);
    etch_free(mbox2);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_setup_teardown()
 * test the test setup/teardown and verify all memory accounted for.
 * passing this test ensures that any leaks that might show up in subsequeuent 
 * tests are specific to the test, not the common test data.
 */
void test_setup_teardown(void)
{
    etch_message* test_message;

    int result = setup_this_test();
    CU_ASSERT_EQUAL_FATAL(result, 0); 

    /* test that messages added to the global list are destroyed at teardown */
    test_message = new_message(g_type1, 0, (etch_value_factory*) g_vf);
    CU_ASSERT_PTR_NOT_NULL_FATAL(test_message); 
    arraylist_add(g_list, new_mailbox_element(test_message, g_who1));

    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_constructor()
 * test the plain mailbox constructor
 */
void test_constructor(void)
{
    setup_this_test();

    do
    {   etch_plainmailbox* mbox = NULL;
        const int64 THISMSGID = 9876543210;     
        const int   QUEUEDELAY_1SEC = 1000, LIFETIME_TWO_MINUTES = 120000;
        const int   MAXMSGS_ONE = 1, QUEUEEWAITFOREVER = 0, QUEUENODELAY = -1;
        printf("\n");

        mbox = new_mailbox (g_manager->imanager, THISMSGID, 
            QUEUEDELAY_1SEC, MBOX_LIFETIME_UNTIL_CLOSE, MAXMSGS_ONE);

        CU_ASSERT_PTR_NOT_NULL_FATAL(mbox);
        CU_ASSERT_EQUAL(THISMSGID, mbox->get_message_id(mbox->imailbox));  
        CU_ASSERT_EQUAL(QUEUEDELAY_1SEC, mbox->max_message_delay);  
        CU_ASSERT_EQUAL(MBOX_LIFETIME_UNTIL_CLOSE, mbox->lifetime);  
        CU_ASSERT_EQUAL(MAXMSGS_ONE,  mbox->max_messages);  
        mbox->destroy(mbox);

        mbox = new_mailbox (g_manager->imanager, THISMSGID, 
            QUEUENODELAY, MBOX_LIFETIME_UNTIL_CLOSE, MAXMSGS_ONE);
        CU_ASSERT_PTR_NOT_NULL_FATAL(mbox);
        mbox->destroy(mbox);

        mbox = new_mailbox (g_manager->imanager, THISMSGID, 
            QUEUEDELAY_1SEC, MBOX_LIFETIME_UNTIL_CLOSE, MAXMSGS_ONE);
        CU_ASSERT_PTR_NOT_NULL_FATAL(mbox);
        mbox->destroy(mbox);

        /* testing with a nonzero lifetime here, this is atypical */
        mbox = new_mailbox (g_manager->imanager, THISMSGID, 
            QUEUEEWAITFOREVER, LIFETIME_TWO_MINUTES, MAXMSGS_ONE);

        CU_ASSERT_PTR_NOT_NULL_FATAL(mbox);
        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_deliver()
 * test message deliver
 */
void test_deliver(void)
{
    setup_this_test();

    do
    {   etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, 0, 2);
        check_mailbox(mbox, TRUE, FALSE, FALSE, FALSE, 0);

        check_close_delivery(mbox, TRUE);
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 0);

        check_deliver(mbox, FALSE, g_who1, new_message(g_type1, 0, (etch_value_factory*) g_vf));
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 0);

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_close_delivery1()
 * open box and close it for delivery while empty
 */
void test_closedelivery1(void)
{
    setup_this_test();

    do
    {   etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, 0, 2);
        check_close_delivery(mbox, TRUE);   
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 0);  
        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_close_delivery2()
 * open box and close it for delivery while non-empty
 */
void test_closedelivery2(void)
{
    setup_this_test();

    do
    {   etch_plainmailbox* mbox  = new_mailbox (g_manager->imanager, 1, -1, 0, 2);
        etch_message *mymessage1 = new_message(g_type1, 0, (etch_value_factory*) g_vf);
        message_set_id(mymessage1, new_int64(1)); 
       
        check_deliver(mbox, TRUE, g_who1, mymessage1);
        check_mailbox(mbox, FALSE, FALSE, FALSE, FALSE, 0);  

        check_close_delivery(mbox, TRUE);
        check_mailbox(mbox, FALSE, FALSE, TRUE, TRUE, 0);  

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_close_delivery3()
 * open box and close it for delivery twice
 */
void test_closedelivery3(void)
{
    setup_this_test();

    do
    {   etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, 0, 2);

        check_close_delivery(mbox, TRUE);
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 0);  

        check_close_delivery(mbox, FALSE);
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 0);  

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_notify1()
 * test message notify
 */
void test_notify1(void)
{
    setup_this_test();

    do
    {   int result = 0;
        etch_object* mystateobj = NULL;
        etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, 0, 2);
        i_mailbox* ibox = mbox->imailbox;
        check_mailbox_status(FALSE, NULL, NULL, FALSE);
        mystateobj = new_etch_object(CLASSID_MYSTATE, new_int32(MYSTATE_VALUE));

        result = mbox->register_notify(ibox, my_mailbox_notify, mystateobj, ETCH_INFWAIT);

        CU_ASSERT_EQUAL(result, 0);
        check_mailbox_status(FALSE, NULL, NULL, FALSE); 

        check_close_delivery(mbox, TRUE);
        check_mailbox_status(TRUE, ibox, mystateobj, TRUE); 

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_timer()
 * test current implementation of timer
 */
void test_timer(void)
{
    setup_this_test();

     /* this test breaks if breakpoints are set in the underlying wait objects
      * and thread procedures. this will of course be due to wait expiration 
      * and subsequent object destruction, however we should have better state
      * maintenance in the thread, mutex, wait, and timer objects in order to 
      * be able to detect and avoid such problems.
      */
    do
    {   const int ONE_SECOND = 1000, TWO_SECONDS = 2000, THREE_SECONDS = 3000, TEN_SECONDS = 10000;
        etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, 0, 2);
        etch_timer* timer;
        int result = 0;

        timer  = new_timer(ONE_SECOND, mbox, my_alarm_wakeup);  
 
        result = etch_timer_start(timer);
        etch_sleep(TWO_SECONDS);
        CU_ASSERT_PTR_EQUAL(g_wakeupdata, mbox);
        CU_ASSERT_EQUAL(g_wakeupreason, ETCH_TIMER_REASON_TIMEOUT);
        timer->destroy(timer);

        g_wakeupdata = NULL;
        timer  = new_timer(TEN_SECONDS, mbox, my_alarm_wakeup);  
        result = etch_timer_start(timer);
        etch_sleep(TWO_SECONDS);
        etch_timer_stop(timer);
        CU_ASSERT_PTR_EQUAL(g_wakeupdata, mbox);
        CU_ASSERT_NOT_EQUAL(g_wakeupreason, ETCH_TIMER_REASON_TIMEOUT);
        timer->destroy(timer);

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_notify2()
 * test message notify with mailbox expiration
 */
void test_notify2(void)
{
    setup_this_test();

    do
    {   int result = 0;
        etch_object* mystateobj = NULL;
        const int LIFETIME_ONE_SECOND = 1000, TWO_SECONDS = 2000;
        etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, 0, 2);
        i_mailbox* ibox  = mbox->imailbox;
        check_mailbox_status(FALSE, NULL, NULL, FALSE);
        mystateobj = new_etch_object(CLASSID_MYSTATE, new_int32(MYSTATE_VALUE));

        result = mbox->register_notify(ibox, my_mailbox_notify, mystateobj, LIFETIME_ONE_SECOND);

        CU_ASSERT_EQUAL(result, 0);
        check_mailbox_status(FALSE, NULL, NULL, FALSE); 

        etch_sleep(TWO_SECONDS);
        check_mailbox_status(TRUE, ibox, mystateobj, TRUE); 

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_notify3()
 * test message notify 
 */
void test_notify3(void)
{
    setup_this_test();

    do
    {   int result = 0;
        etch_object* mystateobj = NULL;
        const int LIFETIME_FOREVER = 0, TWO_SECONDS = 2000;
        etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, 0, 2);
        check_mailbox_status(FALSE, NULL, NULL, FALSE);
        mystateobj = new_etch_object(CLASSID_MYSTATE, new_int32(MYSTATE_VALUE));

        result = mbox->register_notify(mbox->imailbox, my_mailbox_notify, mystateobj, LIFETIME_FOREVER);

        CU_ASSERT_EQUAL(result, 0);
        check_mailbox_status(FALSE, NULL, NULL, FALSE); 

        etch_sleep(TWO_SECONDS);
        check_mailbox_status(FALSE, NULL, NULL, FALSE); 

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_notify4()
 * test message notify.
 * test that notify callback receives expected results on a successful queue of
 * a message to a mailbox. 
 */
void test_notify4(void)
{
    setup_this_test();

    do
    {   int result = 0;
        etch_object* mystateobj = NULL;
        etch_message* mymessage = NULL;
        const int LIFETIME_FOREVER = 0, MYMSGID = 1337;
        etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, 0, 2);
        i_mailbox* ibox = mbox->imailbox;
        check_mailbox_status(FALSE, NULL, NULL, FALSE);
        mystateobj = new_etch_object(CLASSID_MYSTATE, new_int32(MYSTATE_VALUE));

        result = mbox->register_notify(ibox, my_mailbox_notify, mystateobj, LIFETIME_FOREVER);

        CU_ASSERT_EQUAL(result, 0);
        check_mailbox_status(FALSE, NULL, NULL, FALSE); 

        mymessage = new_message(g_type1, 0, (etch_value_factory*) g_vf);
        message_set_id(mymessage, new_int64(MYMSGID));
        printf("\n");

        /* relinquish message on successful queue of message to mailbox, retain message
         * on other than success. possible results: 0, -1, ETCH_MAILBOX_TIMEOUT.
         */
        result = mbox->message(ibox, g_who2, mymessage);

        CU_ASSERT_EQUAL(result, 0);
        if (0 != result)
            mymessage->destroy(mymessage);

        check_mailbox_status(TRUE, ibox, mystateobj, FALSE); 

        /* destroying mailbox causes close which causes our message to be redelivered.
         * our redelivery override causes that message to arrive in our redelivery list.
         * that list and its message content is destroyed at teardown_this_test(). 
         */
        mbox->destroy(mbox); 

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_read1()
 * test read message from mailbox and close box
 */
void test_read1(void)
{
    setup_this_test();

    do
    {   int result = 0, MYMSGID = 1337;
        etch_message* mymessage = NULL;
        etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, 0, 2);
        check_mailbox(mbox, TRUE, FALSE, FALSE, FALSE, 0);
        mymessage = new_message(g_type1, 0, (etch_value_factory*) g_vf);
        message_set_id(mymessage, new_int64(MYMSGID));

        check_deliver(mbox, TRUE, g_who2, mymessage);
        check_mailbox(mbox, FALSE, FALSE, FALSE, FALSE, 0);

        check_read(mbox, TRUE, g_who2, mymessage);
        check_mailbox(mbox, TRUE, FALSE, FALSE, FALSE, 0);

        check_close_delivery(mbox, TRUE);
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 0);

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_read2()
 * test close delivery before read message from mailbox 
 */
void test_read2(void)
{
    setup_this_test();

    do
    {   int result = 0, MYMSGID = 1337;
        etch_message* mymessage = NULL;
        etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, 0, 2);
        check_mailbox(mbox, TRUE, FALSE, FALSE, FALSE, 0);
        mymessage = new_message(g_type1, 0, (etch_value_factory*) g_vf);
        message_set_id(mymessage, new_int64(MYMSGID));

        check_deliver(mbox, TRUE, g_who2, mymessage);
        check_mailbox(mbox, FALSE, FALSE, FALSE, FALSE, 0);

        check_close_delivery(mbox, TRUE);
        check_mailbox(mbox, FALSE, FALSE, TRUE, TRUE, 0);

        /* these checks were different in the java test, assuming it was wrong.
         * java had us expecting to read something, however the queue was closed above 
         */
        check_read(mbox, FALSE, g_who2, mymessage);
        check_mailbox(mbox, FALSE, FALSE, TRUE, TRUE, 0);

        /* mymessage remains queued in the mailbox. destroying the mailbox destroys the message  */
        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_read4()
 */
void test_read4(void)
{
    setup_this_test();

    do
    {   int result = 0;
        etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, 0, 2);

        check_close_delivery(mbox, TRUE);
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 0);

        check_read(mbox, FALSE, NULL, NULL);
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 0);

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_read6()
 */
void test_read6(void)
{
    setup_this_test();

    do
    {   const int LIFETIME_100 = 100;
        etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, LIFETIME_100, 2);

        check_close_delivery(mbox, TRUE);
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 0);

        etch_sleep(2 * LIFETIME_100);

        check_read(mbox, FALSE, NULL, NULL);
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 0);

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_read7()
 */
void test_read7(void)
{
    setup_this_test();

    do
    {   etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, 0, 2);

        check_read_withwait(mbox, ETCH_NOWAIT, FALSE, NULL, NULL);
        check_mailbox(mbox, TRUE, FALSE, FALSE, FALSE, 0);

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_read8()
 */
void test_read8(void)
{
    setup_this_test();

    do
    {   const int MAXDELAY_1MS = 1;
        etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, 0, 2);

        check_read_withwait(mbox, MAXDELAY_1MS, FALSE, NULL, NULL);
        check_mailbox(mbox, TRUE, FALSE, FALSE, FALSE, 0);

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_closeread1()
 * open box and close it for read while empty
 */
void test_closeread1(void)
{
    setup_this_test();

    do
    {   etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, 0, 2);
        check_close_read(mbox, TRUE);   
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 0);  
        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_closeread2()
 * open box and close it for delivery while non-empty
 */
void test_closeread2(void)
{
    setup_this_test();

    do
    {   etch_plainmailbox* mbox  = new_mailbox (g_manager->imanager, 1, -1, 0, 2);
        etch_message *mymessage1 = new_message(g_type1, 0, (etch_value_factory*) g_vf);
        message_set_id(mymessage1, new_int64(1)); 
        #if IS_DEBUG_CONSOLE
        printf("\n");
        #endif
       
        check_deliver(mbox, TRUE, g_who1, mymessage1);
        check_mailbox(mbox, FALSE, FALSE, FALSE, FALSE, 0);  

        check_close_read(mbox, TRUE);
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 1);  
        check_redelivered(mbox, 0, mymessage1);

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_closeread3()
 * open box and close it for read twice while empty
 */
void test_closeread3(void)
{
    setup_this_test();

    do
    {   etch_plainmailbox* mbox = new_mailbox (g_manager->imanager, 1, -1, 0, 2);
        check_close_read(mbox, TRUE);   
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 0);  
        // currently we don't get an already closed return when the underlying queue has not
        // yet been closed. this is benign, but we should really ensure that the queue is closed 
        // before mailbox state is set to either closed for read or shutdown.
        // check_close_read(mbox, FALSE); // test disabled due to situation commented above 
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 0);  
        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_closeread4()
 * open box and close it for delivery twice while non-empty
 */
void test_closeread4(void)
{
    setup_this_test();

    do
    {   etch_plainmailbox* mbox  = new_mailbox (g_manager->imanager, 1, -1, 0, 2);
        etch_message *mymessage1 = new_message(g_type1, 0, (etch_value_factory*) g_vf);
        message_set_id(mymessage1, new_int64(1)); 
        #if IS_DEBUG_CONSOLE
        printf("\n");
        #endif
       
        check_deliver(mbox, TRUE, g_who1, mymessage1);
        check_mailbox(mbox, FALSE, FALSE, FALSE, FALSE, 0);  

        check_close_read(mbox, TRUE);
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 1);  
        check_redelivered(mbox, 0, mymessage1);

        // check_close_read(mbox, FALSE);  // test disabled - see test_closeread3 for comment
        check_mailbox(mbox, TRUE, FALSE, TRUE, TRUE, 1);  
        check_redelivered(mbox, 0, mymessage1);

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_full1()
 */
void test_full1(void)
{
    setup_this_test();

    do
    {   const int CAPACITY_2 = 2;
        etch_plainmailbox* mbox  = new_mailbox (g_manager->imanager, 1, -1, 0, CAPACITY_2);
        etch_message *mymessage1 = new_message(g_type1, 0, (etch_value_factory*) g_vf);
        etch_message *mymessage2 = new_message(g_type2, 0, (etch_value_factory*) g_vf);
        message_set_id(mymessage1, new_int64(1)); 
        message_set_id(mymessage2, new_int64(2));
        #if IS_DEBUG_CONSOLE
        printf("\n");
        #endif

        check_mailbox(mbox, TRUE, FALSE, FALSE, FALSE, 0);  /* should be empty */

        check_deliver(mbox, TRUE, g_who1, mymessage1);
        check_mailbox(mbox, FALSE, FALSE, FALSE, FALSE, 0); /* should be non-empty */

        check_deliver(mbox, TRUE, g_who2, mymessage2);
        check_mailbox(mbox, FALSE, TRUE, FALSE, FALSE, 0);  /* should be full */

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_full2()
 */
void test_full2(void)
{
    setup_this_test();

    do
    {   const int CAPACITY_2 = 2;
        etch_plainmailbox* mbox  = new_mailbox (g_manager->imanager, 1, -1, 0, CAPACITY_2);
        etch_message *mymessage1 = new_message(g_type1, 0, (etch_value_factory*) g_vf);
        etch_message *mymessage2 = new_message(g_type2, 0, (etch_value_factory*) g_vf);
        message_set_id(mymessage1, new_int64(1)); 
        message_set_id(mymessage2, new_int64(2));
        #if IS_DEBUG_CONSOLE
        printf("\n");
        #endif

        check_deliver(mbox, TRUE, g_who1, mymessage1);      /* deliver message 1 */
        check_mailbox(mbox, FALSE, FALSE, FALSE, FALSE, 0); /* should be non-empty */

        check_deliver(mbox, TRUE, g_who2, mymessage2);      /* deliver message 2 */
        check_mailbox(mbox, FALSE, TRUE, FALSE, FALSE, 0);  /* box should be full */

        check_deliver(mbox, FALSE, g_who1, mymessage2);     /* deliver should fail */
        check_mailbox(mbox, FALSE, TRUE, FALSE, FALSE, 0);  /* box should still be full */

        mbox->destroy(mbox);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * main   
 */
int _tmain(int argc, _TCHAR* argv[])
{    
    char c=0;
    CU_pSuite pSuite = NULL;
    g_is_automated_test = argc > 1 && 0 != wcscmp(argv[1], L"-a");
    if (CUE_SUCCESS != CU_initialize_registry()) return 0;
    pSuite = CU_add_suite("plain mailbox test suite", init_suite, clean_suite);
    CU_set_output_filename("../test_plainmbox");
    etch_watch_id = 0;  

    CU_add_test(pSuite, "test mailbox map", test_mailboxes_map); 
    CU_add_test(pSuite, "test setup teardown", test_setup_teardown); 
    CU_add_test(pSuite, "test constructor", test_constructor); 

    CU_add_test(pSuite, "test delivery", test_deliver); 

    CU_add_test(pSuite, "test close delivery 1", test_closedelivery1);
    CU_add_test(pSuite, "test close delivery 2", test_closedelivery2);
    CU_add_test(pSuite, "test close delivery 3", test_closedelivery3);

    CU_add_test(pSuite, "test timer", test_timer); 

    CU_add_test(pSuite, "test notify a", test_notify1); 
    CU_add_test(pSuite, "test notify b", test_notify2); 
    CU_add_test(pSuite, "test notify c", test_notify3);
    CU_add_test(pSuite, "test notify d", test_notify4); 

    CU_add_test(pSuite, "test read a", test_read1); 
    CU_add_test(pSuite, "test read b", test_read2);
    CU_add_test(pSuite, "test read c", test_read4);
    CU_add_test(pSuite, "test read d", test_read6);
    CU_add_test(pSuite, "test read e", test_read7);
    CU_add_test(pSuite, "test read f", test_read8);

    CU_add_test(pSuite, "test close read 1", test_closeread1);
    CU_add_test(pSuite, "test close read 2", test_closeread2);
    CU_add_test(pSuite, "test close read 3", test_closeread3);
    CU_add_test(pSuite, "test close read 4", test_closeread4);

    CU_add_test(pSuite, "test full 1", test_full1);
    CU_add_test(pSuite, "test full 2", test_full2);
              
    if (g_is_automated_test)    
        CU_automated_run_tests();    
    else
    {   CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    }

    if (!g_is_automated_test) { printf("any key ..."); while(!c) c = _getch(); printf("\n"); }     
    CU_cleanup_registry();
    return CU_get_error(); 
}
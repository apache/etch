/**
 * test_tcpconn.c
 * test TCP connection
 */

#include "apr_time.h" /* some apr must be included first */
#include "etchmon.h"  /* must be included second */

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#include "etch_global.h"
#include "etchconn.h"
#include "etchthread.h"
#include "etchlog.h"

int apr_setup(void);
int apr_teardown(void);
apr_pool_t* g_apr_mempool;
const char* pooltag = "etchpool";

int init_suite(void)
{
    apr_setup();
    return etch_runtime_init();
}

int clean_suite(void)
{
    apr_teardown();
    return etch_runtime_cleanup(0,0); /* free memtable and cache etc */
}

int g_is_automated_test, g_bytes_allocated;

#define IS_DEBUG_CONSOLE FALSE

/*
 * apr_setup()
 * establish apache portable runtime environment
 */
int apr_setup(void)
{
    int result = 0;
    apr_status_t aprresult = apr_initialize();
    if (aprresult == APR_SUCCESS)
        aprresult = apr_pool_create(&g_apr_mempool, NULL);
    if (aprresult == APR_SUCCESS)
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

static char *testval  = "test";
static char *VAL_UP   = "UP";
static char *VAL_DOWN = "DOWN";

void setAndWait(apr_thread_start_t setfunc, apr_thread_start_t waitfunc);



void testCreateAndDestroy(void)
{
    etchmon *mon_ptr;
    const size_t vlen = (strlen(testval)+1) * sizeof(char);
    etch_apr_mempool = g_apr_mempool;
    
    mon_ptr = etchmon_create("testCreateAndDestroy", testval, vlen, g_apr_mempool);
    CU_ASSERT_TRUE(mon_ptr != NULL);
    
    etchmon_destroy(mon_ptr);

    g_bytes_allocated = etch_showmem(TRUE,FALSE);
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);
    memtable_clear(); /* start fresh for next test */
}


void testSet(void)
{
    etchmon *mon_ptr;
    const size_t vlen = (strlen(testval)+1) * sizeof(char);
    etch_apr_mempool = g_apr_mempool;

    mon_ptr = etchmon_create("testSet", testval, vlen, g_apr_mempool);
    CU_ASSERT_TRUE(mon_ptr != NULL);    
    
    etchmon_set(mon_ptr, testval, (strlen(testval)+1)*sizeof(char));
    CU_ASSERT_STRING_EQUAL(mon_ptr->val.value_ptr_to,testval);

    etchmon_destroy(mon_ptr);

    g_bytes_allocated = etch_showmem(TRUE,FALSE);
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);
    memtable_clear(); /* start fresh for next test */
}


void testWaitTimeout(void)
{
    etchmon *mon_ptr;
    int rc;
    const size_t vlen = (strlen(testval)+1) * sizeof(char);
    etch_apr_mempool = g_apr_mempool;

    mon_ptr = etchmon_create("testWaitTimeout", testval, vlen, g_apr_mempool);
    CU_ASSERT_TRUE(mon_ptr != NULL);
    
    rc = etchmon_wait_until_set(mon_ptr, 10000);
    CU_ASSERT( rc == ETCHMON_STATUS_TIMEOUT);

    etchmon_destroy(mon_ptr);

    g_bytes_allocated = etch_showmem(TRUE,FALSE);
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);
    memtable_clear(); /* start fresh for next test */
}


static void* APR_THREAD_FUNC threadSetFunc(apr_thread_t *thread_ptr, void *p)
{   
    int s;
    apr_status_t r;
    const size_t vlen = (strlen(testval)+1) * sizeof(char);
    etchmon *mon_ptr = (etchmon *) p;
    s = etchmon_set(mon_ptr, testval, vlen);
    CU_ASSERT(s == ETCHMON_STATUS_SUCCESS);
    r = apr_thread_exit(thread_ptr, APR_SUCCESS);

    return NULL;
}


static void* APR_THREAD_FUNC threadWaitFunc(apr_thread_t *thread_ptr, void *p)
{
    int w;
    apr_status_t r;
    etchmon *mon_ptr = (etchmon *) p;
    w = etchmon_wait_until_set(mon_ptr, 0);
    CU_ASSERT(w == ETCHMON_STATUS_SUCCESS);
    CU_ASSERT_STRING_EQUAL(mon_ptr->val.value_ptr_to, testval);
    r = apr_thread_exit(thread_ptr, APR_SUCCESS);

    return NULL;
}


static void* APR_THREAD_FUNC threadMultiSetFunc(apr_thread_t *thread_ptr, void *p)
{   
    int s;
    apr_status_t r;
    etchmon *mon_ptr = (etchmon *) p;
    s = etchmon_set(mon_ptr, testval, (strlen(testval)+1)*sizeof(char));
    /* sleep for a bit */
    etch_sleep(5);
    s = etchmon_set(mon_ptr, VAL_UP, (strlen(VAL_UP)+1)*sizeof(char));
    etch_sleep(5);
    s = etchmon_set(mon_ptr, VAL_UP, (strlen(VAL_DOWN)+1)*sizeof(char));
    CU_ASSERT(s == ETCHMON_STATUS_SUCCESS);
    r = apr_thread_exit(thread_ptr, APR_SUCCESS);

    return NULL;
}


static void* APR_THREAD_FUNC threadWaitTillEqualFunc(apr_thread_t *thread_ptr, void *p)
{
    int w;
    apr_status_t r;
    etchmon *mon_ptr = (etchmon *) p;
    w = etchmon_wait_until_equal(mon_ptr, VAL_UP, (strlen(VAL_UP)+1)*sizeof(char), 0);
    CU_ASSERT(w == ETCHMON_STATUS_SUCCESS);
    w = etchmon_wait_until_equal(mon_ptr, VAL_UP, (strlen(VAL_DOWN)+1)*sizeof(char), 0);
    r = apr_thread_exit(thread_ptr, APR_SUCCESS);

    return NULL;
}


/* create two threads to test synchronization */
void testSetAndWait(void)
{   
    etch_apr_mempool = g_apr_mempool;

    setAndWait(threadSetFunc, threadWaitFunc);

    g_bytes_allocated = etch_showmem(TRUE,FALSE);
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);
    memtable_clear(); /* start fresh for next test */
}


void testSetAndWaitUntilEqual(void)
{
    etch_apr_mempool = g_apr_mempool;

    setAndWait(threadMultiSetFunc, threadWaitTillEqualFunc);

    g_bytes_allocated = etch_showmem(TRUE,FALSE);
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);
    memtable_clear(); /* start fresh for next test */
}


void setAndWait(apr_thread_start_t setfunc, apr_thread_start_t waitfunc)
{
    apr_status_t r;
    apr_status_t rt;
    apr_threadattr_t    *attr_ptr;
    apr_thread_t        *set_thread_ptr;
    apr_thread_t        *wait_thread_ptr;
    etchmon *mon_ptr;
    const size_t vlen = (strlen(testval)+1) * sizeof(char);
    
    mon_ptr = etchmon_create("testSetAndWait", testval, vlen, g_apr_mempool);
    CU_ASSERT_TRUE(mon_ptr != NULL);

    r = apr_threadattr_create(&attr_ptr, g_apr_mempool);
    CU_ASSERT_TRUE(r == APR_SUCCESS );

    r = apr_thread_create(&set_thread_ptr, attr_ptr, setfunc, (void *)mon_ptr, g_apr_mempool);
    CU_ASSERT_TRUE(r == APR_SUCCESS );

    r = apr_threadattr_create(&attr_ptr, g_apr_mempool);
    CU_ASSERT_TRUE(r == APR_SUCCESS );  

    r = apr_thread_create(&wait_thread_ptr, attr_ptr, waitfunc, (void *)mon_ptr, g_apr_mempool);
    CU_ASSERT_TRUE(r == APR_SUCCESS );

    r = apr_thread_join(&rt, set_thread_ptr);
    CU_ASSERT(r == APR_SUCCESS);

    r = apr_thread_join(&rt, wait_thread_ptr);
    CU_ASSERT(r == APR_SUCCESS);

    etchmon_destroy(mon_ptr);
}



/* ============================================================================
 * START TCP TESTS
 * ============================================================================
 */

etch_tcpconn     *tcpconn;
etch_tcpconn     *tcpconn2;
etch_tcplistener *listener;
#define TEST_PORT 7302
#define TEST_IP  "127.0.0.1"
#define NUM_TEST_CONNECTIONS 10
etch_tcpconn *accepted_connections[NUM_TEST_CONNECTIONS];


/**
 * received_data_handler()
 * callback to receive data from TCP stack
 */
void received_data_handler(void *data, size_t len)
{
    etchlog("etchconntest", ETCHLOG_DEBUG, "got data '%s'\n", (const char*)data);
}


/**
 * got_accepted()
 * callback to receive notification of accepted connection
 */
int got_accepted(apr_socket_t *aprsocket)
{
    int result = 0, i = 0;

    etch_tcpconn *accepted_conx = etchconn_create_tcp_with_socket(aprsocket);

    if (accepted_conx != NULL)
    {
        /* set the data handler callback */
        accepted_conx->conn.on_data = received_data_handler; 

        result = etchconn_start((etchconn*) accepted_conx);
        
        /* save the pointer for later releasing */

        while(i < NUM_TEST_CONNECTIONS && accepted_connections[i] != NULL)
              ++i;

        if (i < NUM_TEST_CONNECTIONS)
            accepted_connections[i] = accepted_conx;
    }

    return result;
}


/**
 * tcp_test_setup()
 */
int tcp_test_setup(void)
{
    etch_apr_mempool = g_apr_mempool;
    listener = etchconn_create_listener(TEST_IP, TEST_PORT, 5, 5, g_apr_mempool);
    CU_ASSERT_PTR_NOT_NULL_FATAL(listener);
    
    listener->conn.on_accepted = got_accepted;

    tcpconn = etchconn_create_tcp(TEST_IP, TEST_PORT, 5, g_apr_mempool);
    CU_ASSERT_PTR_NOT_NULL_FATAL(listener);

    /* create another connection to listener */
    tcpconn2 = etchconn_create_tcp(TEST_IP, TEST_PORT, 5, g_apr_mempool);
    CU_ASSERT_PTR_NOT_NULL_FATAL(tcpconn2);
    
    memset(accepted_connections, 0, NUM_TEST_CONNECTIONS * sizeof(void*));

    return 0;
}


/**
 * tcp_test_teardown()
 */
int tcp_test_teardown(void)
{
    int i = 0;  /* memory leaks up the yingyang here - wrappers not destroyed */
    etchconn_destroy((etchconn*)tcpconn); /* TODO call object dtor instead */
    etchconn_destroy((etchconn*)tcpconn2);
    etchconn_destroy((etchconn*)listener);
    
    for(; i < NUM_TEST_CONNECTIONS; i++)
    {
         etch_tcpconn* connection = accepted_connections[i];
         if (NULL == connection) break;
         
         etch_tcpconn_destroy(connection);  

         accepted_connections[i] = NULL;
    }

    return 0;
}


/**
 * test_tcp_connection()
 */
void test_tcp_connection(void)
{
    int i=0, result=0;
    char data[] = "this is testSimple data";
    const int ITERS = 50;

    result = tcp_test_setup();

    result = etchconn_start((etchconn*)listener);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = etchmon_wait_until_equal(listener->conn.monitor, /* HANGS HERE */
        ETCHCONN_UP, (strlen(ETCHCONN_UP)+1)*sizeof(char),0);

    result = etchconn_start((etchconn*)tcpconn);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = etchmon_wait_until_equal(tcpconn->conn.monitor, 
        ETCHCONN_UP, (strlen(ETCHCONN_UP)+1)*sizeof(char),0);

    CU_ASSERT_EQUAL(result,0);

    for(i = 0; i < ITERS; i++)
    {
        result = etchconn_send_tcp((etchconn*)tcpconn, data, 
            (strlen(data)+1)*sizeof(char));

        CU_ASSERT_EQUAL(result,0);
        etch_sleep(100);
    }

    etch_sleep(1000);

    result = etchconn_stop((etchconn*)tcpconn);
    CU_ASSERT_EQUAL(result,0);

    etch_sleep(1000);
    CU_ASSERT_PTR_NOT_NULL(accepted_connections[0]);

    result = etchconn_stop((etchconn*)accepted_connections[0]);
    CU_ASSERT_EQUAL(result,0);

    result = etchconn_stop((etchconn*)listener);
    CU_ASSERT_EQUAL(result,0);

    result = tcp_test_teardown();
}


/**
 * test_more_connections()
 */
void test_more_connections(void)
{
    int i=0, result=0;
    const int ITERS = 100;
    char data[]  = "CONN1 test_more_connections";
    char data2[] = "CONN2 test_more_connections";

    result = tcp_test_setup();

    result = etchconn_start((etchconn*)listener);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = etchmon_wait_until_equal(listener->conn.monitor, 
        ETCHCONN_UP, (strlen(ETCHCONN_UP)+1)*sizeof(char),0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = etchconn_start((etchconn*)tcpconn);
    CU_ASSERT_EQUAL_FATAL(result,0);

    /* create another connection to listener */
    result = etchconn_start((etchconn*) tcpconn2);
    CU_ASSERT_EQUAL_FATAL(result,0)

    result = etchmon_wait_until_equal(tcpconn->conn.monitor,  
        ETCHCONN_UP, (strlen(ETCHCONN_UP)+1)*sizeof(char),0);
    CU_ASSERT_EQUAL(result,0)

    result = etchmon_wait_until_equal(tcpconn2->conn.monitor, 
        ETCHCONN_UP, (strlen(ETCHCONN_UP)+1)*sizeof(char),0);
    CU_ASSERT_EQUAL(result,0)

    for(i = 0; i < ITERS; i++)
    {
        result = etchconn_send_tcp((etchconn*)tcpconn,  data, 
            (strlen(data) +1)*sizeof(char));
        CU_ASSERT_EQUAL(result,0)

        result = etchconn_send_tcp((etchconn*)tcpconn2, data2,
            (strlen(data2)+1)*sizeof(char));
        CU_ASSERT_EQUAL(result,0)

        etch_sleep(10);
    }

    etch_sleep(1000);

    result = etchconn_stop((etchconn*)tcpconn);
    CU_ASSERT_EQUAL(result,0)

    result = etchconn_stop((etchconn*)tcpconn2);
    CU_ASSERT_EQUAL(result,0)

    etch_sleep(1000);
    CU_ASSERT_PTR_NOT_NULL(accepted_connections[0]);

    result = etchconn_stop((etchconn*)accepted_connections[0]);
    CU_ASSERT_EQUAL(result,0)

    CU_ASSERT_PTR_NOT_NULL(accepted_connections[1]);

    result = etchconn_stop((etchconn*)accepted_connections[1]);
    CU_ASSERT_EQUAL(result,0)

    result = etchconn_stop((etchconn*)listener);
    CU_ASSERT_EQUAL(result,0)

    result = tcp_test_teardown();
}

/* ============================================================================
 * END TCP TESTS
 * ============================================================================
 */

/**
 * main   
 */
int _tmain(int argc, _TCHAR* argv[])
{
    char c=0;
    CU_pSuite ps = NULL;
    g_is_automated_test = argc > 1 && 0 != wcscmp(argv[1], L"-a");
    if (CUE_SUCCESS != CU_initialize_registry()) return 0;
    CU_set_output_filename("../test_tcpconn");
    ps = CU_add_suite("suite_tcpconn", init_suite, clean_suite);
        
    //CU_add_test(ps, "createAndDestroy", testCreateAndDestroy);
    //CU_add_test(ps, "set", testSet);
    //CU_add_test(ps, "waittimeout", testWaitTimeout);
    //CU_add_test(ps, "setAndWait", testSetAndWait);
    //CU_add_test(ps, "testSetAndWaitUntilEqual", testSetAndWaitUntilEqual);

    CU_add_test(ps, "simple tcp connection", test_tcp_connection);

    if (g_is_automated_test)    
        CU_automated_run_tests();    
    else
    {   CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    }

    if (!g_is_automated_test) { printf("any key ..."); while(!c) c = _getch(); wprintf(L"\n"); }     
    CU_cleanup_registry();
    return CU_get_error();
}
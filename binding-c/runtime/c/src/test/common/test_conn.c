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

/**
 * etchconn test routines.
 */
 
 #include "CUnit.h"
#include "Basic.h"
#include "Console.h"
#include "Automated.h"
#include "etchconn.h"
#include "apr_time.h"
#include "etchlog.h"

#define MAX_TEST_CONNECTIONS    10

static apr_pool_t *pool_ptr = NULL;
static etch_tcpconn *tcpconn_ptr = NULL;
static etch_tcpconn *tcpconn2_ptr = NULL;
static etch_tcplistener *listener_ptr = NULL;
static etch_tcpconn *accepted[MAX_TEST_CONNECTIONS];
static BOOLEAN got_accepted(apr_socket_t *socket_ptr);

static void got_data(void *data_ptr, size_t len);

int setup(void)
{
    apr_status_t r;
    r = apr_initialize();
    if (r == APR_SUCCESS)
        r = apr_pool_create(&pool_ptr, NULL);

    if (r == APR_SUCCESS )
    {
        listener_ptr = etchconn_create_listener("127.0.0.1", 7302, 5, 5, pool_ptr);
        if (listener_ptr != NULL)
            listener_ptr->conn.handle_accepted = got_accepted;
        tcpconn_ptr = etchconn_create_tcp("127.0.0.1", 7302, 5, pool_ptr);
        /* create another connection to listener */
        tcpconn2_ptr = etchconn_create_tcp("127.0.0.1", 7302, 5, pool_ptr);
    }
    
    memset(accepted, 0, sizeof(accepted));

    return (listener_ptr != NULL  && tcpconn_ptr != NULL  && tcpconn2_ptr != NULL ? 0 : 1);
}

int teardown(void)
{
    int i;
    etchconn_destroy((etchconn*)tcpconn_ptr);
    etchconn_destroy((etchconn*)tcpconn2_ptr);
    etchconn_destroy((etchconn*)listener_ptr);
    
    for(i = 0; i < MAX_TEST_CONNECTIONS && accepted[i] != NULL; i++)
    {
         etchconn_destroy((etchconn*)accepted[i]);
         accepted[i] = NULL;
    }

    if (pool_ptr != NULL)
        apr_pool_destroy(pool_ptr);
    pool_ptr = NULL;

    apr_terminate();

    return 0;
}

static BOOLEAN got_accepted(apr_socket_t *socket_ptr)
{
    BOOLEAN rc = FALSE;
    int i;

    etch_tcpconn *accepted_ptr = etchconn_create_tcp_with_socket(socket_ptr);
    if (accepted_ptr != NULL)
    {
        /* need to initialize data handler */
        accepted_ptr->conn.handle_data = got_data; 
        rc = etchconn_start((etchconn *) accepted_ptr);
        
        /* save the pointer for later releasing */
        i = 0;

        while (i < MAX_TEST_CONNECTIONS && accepted[i] != NULL)
            ++i;
        if (i < MAX_TEST_CONNECTIONS)
            accepted[i] = accepted_ptr;
    }

    return rc;
}

static void got_data(void *data_ptr, size_t len)
{
    etchlog_report("etchconntest", ETCHLOG_DEBUG, "Got data: %s\n", (const char*) data_ptr);
}

void testSimple(void)
{
    int i;
    int rc;
    char data[] = "this is testSimple data.";

    CU_ASSERT(etchconn_start((etchconn*)listener_ptr));
    rc = etchmon_wait_until_equal(listener_ptr->conn.monitor_ptr, ETCHCONN_UP, (strlen(ETCHCONN_UP)+1)*sizeof(char),0);
    CU_ASSERT(etchconn_start((etchconn*)tcpconn_ptr));

    rc = etchmon_wait_until_equal(tcpconn_ptr->conn.monitor_ptr, ETCHCONN_UP, (strlen(ETCHCONN_UP)+1)*sizeof(char),0);
    CU_ASSERT(rc==ETCHMON_STATUS_SUCCESS);
    for(i = 0; i < 50; i++)
    {
        CU_ASSERT(etchconn_send_tcp((etchconn*)tcpconn_ptr, data, (strlen(data)+1)*sizeof(char)));
        apr_sleep(100000);
    }
    apr_sleep(1000000);
    etchconn_stop((etchconn*)tcpconn_ptr);

    apr_sleep(1000000);
    CU_ASSERT_PTR_NOT_NULL(accepted[0]);
    etchconn_stop((etchconn*)accepted[0]);

    etchconn_stop((etchconn*)listener_ptr);
}

void testMoreConnections(void)
{
    int i;
    int rc;
    char data[] = "CONN1: this is testMoreConnections data.";
    char data2[] = "CONN2: this is testMoreConnections data.";

    CU_ASSERT(etchconn_start((etchconn*)listener_ptr));
    rc = etchmon_wait_until_equal(listener_ptr->conn.monitor_ptr, ETCHCONN_UP, (strlen(ETCHCONN_UP)+1)*sizeof(char),0);
    CU_ASSERT(etchconn_start((etchconn*)tcpconn_ptr));

    /* create another connection to listener */
    CU_ASSERT(etchconn_start((etchconn*) tcpconn2_ptr));

    rc = etchmon_wait_until_equal(tcpconn_ptr->conn.monitor_ptr, ETCHCONN_UP, (strlen(ETCHCONN_UP)+1)*sizeof(char),0);
    CU_ASSERT(rc==ETCHMON_STATUS_SUCCESS);

    rc = etchmon_wait_until_equal(tcpconn2_ptr->conn.monitor_ptr, ETCHCONN_UP, (strlen(ETCHCONN_UP)+1)*sizeof(char),0);
    CU_ASSERT(rc==ETCHMON_STATUS_SUCCESS);

    for(i = 0; i < 100; i++)
    {
        CU_ASSERT(etchconn_send_tcp((etchconn*)tcpconn_ptr, data, (strlen(data)+1)*sizeof(char)));
        CU_ASSERT(etchconn_send_tcp((etchconn*)tcpconn2_ptr, data2, (strlen(data2)+1)*sizeof(char)));
        apr_sleep(10000);
    }

    apr_sleep(1000000);
    etchconn_stop((etchconn*)tcpconn_ptr);

    etchconn_stop((etchconn*)tcpconn2_ptr);

    apr_sleep(1000000);
    CU_ASSERT_PTR_NOT_NULL(accepted[0]);
    etchconn_stop((etchconn*)accepted[0]);

    CU_ASSERT_PTR_NOT_NULL(accepted[1]);
    etchconn_stop((etchconn*)accepted[1]);

    etchconn_stop((etchconn*)listener_ptr);
}


int main(int argc, char **argv)
{
    CU_pSuite ps;
    CU_pTest pt;

    printf("hellor world!");
    CU_initialize_registry();
    
    ps = CU_add_suite("etchconn test suit", setup, teardown);
    pt = CU_add_test(ps, "testSimple", testSimple);
    pt = CU_add_test(ps, "testMoreConnections", testMoreConnections);

    CU_automated_run_tests();
    /*CU_console_run_tests();*/
    
    CU_cleanup_registry();
    return 0;
}
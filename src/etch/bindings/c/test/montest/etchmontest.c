#include "CUnit.h"
#include "Basic.h"
#include "Console.h"
#include "Automated.h"
#include "etchmon.h"
#include "apr_time.h"
#include "etchlog.h"

static apr_pool_t *pool_ptr = NULL;
static char *val_ptr = "test";
static char *VAL_UP = "UP";
static char *VAL_DOWN = "DOWN";

void setAndWait(apr_thread_start_t setfunc, apr_thread_start_t waitfunc);

int setup(void)
{
    apr_status_t r;
    r = apr_initialize();
    if (r == APR_SUCCESS)
        r = apr_pool_create(&pool_ptr, NULL);

    return (r == APR_SUCCESS ? 0 : 1);
}

int teardown(void)
{
    if (pool_ptr != NULL)
        apr_pool_destroy(pool_ptr);
    pool_ptr = NULL;

    apr_terminate();

    return 0;
}

void testCreateAndDestroy(void)
{
    etchmon *mon_ptr;
    
    mon_ptr = etchmon_create("testCreateAndDestroy", val_ptr, (strlen(val_ptr)+1)*sizeof(char), pool_ptr);
    CU_ASSERT_TRUE(mon_ptr != NULL);
    
    etchmon_destroy(mon_ptr);
    etchlog_report("testCreateAndDestroy", ETCHLOG_DEBUG, "good test create and destroy");
}

void testSet(void)
{
    etchmon *mon_ptr;

    mon_ptr = etchmon_create("testSet", val_ptr, (strlen(val_ptr)+1)*sizeof(char), pool_ptr);
    CU_ASSERT_TRUE(mon_ptr != NULL);
    
    
    etchmon_set(mon_ptr, val_ptr, (strlen(val_ptr)+1)*sizeof(char));
    CU_ASSERT_STRING_EQUAL(mon_ptr->val.value_ptr_to,val_ptr);

    etchmon_destroy(mon_ptr);
}

void testWaitTimeout(void)
{
    etchmon *mon_ptr;
    int rc;

    mon_ptr = etchmon_create("testWaitTimeout", val_ptr, (strlen(val_ptr)+1)*sizeof(char), pool_ptr);
    CU_ASSERT_TRUE(mon_ptr != NULL);
    
    rc = etchmon_wait_until_set(mon_ptr, 10000);
    CU_ASSERT( rc == ETCHMON_STATUS_TIMEOUT);

    etchmon_destroy(mon_ptr);
}

static void* APR_THREAD_FUNC threadSetFunc(apr_thread_t *thread_ptr, void *p)
{   
    int s;
    apr_status_t r;
    etchmon *mon_ptr = (etchmon *) p;
    s = etchmon_set(mon_ptr, val_ptr, (strlen(val_ptr)+1)*sizeof(char));
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
    CU_ASSERT_STRING_EQUAL(mon_ptr->val.value_ptr_to, val_ptr);
    r = apr_thread_exit(thread_ptr, APR_SUCCESS);

    return NULL;
}


static void* APR_THREAD_FUNC threadMultiSetFunc(apr_thread_t *thread_ptr, void *p)
{   
    int s;
    apr_status_t r;
    etchmon *mon_ptr = (etchmon *) p;
    s = etchmon_set(mon_ptr, val_ptr, (strlen(val_ptr)+1)*sizeof(char));
    /* sleep for a bit */
    apr_sleep(5000);
    s = etchmon_set(mon_ptr, VAL_UP, (strlen(VAL_UP)+1)*sizeof(char));
    apr_sleep(5000);
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
    setAndWait(threadSetFunc, threadWaitFunc);
}

void testSetAndWaitUntilEqual(void)
{
    setAndWait(threadMultiSetFunc, threadWaitTillEqualFunc);
}

void setAndWait(apr_thread_start_t setfunc, apr_thread_start_t waitfunc)
{
    apr_status_t r;
    apr_status_t rt;
    apr_threadattr_t    *attr_ptr;
    apr_thread_t        *set_thread_ptr;
    apr_thread_t        *wait_thread_ptr;

    etchmon *mon_ptr;
    
    mon_ptr = etchmon_create("testSetAndWait", val_ptr, (strlen(val_ptr)+1)*sizeof(char), pool_ptr);
    CU_ASSERT_TRUE(mon_ptr != NULL);

    r = apr_threadattr_create(&attr_ptr, pool_ptr);
    CU_ASSERT_TRUE(r == APR_SUCCESS );

    r = apr_thread_create(&set_thread_ptr, attr_ptr, setfunc, (void *)mon_ptr, pool_ptr);
    CU_ASSERT_TRUE(r == APR_SUCCESS );

    r = apr_threadattr_create(&attr_ptr, pool_ptr);
    CU_ASSERT_TRUE(r == APR_SUCCESS );

    r = apr_thread_create(&wait_thread_ptr, attr_ptr, waitfunc, (void *)mon_ptr, pool_ptr);
    CU_ASSERT_TRUE(r == APR_SUCCESS );

    r = apr_thread_join(&rt, set_thread_ptr);
    CU_ASSERT(r == APR_SUCCESS);

    r = apr_thread_join(&rt, wait_thread_ptr);
    CU_ASSERT(r == APR_SUCCESS);

    etchmon_destroy(mon_ptr);

}

int main(int argc, char **argv)
{
    CU_pSuite ps;
    CU_pTest pt;

    printf("hellor world!");
    CU_initialize_registry();
    
    ps = CU_add_suite("etchmon test suit", setup, teardown);
    pt = CU_add_test(ps, "createAndDestroy", testCreateAndDestroy);
    pt = CU_add_test(ps, "set", testSet);
    pt = CU_add_test(ps, "waittimeout", testWaitTimeout);
    pt = CU_add_test(ps, "setAndWait", testSetAndWait);
    pt = CU_add_test(ps, "testSetAndWaitUntilEqual", testSetAndWaitUntilEqual);

    CU_automated_run_tests();
    
    CU_cleanup_registry();
    return 0;
}
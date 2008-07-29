#include <limits.h>
#include <float.h>
#include "CUnit.h"
#include "Basic.h"
#include "Console.h"
#include "Automated.h"
#include "etchflexbuff.h"
#include "etchmem.h"
#include "etchrun.h"
#include "etch_global.h"

etch_flex_buffer    *efb_ptr = NULL;

int setup(void)
{
    return etch_init();
}

int teardown(void)
{
    return etch_runtime_cleanup(0,0); /* free memtable and cache etc */
}

void testCreateAndDestroy0(void)
{
    efb_ptr = etch_flex_buffer_create();
    CU_ASSERT_PTR_NOT_NULL(efb_ptr);
    
    CU_ASSERT(etch_flex_buffer_avail(efb_ptr) == 0);
    
    etch_flex_buffer_destroy(efb_ptr);
}

void testCreateAndDestroy1(void)
{
    efb_ptr = etch_flex_buffer_create_s(0);
    CU_ASSERT_PTR_NULL(efb_ptr);
    
    efb_ptr = etch_flex_buffer_create_s(1);
    CU_ASSERT_PTR_NOT_NULL(efb_ptr);
    CU_ASSERT(etch_flex_buffer_avail(efb_ptr) == 0);

    etch_flex_buffer_destroy(efb_ptr);
    
    efb_ptr = etch_flex_buffer_create_s(4*1024*1024+1);
    CU_ASSERT_PTR_NULL(efb_ptr);
}

void testCreateAndDestroy2(void)
{
    void *ptr = etch_malloc(1024, 0);
    efb_ptr = etch_flex_buffer_create_b(ptr, 1024, 0);
    CU_ASSERT_PTR_NOT_NULL(efb_ptr);
    CU_ASSERT(etch_flex_buffer_avail(efb_ptr) == 0);

    etch_flex_buffer_destroy(efb_ptr);
}

void testCreateAndDestroy3(void)
{
    void *ptr = etch_malloc(1024, 0);
    efb_ptr = etch_flex_buffer_create_bi(ptr, 1024, 100, 0);
    CU_ASSERT_PTR_NOT_NULL(efb_ptr);
    CU_ASSERT(etch_flex_buffer_avail(efb_ptr) == 100);

    etch_flex_buffer_destroy(efb_ptr);
}

void testPutAndGet(void)
{
    byte test_bytes[] = "testbytes";
    void *test_buffer = etch_malloc(128, 0);
    size_t n;
    int i;
    etch_flex_buffer *efb2_ptr;

    efb_ptr = etch_flex_buffer_create();

    etch_flex_buffer_put(efb_ptr, test_bytes, 0, strlen(test_bytes));

    /* rewind the buffer */
    etch_flex_buffer_set_index(efb_ptr, 0);
    n = etch_flex_buffer_get(efb_ptr, test_buffer, 0, strlen(test_bytes));
    CU_ASSERT(memcmp(test_buffer, test_bytes, n) == 0);

    /* test put byte */
    etch_flex_buffer_set_index(efb_ptr, 0);
    for(i = 0; i < (int) strlen(test_bytes); i++)
        etch_flex_buffer_put_byte(efb_ptr, test_bytes[i]);    

    etch_flex_buffer_set_index(efb_ptr, 0);
    n = etch_flex_buffer_get(efb_ptr, test_buffer, 0, strlen(test_bytes));
    CU_ASSERT(memcmp(test_buffer, test_bytes, n) == 0);

    /* test put from another buffer */
    efb2_ptr = etch_flex_buffer_create();
    etch_flex_buffer_set_index(efb_ptr, 0);
    etch_flex_buffer_put_from_flex_buffer(efb2_ptr, efb_ptr, n);

    etch_flex_buffer_set_index(efb2_ptr, 0);
    n = etch_flex_buffer_get(efb2_ptr, test_buffer, 0, strlen(test_bytes));
    CU_ASSERT(memcmp(test_buffer, test_bytes, n) == 0);

    etch_free(test_buffer);
    etch_flex_buffer_destroy(efb_ptr);
    etch_flex_buffer_destroy(efb2_ptr);
}

void testPutAndGetFully(void)
{
    byte test_bytes[] = "testbytes";
    void *test_buffer = etch_malloc(128, 0);
    size_t n;

    efb_ptr = etch_flex_buffer_create();

    etch_flex_buffer_put(efb_ptr, test_bytes, 0, strlen(test_bytes));

    /* rewind the buffer */
    etch_flex_buffer_set_index(efb_ptr, 0);
    n = etch_flex_buffer_get_fully(efb_ptr, test_buffer, strlen(test_bytes));
    CU_ASSERT(memcmp(test_buffer, test_bytes, n) == 0);

    etch_flex_buffer_set_index(efb_ptr, 0);
    n = etch_flex_buffer_get_fully(efb_ptr, test_buffer, 4);
    CU_ASSERT(memcmp(test_buffer, test_bytes, 4) == 0);

    etch_flex_buffer_set_index(efb_ptr, 0);
    n = etch_flex_buffer_get_fully(efb_ptr, test_buffer, 32);
    CU_ASSERT(memcmp(test_buffer, test_bytes, min(strlen(test_bytes), 32)) == 0);

    etch_free(test_buffer);
    etch_flex_buffer_destroy(efb_ptr);
}


void testPutAndGetByte(void)
{
    byte test_bytes[] = "testbytes";
    void *test_buffer = etch_malloc(128, 0);
    size_t n;
    int i;

    efb_ptr = etch_flex_buffer_create();

    etch_flex_buffer_set_index(efb_ptr, 0);
    for(i = 0; i < (int)strlen(test_bytes); i++)
        etch_flex_buffer_put_byte(efb_ptr, test_bytes[i]);    

    etch_flex_buffer_set_index(efb_ptr, 0);
    n = etch_flex_buffer_get(efb_ptr, test_buffer, 0, strlen(test_bytes));
    CU_ASSERT(memcmp(test_buffer, test_bytes, n) == 0);


    etch_free(test_buffer);
    etch_flex_buffer_destroy(efb_ptr);
}

void testPutAndGetShort(void)
{
    short test_shorts[] = {SHRT_MIN, SHRT_MAX, 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 0x7FFF, \
        0xFFFF, 0xFFFE, -8192, -4096, -2048, -1024, -512, -256, -128, -64, -32, -16, -8, -4, -2, -1};

    int i;

    efb_ptr = etch_flex_buffer_create();

    for(i = 0; i < sizeof(test_shorts)/sizeof(short); i++)
        etch_flex_buffer_put_short(efb_ptr, test_shorts[i]);    

    etch_flex_buffer_set_index(efb_ptr, 0);
    for(i = 0; i < sizeof(test_shorts)/sizeof(short); i++)
        CU_ASSERT(test_shorts[i] == etch_flex_buffer_get_short(efb_ptr));    

    etch_flex_buffer_destroy(efb_ptr);
}

void testPutAndGetInt(void)
{
    int test_ints[] = {INT_MIN, INT_MAX, 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 0x7FFF, \
        0xFFFF, 0xFFFFF, 0xFFFFFF, 0xFFFFFFF, 0xFFFFFFE,0x7FFFFFFF, 0xFFFFFFFF, 0xF7FFFFFF, 0xF6FFFFFF, \
        0xF5FFFFFF, 0xF4FFFFFF, 0xF3FFFFFF, 0xF2FFFFFF, 0xF1FFFFFF, 0xF0FFFFFF, 0xFF0FFFFF, \
        -8192, -4096, -2048, -1024, -512, -256, -128, -64, -32, -16, -8, -4, -2, -1};

    int i;

    efb_ptr = etch_flex_buffer_create();

    for(i = 0; i < sizeof(test_ints)/sizeof(int); i++)
        etch_flex_buffer_put_int(efb_ptr, test_ints[i]);    

    etch_flex_buffer_set_index(efb_ptr, 0);
    for(i = 0; i < sizeof(test_ints)/sizeof(int); i++)
        CU_ASSERT(test_ints[i] == etch_flex_buffer_get_int(efb_ptr));    

    etch_flex_buffer_destroy(efb_ptr);
}

void testPutAndGetInt64(void)
{
    int64 test_int64s[] = {LONG_MIN, LONG_MAX, 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 0x7FFF, \
        0xFFFF, 0xFFFFF, 0xFFFFFF, 0xFFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF, 0xF7FFFFFF, 0xF6FFFFFF, 0x7FFFFFFFFFFFFFFF,\
        0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF0, 0xFFFFFFFFFFFFFF00, 0xFFFFFFFFFFFFF000, 0xFFFFFFFFFFFFF000, \
        0xF5FFFFFF, 0xF4FFFFFF, 0xF3FFFFFF, 0xF2FFFFFF, 0xF1FFFFFF, 0xF0FFFFFF, 0xFF0FFFFF, \
        -8192, -4096, -2048, -1024, -512, -256, -128, -64, -32, -16, -8, -4, -2, -1};

    int i;

    efb_ptr = etch_flex_buffer_create();

    for(i = 0; i < sizeof(test_int64s)/sizeof(int64); i++)
        etch_flex_buffer_put_long(efb_ptr, test_int64s[i]);    

    etch_flex_buffer_set_index(efb_ptr, 0);
    for(i = 0; i < sizeof(test_int64s)/sizeof(int64); i++)
        CU_ASSERT(test_int64s[i] == etch_flex_buffer_get_long(efb_ptr));    

    etch_flex_buffer_destroy(efb_ptr);
}

void testPutAndGetFloat(void)
{
    float test_floats[] = {FLT_MIN, 0.0f, 1.1f, 2.2f, 4.4f, 8.8f, 16.16f, 32.32f, 64.64f, 128.128f, 256.256f, 512.512f,\
        1024.1024f, 2048.2048f, 4096.4096f, 8192.8192f, FLT_MAX};

    int i;

    efb_ptr = etch_flex_buffer_create();

    for(i = 0; i < sizeof(test_floats)/sizeof(float); i++)
        etch_flex_buffer_put_float(efb_ptr, test_floats[i]);    

    etch_flex_buffer_set_index(efb_ptr, 0);
    for(i = 0; i < sizeof(test_floats)/sizeof(float); i++)
        CU_ASSERT(test_floats[i] == etch_flex_buffer_get_float(efb_ptr));    

    etch_flex_buffer_destroy(efb_ptr);
}

void testPutAndGetDouble(void)
{
    double test_doubles[] = {DBL_MIN, 0.0, 1.1, 2.2, 4.4, 8.8, 16.16, 32.32, 64.64, 128.128, 256.256, 512.512,\
        1024.1024, 2048.2048, 4096.4096, 8192.8192, DBL_MAX};

    int i;

    efb_ptr = etch_flex_buffer_create();

    for(i = 0; i < sizeof(test_doubles)/sizeof(double); i++)
        etch_flex_buffer_put_double(efb_ptr, test_doubles[i]);    

    etch_flex_buffer_set_index(efb_ptr, 0);
    for(i = 0; i < sizeof(test_doubles)/sizeof(double); i++)
        CU_ASSERT(test_doubles[i] == etch_flex_buffer_get_double(efb_ptr));    

    etch_flex_buffer_destroy(efb_ptr);
}

void testSetSize(void)
{
    etch_flex_buffer *efb_ptr = etch_flex_buffer_create();

    CU_ASSERT(etch_flex_buffer_set_length(efb_ptr, 1024));
    CU_ASSERT(etch_flex_buffer_avail(efb_ptr) == 1024);

    CU_ASSERT(etch_flex_buffer_set_length(efb_ptr, 0));
    CU_ASSERT(etch_flex_buffer_avail(efb_ptr) == 0);

    CU_ASSERT(etch_flex_buffer_set_length(efb_ptr, 64));
    CU_ASSERT(etch_flex_buffer_avail(efb_ptr) == 64);

    CU_ASSERT(etch_flex_buffer_set_length(efb_ptr, 4*1024*1024));
    CU_ASSERT(etch_flex_buffer_avail(efb_ptr) == 4*1024*1024);

    etch_flex_buffer_destroy(efb_ptr);
}

void testCompact(void)
{
    int i;
    etch_flex_buffer *efb_ptr = etch_flex_buffer_create_s(128);

    for(i = 0; i < 10; i++)
        etch_flex_buffer_put_short(efb_ptr, 1);

    etch_flex_buffer_set_index(efb_ptr, 0);
    etch_flex_buffer_compact(efb_ptr);
    CU_ASSERT(efb_ptr->index ==0);
    CU_ASSERT(etch_flex_buffer_avail(efb_ptr) == (10*sizeof(short)));

    etch_flex_buffer_destroy(efb_ptr);
}

int main(int argc, char **argv)
{
    CU_pSuite ps;
    CU_pTest pt;

    CU_initialize_registry();
    
    ps = CU_add_suite("flex buffer test suit", setup, teardown);
    pt = CU_add_test(ps, "testCreateAndDestroy0", testCreateAndDestroy0);
    pt = CU_add_test(ps, "testCreateAndDestroy1", testCreateAndDestroy1);
    pt = CU_add_test(ps, "testCreateAndDestroy2", testCreateAndDestroy2);
    pt = CU_add_test(ps, "testCreateAndDestroy3", testCreateAndDestroy3);
    pt = CU_add_test(ps, "testPutAndGet", testPutAndGet);
    pt = CU_add_test(ps, "testPutAndGetFully", testPutAndGetFully);
    pt = CU_add_test(ps, "testPutAndGetByte", testPutAndGetByte);
    pt = CU_add_test(ps, "testPutAndGetShort", testPutAndGetShort);
    pt = CU_add_test(ps, "testPutAndGetInt", testPutAndGetInt);
    pt = CU_add_test(ps, "testPutAndGetInt64", testPutAndGetInt64);
    pt = CU_add_test(ps, "testPutAndGetFloat", testPutAndGetFloat);
    pt = CU_add_test(ps, "testPutAndGetFloat", testPutAndGetDouble);
    pt = CU_add_test(ps, "testSetSize", testSetSize);
    pt = CU_add_test(ps, "testCompact", testCompact);

 /*   CU_console_run_tests(); */
    CU_automated_run_tests();
    
    CU_cleanup_registry();
    return 0;

}
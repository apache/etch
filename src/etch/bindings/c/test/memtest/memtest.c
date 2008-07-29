/**
 * memtest.c -- test the debug allocator 
 */

#include <tchar.h>
#include <stdio.h>
#include <conio.h>
#include "etchobj.h"
#include "etch_global.h"

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#define PAYLOADSIG 0xf00f00
#define NUMITEMS 3

typedef struct payload
{
    int count;
    int signature;
} payload;

/**
 * Initialization tasks for the test suite
 */
int init_suite_mem(void)
{
    return etch_init();
}

/**
 * Cleanup tasks for the test suite
 */
int clean_suite_mem(void)
{
    return etch_runtime_cleanup(0,0); /* free memtable and cache etc */
}


/* 
 * This subtest test our ability to build a hashtable each key of which is a
 * pointer to that key's value, in other words, the hashtable stores a void**
 * as they key. This essentially models a debug allocator. 
 */
void malloc_test(void)
{
    /* These are what is stored in the hash table for each hash, two pointers.
       To clarify, *pointers* are what is copied into the hashtable, not values.
       So it follows that if I want to use a pointer as a hash key, what I must
       store in the hashtable as the key is a pointer to that pointer.
    */
    char **pkey = 0; void *pval = 0; 
     
    payload** allocations[NUMITEMS];
    int i, testresult = 0, count = 0;
    etch_hashtable* myhash;
    etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket;
    /* printf("start pointer hash key subtest\n\n"); */

    do {

    is_memtable_instance = TRUE;
    myhash = new_hashtable(16);  /* create hash table */
    is_memtable_instance = FALSE;
    CU_ASSERT_PTR_NOT_NULL(myhash); 
    if (!myhash) break;

    /* insert NUMITEMS allocations into the tracking table
     */
    for(i = 0; i < NUMITEMS; i++)
    {
        int      result = 0;
        void*    pkey = 0;
        payload* pval = 0;
       
        pkey = malloc(sizeof(void*));        /* allocate the key*    */        
        pval = malloc(sizeof(payload));      /* allocate the value   */
        pval->count = i; pval->signature = PAYLOADSIG; /* debug info */
        memcpy(pkey, &pval, sizeof(void*));  /* copy value* into key */
        allocations[i] = pkey  ;             /* save for iterating   */

        result = myhash->vtab->insert(myhash->realtable, pkey, sizeof(void*), pval, sizeof(payload), 0, 0);
        CU_ASSERT_EQUAL(result,0);
                               
        if (0 != result)  
        {   printf("** hash of pointer key failed at %s line %d\n", __FILE__,__LINE__);
	        break;
	    } 

       /* printf("inserted test allocation %d\n", pval->count); */
    } 

    /* ensure there are NUMITEMS entries in the tracking table*/
    count = myhash->vtab->count(myhash->realtable, 0, 0);
    CU_ASSERT_EQUAL(count,NUMITEMS);
    if (count != NUMITEMS)
        break;

    for(i = 0; i < NUMITEMS; i++)
    {
        int       result = 0;
        payload*  pval = 0;
        payload** pkey = (payload**) &allocations[i]; 

        result = myhash->vtab->find(myhash->realtable, *pkey, sizeof(void*), NULL, &myentry); 
        CU_ASSERT_EQUAL(result,0);
        
 	    if  (0 != result) 
	    {	 printf("** lookup of pointer key failed at %s line %d\n", __FILE__,__LINE__);
	         break;
	    } 

        pval = (payload*)myentry->value;
        CU_ASSERT_EQUAL(pval->signature, PAYLOADSIG);
        CU_ASSERT_EQUAL(pval->count, i);
        if (pval->signature != PAYLOADSIG || pval->count != i)
	    {	 printf("** object of pointer key failed at %s line %d\n", __FILE__,__LINE__);
	         break;
	    }

        /* printf("found test allocation %d\n", pval->count); */
    } 

    testresult = TRUE;

    /* 
     * Clear the hashtable, while doing so ask hashtable to free memory at the 
     * and value pointers. Since we have individually allocated both keys (4 bytes
     * each, each a void**), and values (sizeof(payload) bytes each), we can do so. 
     * If we do not crash, this mini-test succeeds.
     * CUnit note: there is no easy way to test if this succeeds, since the hashtable
     * no longer exists if successful, and an attempt to reference it would elicit 
     * an exception.
     */
    myhash->vtab->clear(myhash->realtable, TRUE, TRUE, 0, 0);

    } while(0);

    /* if (testresult) printf("\nsubtest succeeded\n"); else printf("\nsubtest failed\n"); */
    /* printf("end pointer hash key subtest\n\n"); */
}


/* 
 * This subtest tests our implemented debug allocator. 
 */
void etch_malloc_test(void)
{
    /* These are what is stored in the hash table for each hash, two pointers.
       To clarify, *pointers* are what is copied into the hashtable, not values.
       So it follows that if I want to use a pointer as a hash key, what I must
       store in the hashtable as the key is a pointer to that pointer.
    */
     
    payload* allocations[NUMITEMS];
    int i, testresult = 0;
    etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket;
    /* printf("start debug allocator subtest\n\n"); */

    do {

    /* insert NUMITEMS allocations into the tracking table
     */
    for(i = 0; i < NUMITEMS; i++)
    {
        void*    pmem = 0;
        payload* pval = 0;

        pval = etch_malloc(sizeof(payload), 0); /* allocate value, get key */ 
        CU_ASSERT_PTR_NOT_NULL(pval); 
        if (pval == NULL)
        {   printf("** etch_malloc failed at %s line %d\n", __FILE__,__LINE__); 
            break;
        }      
        pval->count = i; pval->signature = PAYLOADSIG; /* debug info */
        allocations[i] = pval  ;   /* save for iterating */
        /* printf("OK etch_malloc'ed item %d\n", pval->count); */
    } 

    /* ensure there are NUMITEMS entries in the tracking table */
    CU_ASSERT_NOT_EQUAL(memtable, 0);
    if (!memtable) break;
    CU_ASSERT_EQUAL(memtable->vtab->count(memtable->realtable, 0, 0), NUMITEMS);   
    if (memtable->vtab->count(memtable->realtable, 0, 0) != NUMITEMS)
        break;

    for(i = 0; i < NUMITEMS; i++)
    {
        int n, result;
        payload* pval = (payload*) allocations[i]; 
        n = pval->count;

        result = etch_free(pval);        
        CU_ASSERT_EQUAL(result, 0);

 	    if  (0 != result)
	    {	 printf("** etch_free failed at %s line %d\n", __FILE__,__LINE__);
	         break;
	    } 

        /* printf("OK etch_freed item %d\n", n); */
    } 

    /* ensure there are zero entries in the tracking table */
    CU_ASSERT_NOT_EQUAL(memtable, 0);
    if (!memtable) break;
    CU_ASSERT_EQUAL(memtable->vtab->count(memtable->realtable, 0, 0), 0);
    if (memtable->vtab->count(memtable->realtable, 0, 0) != 0)
        break;

    testresult = TRUE;

    etch_showmem(TRUE, TRUE); /* display memory leaks and clean them up */
    memtable->vtab->clear(memtable->realtable, TRUE, TRUE, 0, 0);

    } while(0);

    /* if (testresult) printf("\nsubtest succeeded\n"); else printf("\nsubtest failed\n"); */
    /* printf("end debug allocator subtest\n\n"); */
}


/* 
 * This subtest tests that the debug allocator reports un-freed memory as expected.
 */
void negative_etch_malloc_test(void)
{
    struct 
    {   double d;
    } EIGHTBYTES;
    struct
    {   char x[496];
        int  n;
    } FIVEHUNDREDBYTES;

    void *p1=0, *p2=0;
    int   bytes_allocated = 0;
    memtable = NULL; /* ensure not dangling from a prior test */

    p1 = etch_malloc(sizeof(EIGHTBYTES), 0); 
    p2 = etch_malloc(sizeof(FIVEHUNDREDBYTES), 0);  

    bytes_allocated = etch_showmem(FALSE, TRUE); /* display leaks - don't free them */
    CU_ASSERT_EQUAL(bytes_allocated, sizeof(EIGHTBYTES) + sizeof(FIVEHUNDREDBYTES));
     
    etch_free(p2);
    bytes_allocated = etch_showmem(FALSE, TRUE); 
    CU_ASSERT_EQUAL(bytes_allocated, sizeof(EIGHTBYTES));

    etch_free(p1);
    bytes_allocated = etch_showmem(FALSE, TRUE); 
    CU_ASSERT_EQUAL(bytes_allocated, 0);
}

/* 
 * This subtest tests that the debug allocator reports un-freed memory as expected.
 */
void etch_realloc_test(void)
{
    struct 
    {   double d;
    } EIGHTBYTES;
    struct
    {   char x[496];
        int  n;
    } FIVEHUNDREDBYTES;

    void *p1=0, *p2=0;
    int   bytes_allocated = 0;
    memtable = NULL; /* ensure not dangling from a prior test */

    /* same as malloc */
    p1 = etch_realloc(NULL, sizeof(EIGHTBYTES), 0);
    CU_ASSERT_PTR_NOT_NULL(p1);
    bytes_allocated = etch_showmem(FALSE, TRUE); /* display leaks - don't free them */
    CU_ASSERT_EQUAL(bytes_allocated, sizeof(EIGHTBYTES));

    /* same as free */
    p2 = etch_realloc(p1, 0, 0);
    bytes_allocated = etch_showmem(FALSE, TRUE); /* display leaks - don't free them */
    CU_ASSERT_EQUAL(bytes_allocated, 0);

    /* now expand the memory */
    p1 = etch_realloc(NULL, sizeof(EIGHTBYTES), 0);
    CU_ASSERT_PTR_NOT_NULL(p1);
    bytes_allocated = etch_showmem(FALSE, TRUE); /* display leaks - don't free them */
    CU_ASSERT_EQUAL(bytes_allocated, sizeof(EIGHTBYTES));
    *((long *) p1) = 1234;

    p2 = etch_realloc(p1, sizeof(FIVEHUNDREDBYTES), 0);

    CU_ASSERT_PTR_NOT_NULL(p2);
    bytes_allocated = etch_showmem(FALSE, TRUE); /* display leaks - don't free them */
    CU_ASSERT_EQUAL(bytes_allocated, sizeof(FIVEHUNDREDBYTES));
    CU_ASSERT( (*( (long *) p2) ) == 1234 );

    etch_free(p2);


    /* now shrink memory */
    p1 = etch_realloc(NULL, sizeof(FIVEHUNDREDBYTES), 0);
    CU_ASSERT_PTR_NOT_NULL(p1);
    bytes_allocated = etch_showmem(FALSE, TRUE); /* display leaks - don't free them */
    CU_ASSERT_EQUAL(bytes_allocated, sizeof(FIVEHUNDREDBYTES));
    *((long *) p1) = 1234;

    p2 = etch_realloc(p1, sizeof(EIGHTBYTES), 0);

    CU_ASSERT_PTR_NOT_NULL(p2);
    bytes_allocated = etch_showmem(FALSE, TRUE); /* display leaks - don't free them */
    CU_ASSERT_EQUAL(bytes_allocated, sizeof(EIGHTBYTES));
    CU_ASSERT( (*( (long *) p2) ) == 1234 );

    etch_free(p2);

}

/**
 * main   
 */
int _tmain(int argc, _TCHAR* argv[])
{
    CU_pSuite pSuite = NULL;
    char c=0;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* set output XML file for automatic tests result */
    CU_set_output_filename("../test_etchmalloc");

    do {

    pSuite = CU_add_suite("suite_etchmalloc", init_suite_mem, clean_suite_mem);
    if (NULL == pSuite) 
        break;

    if (CU_add_test(pSuite, "hashtable pointer key test", malloc_test) == NULL)   
        break;

    if (CU_add_test(pSuite, "debug allocator test", etch_malloc_test) == NULL)   
        break;

    if (CU_add_test(pSuite, "debug realloc test", etch_realloc_test) == NULL)   
        break;

    if (CU_add_test(pSuite, "negative debug allocator test", negative_etch_malloc_test) == NULL)   
        break;

    if (argc == 2 && !wcscmp(argv[1], L"-a"))
    {
        CU_automated_run_tests();
    }
    else
    {
        CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    }
     
    } while(0);

    printf("any key ..."); while(!c) c = _getch(); wprintf(L"\n"); 
    CU_cleanup_registry();
    return CU_get_error(); 
}


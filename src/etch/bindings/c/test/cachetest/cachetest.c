/**
 * cachetest.c
 * test the runtime object cache
 * we can swap out cache back ends and this test should work the same regardless
 */

#include <conio.h>
#include "etch_global.h"


/**
 * This subtest instantiates various etch objects which cache some part of 
 * themselves, and destroys the objects. At each step the test verifies that
 * the cache contains the exepcted number of entries, e.g. if I create 
 * multiple hashtables I should only have cached one hashtable vtable.
 */
int multiple_item_subtest()
{
    int testresult = 0, cache_start_count = 0, cache_current_count;
    int result1 = 0, result2 = 0;
    etch_hashtable* myhashtab1 = NULL;    /* define a hashtable */
    etch_hashtable* myhashtab2 = NULL;    /* define a hashtable */
    etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket;

    wchar_t* wstr1 = L"abracadabra"; /* is L macro Windows only? */
    wchar_t* wstr2 = L"gilgamesh";

    const size_t numElements1 = wcslen(wstr1);
    const size_t numElements2 = wcslen(wstr2);

    const size_t numBytes1 = sizeof(wchar_t) * numElements1;
    const size_t numBytes2 = sizeof(wchar_t) * numElements2;

    size_t actlen1 = 0, actlen2 = 0;

    wchar_t *key1 = NULL, *key2 = NULL;
    wchar_t *foundkey1 = NULL,  *foundkey2 = NULL; 
    wprintf(L"begin multiple item subtest\n\n");

    do {

    key1 = malloc(numBytes1 + 2);   
    key2 = malloc(numBytes2 + 2);   
    if (key1 == NULL || key2 == NULL) break;
    cache_start_count = cache_count();
        
    result1 = wcscpy_s(key1, numElements1+1, wstr1);  /* NOTE 2: wcscpy_s param 2 is */
    result2 = wcscpy_s(key2, numElements1+1, wstr2);  /* number of characters + 1    */
    actlen1 = wcslen(key1);  actlen2 = wcslen(key2);

    myhashtab1 = new_hashtable(16);   /* create hash table 1 */
    myhashtab2 = new_hashtable(512);  /* create hash table 2 */

   /* we should only have cached 1 copy of hashtable vtab */
    cache_current_count = cache_count();
    assert(cache_current_count == cache_start_count + 1); 

    myhashtab1->vtab->insert(myhashtab1->realtable, key1, (int)numBytes1, NULL,0,0,0);            
    myhashtab2->vtab->insert(myhashtab2->realtable, key2, (int)numBytes2, NULL,0,0,0);

    /* TODO instantiate some other object here which uses the cache */
    /* debug_showmem(FALSE, TRUE); */

    destroy_hashtable(myhashtab1, TRUE, TRUE);
    destroy_hashtable(myhashtab2, TRUE, TRUE);
    //myhashtab1->vtab->clear(myhashtab1->realtable, TRUE, TRUE, 0, 0);
    //myhashtab2->vtab->clear(myhashtab2->realtable, TRUE, TRUE, 0, 0);
    /* note that key1 and key2 are now dangling pointers since we asked the
     * hashtable to free keys and values memory
     */
 
    testresult = TRUE;

    } while(0);

    debug_showmem(TRUE, TRUE);

    if (testresult) wprintf(L"\nsubtest succeeded\n"); else wprintf(L"\nsubtest failed\n");
    wprintf(L"end multiple item subtest\n\n");
	return testresult;
}


int test_intkeys()
{
    int i, startsize, size;
    const int STARTKEY = 0, ENDKEY = 512, KEYCOUNT = ENDKEY - STARTKEY;
    char* teststring = "it works!";
    char* item = malloc(sizeof(teststring));
    memcpy(item, teststring,sizeof(teststring)); 
    wprintf(L"begin consecutive intkey subtest\n\n");

    startsize = cache_count();

    for(i = STARTKEY; i < ENDKEY; i++)
        cache_add(i, item);

    size = cache_count();
       if (size != KEYCOUNT + startsize)
           wprintf(L"cache count %d not expected count %d\n", size, KEYCOUNT);

    for(i = STARTKEY; i < ENDKEY; i++)
        if (cache_find(i) == NULL)
            wprintf(L"cache key %d lookup failed\n", i);

    for(i = STARTKEY; i < ENDKEY; i++)
       if (cache_del(i) == NULL)
           wprintf(L"cache del %d failed\n", i);

    size = cache_count();
    if (size != startsize)
        wprintf(L"cache count %d not equal startcount %d\n", size, startsize);

    debug_showmem(TRUE, TRUE);
    free(item);

    wprintf(L"end consecutive intkey subtest\n\n");
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
    /* When _UNICODE is defined _tmain expands to wmain, otherwise main 
     * Note that we are creating a copy of the string below on the heap
     * in order to verify the wide character functionality, along with
     * testing the etch_id_name object.
     */
    char c=0;
    int  testresult = FALSE;
    wprintf(L"start cache test\n\n");

    etch_init();
   
    testresult = multiple_item_subtest();

    test_intkeys();

    etch_runtime_cleanup(0,0);
   
    if (testresult) wprintf(L"\ntest succeeded\n"); else wprintf(L"\ntest failed\n");
    wprintf(L"end cache test\n\n");
    wprintf(L"any key ..."); while(!c) c = _getch(); wprintf(L"\n"); 
	return testresult;
}


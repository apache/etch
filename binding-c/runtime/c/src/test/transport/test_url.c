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
 * test_url.c -- test etch_url
 */

#include "apr_time.h" /* some apr must be included first */
#include "etch_url.h"   
#include "etchmap.h"    

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#include "etchthread.h"
#include "etch_global.h"

#define IS_DEBUG_CONSOLE FALSE
int g_is_automated_test, g_bytes_allocated;

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

int init_suite(void)
{
    apr_setup();
    etch_runtime_init(TRUE);
    return this_setup();
}

int clean_suite(void)
{
    this_teardown();
    etch_runtime_cleanup(0,0); /* free memtable and cache etc */
    apr_teardown();
    return 0;
}

/*
 * apr_setup()
 * establish apache portable runtime environment
 */
int apr_setup(void)
{
    int result = apr_initialize();
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
 * tests
 * - - - - - - - - - - - - - -
 */

/* 
 * test_parse_1
 */
void test_parse_1(void)
{
    int result = 0;
    etch_url* url = NULL;
    wchar_t* RAWURL = L"www.cisco.com";

    url = new_url(RAWURL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(url); 

    CU_ASSERT_NOT_EQUAL(url->bytecount,0); 
    CU_ASSERT_NOT_EQUAL(url->charcount,0); 

    result = wcscmp(url->raw, RAWURL);
    CU_ASSERT_EQUAL(result,0);

    result = wcscmp(url->scheme, ETCH_URL_DEFAULT_SCHEME);
    CU_ASSERT_EQUAL(result,0);  

    result = wcscmp(url->host, RAWURL);
    CU_ASSERT_EQUAL(result,0); 

    url->destroy(url);

    g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/* 
 * test_parse_2
 */
void test_parse_2(void)
{
    int result = 0;
    etch_url* url = NULL;
    wchar_t* RAWURL = L"http://www.cisco.com/cuae";

    url = new_url(RAWURL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(url); 

    CU_ASSERT_NOT_EQUAL(url->bytecount,0); 
    CU_ASSERT_NOT_EQUAL(url->charcount,0); 

    result = wcscmp(url->raw, RAWURL);
    CU_ASSERT_EQUAL(result,0);

    result = wcscmp(url->scheme, L"http");
    CU_ASSERT_EQUAL(result,0);  

    result = wcscmp(url->host, L"www.cisco.com");
    CU_ASSERT_EQUAL(result,0); 

    result = wcscmp(url->uri, L"cuae");
    CU_ASSERT_EQUAL(result,0); 

    url->destroy(url);

    g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/* 
 * test_parse_3
 */
void test_parse_3(void)
{
    int result = 0;
    etch_url* url = NULL;
    wchar_t* RAWURL = L"http://www.cisco.com:8080/cuae?param";

    url = new_url(RAWURL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(url); 

    CU_ASSERT_NOT_EQUAL(url->bytecount,0); 
    CU_ASSERT_NOT_EQUAL(url->charcount,0); 

    result = wcscmp(url->raw, RAWURL);
    CU_ASSERT_EQUAL(result,0);

    result = wcscmp(url->scheme, L"http");
    CU_ASSERT_EQUAL(result,0);  

    result = wcscmp(url->host, L"www.cisco.com");
    CU_ASSERT_EQUAL(result,0); 

    result = url->port == 8080;
    CU_ASSERT_EQUAL(result,TRUE); 

    result = wcscmp(url->uri, L"cuae");
    CU_ASSERT_EQUAL(result,0); 

    url->destroy(url);

    g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/* 
 * test_parse_4
 */
void test_parse_4(void)
{
    int result = 0;
    etch_url* url = NULL;
    wchar_t* RAWURL = L"http://administrator:metreos@www.cisco.com:8080/cuae?param";

    url = new_url(RAWURL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(url); 

    CU_ASSERT_NOT_EQUAL(url->bytecount,0); 
    CU_ASSERT_NOT_EQUAL(url->charcount,0); 

    result = wcscmp(url->raw, RAWURL);
    CU_ASSERT_EQUAL(result,0);

    result = wcscmp(url->scheme, L"http");
    CU_ASSERT_EQUAL(result,0);  

    result = wcscmp(url->user, L"administrator");
    CU_ASSERT_EQUAL(result,0); 

    result = wcscmp(url->password, L"metreos");
    CU_ASSERT_EQUAL(result,0); 

    result = wcscmp(url->host, L"www.cisco.com");
    CU_ASSERT_EQUAL(result,0); 

    result = url->port == 8080;
    CU_ASSERT_EQUAL(result,TRUE); 

    result = wcscmp(url->uri, L"cuae");
    CU_ASSERT_EQUAL(result,0); 

    url->destroy(url);

    g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/* 
 * test_parse_5
 * since colon is both scheme and username:password delimiter
 * ensure we can omit scheme and include password
 */
void test_parse_5(void)
{
    int result = 0;
    etch_url* url = NULL;
    wchar_t* RAWURL = L"administrator:metreos@www.cisco.com:8080/cuae?param";

    url = new_url(RAWURL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(url); 

    CU_ASSERT_NOT_EQUAL(url->bytecount,0); 
    CU_ASSERT_NOT_EQUAL(url->charcount,0); 

    result = wcscmp(url->raw, RAWURL);
    CU_ASSERT_EQUAL(result,0);

    result = wcscmp(url->scheme, L"http");
    CU_ASSERT_EQUAL(result,0);  

    result = wcscmp(url->user, L"administrator");
    CU_ASSERT_EQUAL(result,0); 

    result = wcscmp(url->password, L"metreos");
    CU_ASSERT_EQUAL(result,0); 

    result = wcscmp(url->host, L"www.cisco.com");
    CU_ASSERT_EQUAL(result,0); 

    result = url->port == 8080;
    CU_ASSERT_EQUAL(result,TRUE); 

    result = wcscmp(url->uri, L"cuae");
    CU_ASSERT_EQUAL(result,0); 

    url->destroy(url);

    g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/* 
 * test_parse_6
 */
void test_parse_6(void)
{
    int result = 0;
    etch_url* url = NULL;
    wchar_t* RAWURL = L"tcp://administrator:metreos@localhost:10000/defUri;param1;param2?term1=true&term2=false#defFragment";

    url = new_url(RAWURL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(url); 

    CU_ASSERT_NOT_EQUAL(url->bytecount,0); 
    CU_ASSERT_NOT_EQUAL(url->charcount,0); 

    result = wcscmp(url->raw, RAWURL);
    CU_ASSERT_EQUAL(result,0);

    result = wcscmp(url->scheme, L"tcp");
    CU_ASSERT_EQUAL(result,0);  

    result = wcscmp(url->user, L"administrator");
    CU_ASSERT_EQUAL(result,0); 

    result = wcscmp(url->password, L"metreos");
    CU_ASSERT_EQUAL(result,0); 

    result = wcscmp(url->host, L"localhost");
    CU_ASSERT_EQUAL(result,0); 

    result = url->port == 10000;
    CU_ASSERT_EQUAL(result,TRUE); 

    result = wcscmp(url->uri, L"defUri");
    CU_ASSERT_EQUAL(result,0); 

    result = etchurl_paramcount(url);
    CU_ASSERT_EQUAL(result,2); 

    result = etchurl_termcount(url);
    CU_ASSERT_EQUAL(result,2);

    do 
    {   etch_iterator* iterator = etchurl_get_params(url);
        CU_ASSERT_PTR_NOT_NULL_FATAL(iterator);

        while(iterator->has_next(iterator))
        {   int matches = 0;
            etch_string* val = (etch_string*) iterator->current_value;
            wchar_t* param = val? val->v.valw: NULL;
            if (param && 0 == wcscmp(param, L"param1")) matches++;
            if (param && 0 == wcscmp(param, L"param2")) matches++;
            CU_ASSERT_EQUAL(matches,1); 
            iterator->next(iterator);
        }
        iterator->destroy(iterator);
    } while(0);

    do 
    {   etch_iterator iterator;
        set_iterator(&iterator, url->terms, &url->terms->iterable);

        while(iterator.has_next(&iterator))
        {   int keymatches = 0, valmatches = 0;
            wchar_t* key = (wchar_t*) iterator.current_key;
            etch_string* valobj = (etch_string*) iterator.current_value;
            wchar_t* val = valobj? valobj->v.valw: NULL;
            if (key && 0 == wcscmp(key, L"term1")) keymatches++;
            if (key && 0 == wcscmp(key, L"term2")) keymatches++;
            CU_ASSERT_EQUAL(keymatches,1); 
            if (val && 0 == wcscmp(val, L"false")) valmatches++;
            if (val && 0 == wcscmp(val, L"true"))  valmatches++;
            CU_ASSERT_EQUAL(valmatches,1); 
            iterator.next(&iterator);
        }
    } while(0);

    url->destroy(url);

    g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/* 
 * test_parse_7
 * test a url with duplicate term names
 */
void test_parse_7(void)
{
    int result = 0;
    etch_url* url = NULL;
    wchar_t* RAWURL = L"tcp://administrator:metreos@localhost:10000/defUri;param1;param2?term1=true&term1=false";

    url = new_url(RAWURL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(url); 

    CU_ASSERT_NOT_EQUAL(url->bytecount,0); 
    CU_ASSERT_NOT_EQUAL(url->charcount,0); 

    result = wcscmp(url->raw, RAWURL);
    CU_ASSERT_EQUAL(result,0);

    result = wcscmp(url->scheme, L"tcp");
    CU_ASSERT_EQUAL(result,0);  

    result = wcscmp(url->user, L"administrator");
    CU_ASSERT_EQUAL(result,0); 

    result = wcscmp(url->password, L"metreos");
    CU_ASSERT_EQUAL(result,0); 

    result = wcscmp(url->host, L"localhost");
    CU_ASSERT_EQUAL(result,0); 

    result = url->port == 10000;
    CU_ASSERT_EQUAL(result,TRUE); 

    result = wcscmp(url->uri, L"defUri");
    CU_ASSERT_EQUAL(result,0); 

    result = etchurl_paramcount(url);
    CU_ASSERT_EQUAL(result,2); 

    /* since terms had same name, the two terms should now be collected into a set, 
     * therefore the count (at top level) should be 1. this is of course based
     * on our omniscient knowledge of the url content */
    result = etchurl_termcount(url);
    CU_ASSERT_EQUAL(result,1); 

    do  /* iterate all params */
    {   etch_iterator* iterator = etchurl_get_params(url);
        CU_ASSERT_PTR_NOT_NULL_FATAL(iterator);

        while(iterator->has_next(iterator))
        {   int matches = 0;
            etch_string* val = (etch_string*) iterator->current_value;
            wchar_t* param = val? val->v.valw: NULL;
            if (param && 0 == wcscmp(param, L"param1")) matches++;
            if (param && 0 == wcscmp(param, L"param2")) matches++;
            CU_ASSERT_EQUAL(matches,1); 

            iterator->next(iterator);
        }

        iterator->destroy(iterator);

    } while(0);

    do  /* iterate all terms */
    {   etch_iterator iterator1;
        set_iterator(&iterator1, url->terms, &url->terms->iterable);

        /* this is hard coded base on our knowledge of the url content. 
         * there should be one term in the term map with the value "term1",
         * and its value will be a set whose two members are etch_string 
         * objects having values of "true" and "false"
         */
        while(iterator1.has_next(&iterator1))
        {    
            etch_iterator iterator2;
            wchar_t*  key    = (wchar_t*) iterator1.current_key;
            etch_set* setobj = (etch_set*)iterator1.current_value;
            CU_ASSERT_EQUAL_FATAL(is_etch_set(setobj), TRUE);

            result = etchmap_count(setobj);
            CU_ASSERT_EQUAL(result, 2);

            set_iterator(&iterator2, setobj, &setobj->iterable);

            while(iterator2.has_next(&iterator2))
            {
                int matches = 0;
                wchar_t* val = 0;
                etch_string* setmember = iterator2.current_key;
                CU_ASSERT_EQUAL_FATAL(is_etch_string(setmember), TRUE);
                val = setmember->v.valw;
                if (val && 0 == wcscmp(val, L"true"))  matches++;
                if (val && 0 == wcscmp(val, L"false")) matches++;
                CU_ASSERT_EQUAL(matches,1); 

                iterator2.next(&iterator2);
            }
            
            iterator1.next(&iterator1);
        }

    } while(0);

    url->destroy(url);

    g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
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
    CU_set_output_filename("../test_url");
    pSuite = CU_add_suite("suite_url", init_suite, clean_suite);
    etch_watch_id = 0;
    etch_watch_addr = 0; // 0x4421c8;

    CU_add_test(pSuite, "test parse 1", test_parse_1); 
    CU_add_test(pSuite, "test parse 2", test_parse_2); 
    CU_add_test(pSuite, "test parse 3", test_parse_3); 
    CU_add_test(pSuite, "test parse 4", test_parse_4); 
    CU_add_test(pSuite, "test parse 5", test_parse_5);
    CU_add_test(pSuite, "test parse 6", test_parse_6);
    CU_add_test(pSuite, "test parse 7", test_parse_7);

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


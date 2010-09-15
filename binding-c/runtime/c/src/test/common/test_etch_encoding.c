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
 * test_encoding.c -- test etch encoding
 */
#include "etch_runtime.h"
#include "etch_encoding.h"
#include "etch_mem.h"

#include "CUnit.h"
#include <stdio.h>
#include <wchar.h>

#define IS_DEBUG_CONSOLE FALSE

// extern types
extern apr_pool_t* g_etch_main_pool;

static int init_suite(void)
{
    etch_status_t etch_status = ETCH_SUCCESS;

    etch_status = etch_runtime_initialize(NULL);
    if(etch_status != NULL) {
        // error
    }
    return 0;
}

static int clean_suite(void)
{
    //this_teardown();
    etch_runtime_shutdown();
    return 0;
}
/*
static void test_etch_encoding_unicode_to_utf8(void)
{
    int result = 0;
    char* out = NULL;

    result = etch_encoding_unicode_to_utf8(&out, L"U\u00A9");
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_PTR_NOT_NULL(out);
    if(out != NULL)
    {
        CU_ASSERT_EQUAL(out[0], 'U');
        CU_ASSERT_EQUAL(out[1], (char)0xc2);
        CU_ASSERT_EQUAL(out[2], (char)0xa9);
    }
    if(out != NULL)
    {
        etch_free(out);
        out = NULL;
    }
}

static void test_etch_encoding_unicode_to_ansi(void)
{
    int result = 0;
    char* out = NULL;

    result = etch_encoding_unicode_to_ansi(&out, L"UNICODE String");
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_PTR_NOT_NULL(out);
    CU_ASSERT_STRING_EQUAL(out, "UNICODE String");
    if(out != NULL)
    {
        etch_free(out);
        out = NULL;
    }
}

static void test_etch_encoding_utf8_to_unicode(void)
{
    int result = 0;
    char in[] = {'U', 0xc2, 0xa9, 0x00};
    wchar_t* out = NULL;

    result = etch_encoding_utf8_to_unicode(&out, in);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_PTR_NOT_NULL(out);
    if(out != NULL)
    {
        CU_ASSERT_EQUAL(out[0], L'U');
        CU_ASSERT_EQUAL(out[1], L'\u00A9');
    }
    if(out != NULL)
    {
        etch_free(out);
        out = NULL;
    }
    if(out != NULL)
    {
        etch_free(out);
        out = NULL;
    }
}

static void test_etch_encoding_ansi_to_unicode(void)
{
    int result = 0;
    wchar_t* out = NULL;

    result = etch_encoding_ansi_to_unicode(&out, "UNICODE");
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_PTR_NOT_NULL(out);
    CU_ASSERT_EQUAL(wcscmp(out, L"UNICODE"), 0);
    if(out != NULL)
    {
        etch_free(out);
        out = NULL;
    }
    if(out != NULL)
    {
        etch_free(out);
        out = NULL;
    }
}

static void test_etch_encoding_get_unicode_bytecount(void)
{
    size_t count1 = 0;
    size_t count2 = 0;
    wchar_t* str = L"UNICODE";
    count1 = (wcslen(str) + 1) * sizeof(wchar_t);

    count2 = etch_encoding_get_unicode_bytecount(str);
    CU_ASSERT_EQUAL(count1, count2);

}
*/
static void test_etch_encoding_transcode_ucs2_utf8(void)
{

  char* out = 0;
  unsigned char outEncoding = ETCH_ENCODING_UTF8;
  const char in[] = {'a', 0,
                     'b', 0,
                     'c', 0,
                       0, 0};
  unsigned char inEncoding = ETCH_ENCODING_UCS2;
  unsigned int inByteCount = 4*2;
  int outBytes;
  // TODO: pool
  CU_ASSERT_EQUAL(0, etch_encoding_transcode(&out, outEncoding, in, inEncoding, inByteCount, &outBytes, NULL));
  CU_ASSERT(out != NULL);
  if(out){
      CU_ASSERT_EQUAL('a', out[0]);
      CU_ASSERT_EQUAL('b', out[1]);
      CU_ASSERT_EQUAL('c', out[2]);
      CU_ASSERT_EQUAL(0,   out[3]);
      CU_ASSERT_EQUAL(4, outBytes);
      etch_free(out);
  }
}

static void test_etch_encoding_transcode_ucs4_utf8(void)
{

  char* out = 0;
  unsigned char outEncoding = ETCH_ENCODING_UTF8;
  const char in[] = {'a', 0, 0, 0,
                     'b', 0, 0, 0,
                     'c', 0, 0, 0,
                       0, 0, 0, 0};
  unsigned char inEncoding = ETCH_ENCODING_UCS4;
  unsigned int inByteCount = 4*4;
  int outBytes;
  // TODO: pool
  CU_ASSERT_EQUAL(0, etch_encoding_transcode(&out, outEncoding, in, inEncoding, inByteCount, &outBytes, NULL));
  CU_ASSERT(out != NULL);
  if(out){
      CU_ASSERT_EQUAL('a', out[0]);
      CU_ASSERT_EQUAL('b', out[1]);
      CU_ASSERT_EQUAL('c', out[2]);
      CU_ASSERT_EQUAL(0,   out[3]);
      CU_ASSERT_EQUAL(4, outBytes);
      etch_free(out);
  }
}


static void test_etch_encoding_transcode_wchar_utf8(void) {
  char* out = 0;
  unsigned char outEncoding = ETCH_ENCODING_UTF8;
  wchar_t *in = L"abc";
  // TODO: pool
  CU_ASSERT_EQUAL(0, etch_encoding_transcode_wchar(&out, outEncoding, in, NULL));
  CU_ASSERT(out != NULL);
  if(out){
      CU_ASSERT_EQUAL('a', out[0]);
      CU_ASSERT_EQUAL('b', out[1]);
      CU_ASSERT_EQUAL('c', out[2]);
      CU_ASSERT_EQUAL(0,   out[3]);
      etch_free(out);
  }
}

CU_pSuite test_etch_encoding_suite(void)
{
    CU_pSuite ps = CU_add_suite("etch_encoding", init_suite, clean_suite);
    /*
    CU_add_test(ps, "test etch_encoding_unicode_to_utf8", test_etch_encoding_unicode_to_utf8);
    CU_add_test(ps, "test etch_encoding_unicode_to_ansi", test_etch_encoding_unicode_to_ansi);
    CU_add_test(ps, "test etch_encoding_utf8_to_unicode", test_etch_encoding_utf8_to_unicode);
    CU_add_test(ps, "test etch_encoding_ansi_to_unicode", test_etch_encoding_ansi_to_unicode);
    CU_add_test(ps, "test etch_encoding_get_unicode_bytecount", test_etch_encoding_get_unicode_bytecount);
    */
    CU_add_test(ps, "test etch_encoding_transcode_ucs2_utf8", test_etch_encoding_transcode_ucs2_utf8);
    CU_add_test(ps, "test etch_encoding_transcode_ucs4_utf8", test_etch_encoding_transcode_ucs4_utf8);
    CU_add_test(ps, "test etch_encoding_transcode_wchar_utf8", test_etch_encoding_transcode_wchar_utf8);

    return ps;
}

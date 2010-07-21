/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#include "test_all.h"

CU_pSuite test_etchinternal_single_linked_list();
CU_pSuite test_arraylist_suite();
CU_pSuite test_cache_suite();
CU_pSuite test_etch_arrayvalue_suite();
CU_pSuite test_etch_binarytdit_suite();
CU_pSuite test_etch_config_suite();
CU_pSuite test_etch_defvaluefact_suite();
CU_pSuite test_etch_encoding_suite();
CU_pSuite test_etch_field_suite();
CU_pSuite test_etch_flexbuf_suite();
CU_pSuite test_etch_hashing_suite();
CU_pSuite test_etch_hashtable_suite();
CU_pSuite test_etch_idname_suite();
CU_pSuite test_etch_iterator_suite();
CU_pSuite test_etch_log_suite();
CU_pSuite test_etch_mailboxmgr_suite();
CU_pSuite test_etch_message_suite();
CU_pSuite test_etch_messagizer_suite();
CU_pSuite test_etch_mutex_suite();
CU_pSuite test_etch_packetizer_suite();
CU_pSuite test_etch_plainmailbox_suite();
CU_pSuite test_etch_queue();
CU_pSuite test_etch_remote_suite();
CU_pSuite test_etch_runtime_suite();
CU_pSuite test_etch_structvalue_suite();
CU_pSuite test_etch_stub_suite();
CU_pSuite test_etch_suite();
CU_pSuite test_etch_tcpconn_suite();
CU_pSuite test_etch_thread_suite();
CU_pSuite test_etch_threadpool_suite();
CU_pSuite test_etch_transport_suite();
CU_pSuite test_etch_type_suite();
CU_pSuite test_etch_url_suite();
CU_pSuite test_etch_validator_suite();
CU_pSuite test_etch_wait_suite();
CU_pSuite test_etchobject_suite();
CU_pSuite test_etch_linked_list_suite();

suite_creator etch_testlist[] = {
    test_etch_tcpconn_suite,
	test_etch_linked_list_suite,
    test_etchinternal_single_linked_list,
    test_arraylist_suite,
    test_cache_suite,
    test_etch_arrayvalue_suite,
    test_etch_binarytdit_suite,
    test_etch_config_suite,
    test_etch_defvaluefact_suite,
    test_etch_encoding_suite,
    test_etch_field_suite,
    test_etch_flexbuf_suite,
    test_etch_hashing_suite,
    test_etch_hashtable_suite,
    test_etch_idname_suite,
    test_etch_iterator_suite,
    test_etch_log_suite,
    test_etch_message_suite,
    test_etch_messagizer_suite,
    test_etch_mutex_suite,
    test_etch_packetizer_suite,
    test_etch_queue,
    test_etch_remote_suite,
    test_etch_runtime_suite,
    test_etch_structvalue_suite,
    test_etch_stub_suite,
    test_etch_suite,
    test_etch_thread_suite,
    test_etch_threadpool_suite,
    test_etch_transport_suite,
    test_etch_type_suite,
    test_etch_url_suite,
    test_etch_validator_suite,
    test_etchobject_suite,
    test_etch_wait_suite,
    test_etch_mailboxmgr_suite,
    test_etch_plainmailbox_suite,
    0
};


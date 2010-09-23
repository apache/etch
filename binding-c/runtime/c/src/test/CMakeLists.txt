#
# Licensed to the Apache Software Foundation (ASF) under one or more 
# contributor license agreements. See the NOTICE file distributed with  
# this work for additional information regarding copyright ownership. 
# The ASF licenses this file to you under the Apache License, Version  
# 2.0 (the "License"); you may not use this file except in compliance  
# with the License. You may obtain a copy of the License at 
# 
# http://www.apache.org/licenses/LICENSE-2.0 
# 
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and 
# limitations under the License. 
#

# set include dirs
include_directories (${APR_INCLUDE_DIR})
include_directories (${APR-ICONV_INCLUDE_DIR})
include_directories (${CUNIT_INCLUDE_DIR})
include_directories (${PROJECT_SOURCE_DIR}/src/extern/jenkhash)
include_directories (${PROJECT_SOURCE_DIR}/include)

add_executable(etch-c-test 
    main.c
    test_all.c
    common/test_iterator.c
    common/test_hashtable.c
    common/test_hashing.c
    common/test_flexbuf.c
    common/test_etchobject.c
    common/test_etch_wait.c
    common/test_etch_thread.c
    common/test_etch_runtime.c
    common/test_etch_mutex.c
    common/test_etch_log.c
    common/test_etch_linked_list.c
    common/test_etch_encoding.c
    common/test_etch_config.c
    common/test_etch.c
    common/test_cache.c
    common/test_arraylist.c
    internal/test_etchinternal_single_linked_list.c
    message/test_validator.c
    message/test_type.c
    message/test_structvalue.c
    message/test_message.c
    message/test_idname.c
    message/test_field.c
    message/test_defvalufact.c
    message/test_binarytditdo.c
    message/test_arrayvalue.c
    support/test_stub.c
    support/test_remote.c
    transport/test_url.c
    transport/test_transport.c
    transport/test_threadpool.c
    transport/test_tcpconn.c
    transport/test_queue.c
    transport/test_plainmailbox.c
    transport/test_packetizer.c
    transport/test_messagizer.c
    transport/test_mailboxmgr.c
)

target_link_libraries (etch-c-test etch-c)

IF (UNIX)
set_target_properties(etch-c-test PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
ENDIF (UNIX)


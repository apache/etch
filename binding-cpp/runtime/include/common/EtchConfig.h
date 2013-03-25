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

#ifndef __ETCHCONFIG_H__
#define __ETCHCONFIG_H__

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "capu/Config.h"

/**
 * Hash table sizes
 */
#define ETCH_DEFAULT_HASH_TABLE_BIT_SIZE                   4
#define ETCH_DEFAULT_TYPEMAP_HASH_BIT_SIZE                 4
#define ETCH_DEFAULT_C2TYPEMAP_HASH_BIT_SIZE               4
#define ETCH_DEFAULT_FIELDMAP_HASH_BIT_SIZE                4
#define ETCH_DEFAULT_STRUCTVALUE_HASH_BIT_SIZE             4
#define ETCH_DEFAULT_TYPEVALIDATOR_HASH_BIT_SIZE           4
#define ETCH_DEFAULT_CUSTOMVALIDATORCACHE_HASH_BIT_SIZE    8
#define ETCH_DEFAULT_MAILBOXMANAGER_HASH_BIT_SIZE          4
#define ETCH_DEFAULT_URL_TERMS_HASH_BIT_SIZE               4
#define ETCH_DEFAULT_RESOURCES_HASH_BIT_SIZE               4

/**
 * Socket input buffer size (bytes)
 */
#define ETCH_DEFAULT_SOCKET_INPUT_BUFFER_SIZE 8192

#endif

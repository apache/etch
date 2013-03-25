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

#ifndef __ETCHUTIL_H__
#define __ETCHUTIL_H__

#include "common/EtchError.h"
#include "common/EtchConfig.h"

/*
 * copy src to dst. Copies at most dst_size bytes. In case that
 * dst is not long enough, dst will be '\0'.
 * Asserts that dstSize is length of dst.
 */
status_t etch_strcpy_s(char *dst, size_t dstSize, const char *src);

/**
 * calculate the length of given string which is encoded in UTF8
 * @param src string in UTF8
 * @param length as out param
 * @return ETCH_OK if length is successfully calculated
 *         ETCH_ERROR if length calculation has failed
 *         ETCH_EINVAL if src == NULL
 */
status_t etch_strlen_utf8(const char *src, capu::uint32_t &length);

#endif

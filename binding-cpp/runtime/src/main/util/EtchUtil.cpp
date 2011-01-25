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

#include "util/EtchUtil.h"

status_t etch_strcpy_s(char *dst, size_t dstSize, const char *src) {
    char *d, *end;

    if (dst == NULL || src == NULL) {
        return ETCH_EINVAL;
    }

    if (dstSize == 0) {
        return ETCH_ERANGE;
    }

    d = dst;
    end = dst + dstSize - 1;

    for (; d < end; ++d, ++src) {
        if (!(*d = *src)) {
            return ETCH_OK;
        }
    }

    // if src buffer is not 0 now, then we have bytes left in src --> error
    if(! (*d = *src)) {
        return ETCH_OK;
    } else {
        // always null terminate
        *d = '\0';
        return ETCH_EINVAL;
    }
}

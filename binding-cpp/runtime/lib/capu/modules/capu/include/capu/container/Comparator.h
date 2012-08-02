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

#ifndef __COMPARATOR_H__
#define __COMPARATOR_H__

#include "capu/Config.h"

namespace capu {

    class Comparator {
  
    public:
        template <class T>
        bool_t operator () (const T &x, const T &y) const {
            return x == y;
        }

        bool_t operator () (const char* x, const char* y) const {
            return (strcmp(x,y) == 0);
        }

        bool_t operator () (char* x, char* y) const {
            return (strcmp(x,y) == 0);
        }
    };
}

#endif /* COMPARATOR_H */


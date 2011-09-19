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

ADD_MODULE(capu static)

ADD_CLASS(os/Mutex SOURCE_GROUP arch_source_group)
ADD_CLASS(os/AtomicOperation SOURCE_GROUP arch_source_group)
ADD_CLASS(util/SmartPointer)
ADD_CLASS(os/Thread SOURCE_GROUP arch_source_group)
ADD_CLASS(os/CondVar SOURCE_GROUP arch_source_group)

REQUIRED_PACKAGE(Thread)

SOURCE_GROUP(${TARGET_ARCH} FILES  ${arch_source_group})

JUST_DOIT()

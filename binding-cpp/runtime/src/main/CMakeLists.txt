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
include_directories (${PROJECT_SOURCE_DIR}/include)

add_library (etch-cpp STATIC
    common/EtchObject.cpp
    common/EtchString.cpp
    common/EtchInt32.cpp
    common/EtchBool.cpp
    common/EtchByte.cpp
    common/EtchShort.cpp
    common/EtchLong.cpp
    common/EtchFloat.cpp
    common/EtchDouble.cpp
    util/EtchUtil.cpp
)

IF (UNIX)
  set_target_properties (etch-cpp PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
ENDIF (UNIX)

set_target_properties (etch-cpp PROPERTIES OUTPUT_NAME "etch")


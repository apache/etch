#!/usr/local/bin/bash
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements. See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership. The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License. You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied. See the License for the
# specific language governing permissions and limitations
# under the License.

#set -x
set -e

#echo etch $*
#exit

# This script is for the support of the various launch configurations
# used with eclipse.

if test -z "$ETCH_DEPENDENT_JARS"; then
echo Environment variable ETCH_DEPENDENT_JARS not set.
exit 1
fi

ETCH_HOME=`dirname "$0"`/..

if test \! -d "$ETCH_HOME/bin/etch"; then
#echo Cannot find etch classes.
exit 1
fi

#echo java -cp "$ETCH_HOME/bin:$ETCH_DEPENDENT_JARS" etch.compiler.EtchMain ${1+"$@"}
echo etch ${1+"$@"}
java -cp "$ETCH_HOME/bin:$ETCH_DEPENDENT_JARS" etch.compiler.EtchMain ${1+"$@"}

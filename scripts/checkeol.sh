#!/bin/sh
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

# Script to check for native eol setting in subversion

eol=`svn pg svn:eol-style $1`
if [ $? -ne 0 ]; then
  exit 1
fi

cr=$'\r'

#echo "x${eol}x" | od -c
#echo "xnative${cr}x" | od -c

if [ "x${eol}x" = "xnative${cr}x" ]; then
  exit 0
fi

if [ "x${eol}x" = "xnativex" ]; then
  exit 0
fi

exit 1

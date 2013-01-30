#!/usr/local/bin/bash
#  Licensed to the Apache Software Foundation (ASF) under one   *
#  or more contributor license agreements.  See the NOTICE file *
#  distributed with this work for additional information        *
#  regarding copyright ownership.  The ASF licenses this file   *
#  to you under the Apache License, Version 2.0 (the            *
#  "License"); you may not use this file except in compliance   *
#  with the License.  You may obtain a copy of the License at   *
#                                                               *
#    http://www.apache.org/licenses/LICENSE-2.0                 *
#                                                               *
#  Unless required by applicable law or agreed to in writing,   *
#  software distributed under the License is distributed on an  *
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY       *
#  KIND, either express or implied.  See the License for the    *
#  specific language governing permissions and limitations      *
#  under the License.                                           *

set -e
#set -x

q=-q
b="-b java"
d="-d ../../../target/generated-sources/main/etch/java"
i="-I ."
n=-n

walf="-w all,force"
 
testsdir=tests/src/main/etch
examplesdir=examples

x=`pwd`/

pushd $testsdir > /dev/null
${x}scripts/etch-eclipse.bash $q $b $n $d $i        Async.etch
${x}scripts/etch-eclipse.bash $q $b $n $d $i        Bar.etch
${x}scripts/etch-eclipse.bash $q $b $n $d $i        Baz.etch
${x}scripts/etch-eclipse.bash $q $b $n $d $i        BigIdl.etch
${x}scripts/etch-eclipse.bash $q $b $n $d $i        Closing.etch
${x}scripts/etch-eclipse.bash $q $b $n $d $i        Cuae.etch
${x}scripts/etch-eclipse.bash $q $b $n $d $i        Foo.etch
${x}scripts/etch-eclipse.bash $q $b $n $d $i        Inheritance.etch
${x}scripts/etch-eclipse.bash $q $b $n $d $i $walf  Test1.etch
${x}scripts/etch-eclipse.bash $q $b $n $d $i        Test2.etch
${x}scripts/etch-eclipse.bash $q $b $n $d $i        Test3.etch
${x}scripts/etch-eclipse.bash $q $b $n $d $i        Test4.etch
# *** Test5.etch is a negative test ***
${x}scripts/etch-eclipse.bash $q $b $n $d $i        Test6.etch
${x}scripts/etch-eclipse.bash $q $b $n $d $i $walf  TestIncludes.etch
# *** TestReservedWords.etch is a negative test ***
${x}scripts/etch-eclipse.bash $q $b $n $d $i        Types.etch
popd > /dev/null

pushd $examplesdir > /dev/null

pushd chat/src/main/etch > /dev/null
${x}scripts/etch-eclipse.bash $q $b $n $d $i        Chat.etch
popd > /dev/null

pushd distmap/src/main/etch > /dev/null
${x}scripts/etch-eclipse.bash $q $b $n $d $i        DistributedHashTable.etch
popd > /dev/null

pushd example/src/main/etch > /dev/null
${x}scripts/etch-eclipse.bash $q $b $n $d $i        Example.etch
popd > /dev/null

pushd perf/src/main/etch > /dev/null
${x}scripts/etch-eclipse.bash $q $b $n $d $i        Perf.etch
popd > /dev/null

popd > /dev/null

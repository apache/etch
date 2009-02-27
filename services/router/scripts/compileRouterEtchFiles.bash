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

q=-q
b="-b java"
b_xml="-b xml"
d="-d ../../../target/generated-sources/main/etch/java"
d_test="-d ../../../target/generated-sources/test/etch/java"
d_xml="-d ../../../target/generated-sources/main/etch/xml"
d_xml_test="-d ../../../target/generated-sources/test/etch/xml"
i="-I ."
n=-n
namespace_path="org/apache/etch/services/router"

walf="-w all,force"
 
testsdir=tests/src/main/etch
examplesdir=examples

x=`pwd`/../../../

pushd ../src/main/etch > /dev/null
${x}scripts/etch-eclipse.bat $q $b $n $d $i -w server    EtchRouter.etch
${x}scripts/etch-eclipse.bat $q $b_xml $n $d_xml $i -w both,all    EtchRouter.etch
popd > /dev/null

pushd ../src/test/etch > /dev/null
${x}scripts/etch-eclipse.bat $q $b $n $d_test $i -w client,all    TestApp.etch
${x}scripts/etch-eclipse.bat $q $b_xml $n $d_xml_test $i -w both,all       TestApp.etch

${x}scripts/etch-eclipse.bat $q $b $n $d_test $i -w both,all       TestPlugin.etch
${x}scripts/etch-eclipse.bat $q $b_xml $n $d_xml_test $i -w both,all       TestPlugin.etch

rm ../../../target/generated-sources/test/etch/java/${namespace_path}/test/plugin/ImplTestPluginServer.java

mkdir ../../../target/runtime/applications
mkdir ../../../target/runtime/plugins/TestPlugin
cp ../../../target/generated-sources/main/etch/xml/${namespace_path}/EtchRouter.xml ../../../target/runtime/
cp ../../../target/generated-sources/test/etch/xml/${namespace_path}/test/plugin/TestPlugin.xml ../../../target/runtime/plugins/TestPlugin/

popd > /dev/null

#!/usr/local/bin/bash

set -e
#set -x

q=-q
b="-b csharp"
d="-d ../../../target/generated-sources/main/etch/csharp"
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

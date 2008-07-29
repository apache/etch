#!/bin/sh

ant clean
find . -name 'CompilerVersion.java' -exec rm -f {} \;
rm -f compiler/src/main/java/etch/compiler/Version.java
rm -f binding-csharp/runtime/src/main/csharp/Properties/AssemblyInfo.cs
svn update .


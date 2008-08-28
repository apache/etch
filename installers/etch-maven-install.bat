@echo off

rem Copyright 2008 (c) Cisco Systems, Inc.

setlocal

rem Find the etch home directory

set ETCH_HOME=%~dp0..

mvn install:install-file "-Dfile=%ETCH_HOME%\lib\etch-java-runtime-1.0.1.jar" -DgroupId=etch.etch -DartifactId=etch-java-runtime -Dversion=1.0.1 -Dpackaging=jar

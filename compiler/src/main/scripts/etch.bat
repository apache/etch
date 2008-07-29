@echo off

rem Copyright 2008 (c) Cisco Systems, Inc.

setlocal

rem Find the etch home directory

set ETCH_HOME=%~dp0..

rem Make enough of a classpath to get to first base

set ETCH_CP=
for %%i in (%ETCH_HOME%\lib\etch-compiler-*.jar) do set ETCH_CP=%ETCH_CP%;%%i
rem for %%i in (%ETCH_HOME%\lib\etch-util-*.jar) do set ETCH_CP=%ETCH_CP%;%%i

rem add clover to classpath if defined and exists

if "%CLOVER_HOME%" == "" goto skip_clover
if not exist "%CLOVER_HOME%\lib\clover.jar" goto skip_clover
set ETCH_CP=%ETCH_CP%;%CLOVER_HOME%\lib\clover.jar
:skip_clover

java -cp "%ETCH_CP%" -Detch.home="%ETCH_HOME%" etch.compiler.EtchMain %*

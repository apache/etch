@echo off

rem Copyright 2008 Cisco Systems
rem This script is for the support of the various launch configurations
rem used with eclipse.

if "%ETCH_DEPENDENT_JARS%" neq "" goto test_etch_exist
echo Environment variable ETCH_DEPENDENT_JARS not set.
exit /b 1
goto :eof

:test_etch_exist
if exist "%~dp0..\bin\etch" goto run_etch
echo Cannot find etch classes.
exit /b 1
goto :eof

:run_etch
echo etch %*
java -cp "%~dp0..\bin;%ETCH_DEPENDENT_JARS%" etch.compiler.EtchMain %*

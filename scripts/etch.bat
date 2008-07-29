@echo off

REM Copyright 2007 (c) Cisco Systems, Inc.

setlocal
set ETCH_HOME=%~dp0..
set ETCH_JAR_PATH=%ETCH_HOME%\bin\etch.jar
set ETCH_ARGS=%*

if not exist "%ETCH_JAR_PATH%" goto ERROR
@java -jar "%ETCH_JAR_PATH%" %ETCH_ARGS%
@goto DONE

:ERROR
@echo Cannot find etch jar files
@exit /b 1

:DONE

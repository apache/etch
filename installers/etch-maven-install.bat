@echo off
:: Licensed to the Apache Software Foundation (ASF) under one   *
:: or more contributor license agreements.  See the NOTICE file *
:: distributed with this work for additional information        *
:: regarding copyright ownership.  The ASF licenses this file   *
:: to you under the Apache License, Version 2.0 (the            *
:: "License"); you may not use this file except in compliance   *
:: with the License.  You may obtain a copy of the License at   *
::                                                              *
::   http://www.apache.org/licenses/LICENSE-2.0                 *
::                                                              *
:: Unless required by applicable law or agreed to in writing,   *
:: software distributed under the License is distributed on an  *
:: "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY       *
:: KIND, either express or implied.  See the License for the    *
:: specific language governing permissions and limitations      *
:: under the License.                                           *
rem Copyright 2008 (c) Cisco Systems, Inc.

rem This .bat file installs the etch-java-runtime jar file to the local maven
rem repository. It assumes either M2_HOME or MAVEN_HOME is set in the
rem environment to match the maven installation, or "mvn" command is in the
rem search path.

setlocal

rem Define the etch home directory and artifact particulars

set ETCH_HOME=%~dp0..
set GROUP=etch.etch
set ARTIFACTS=etch-ant-plugin, etch-compiler, etch-csharp-compiler, etch-java-compiler, etch-java-runtime, etch-xml-compiler
set VERSION=@EtchVersion@

if "%M2_HOME%" == "" goto try_maven_home
set MVN=%M2_HOME%\bin\mvn.bat
if not exist "%MVN%" goto try_maven_home
goto run_mvn

:try_maven_home
if "%MAVEN_HOME%" == "" goto try_path
set MVN=%MAVEN_HOME%\bin\mvn.bat
if not exist "%MVN%" goto try_path
goto run_mvn

:try_path
set MVN=mvn.bat

:run_mvn
FOR %%A IN (%ARTIFACTS%) DO CALL "%MVN%" install:install-file "-Dfile=%ETCH_HOME%\lib\%%A-%VERSION%.jar" -DgroupId=%GROUP% -DartifactId=%%A -Dversion=%VERSION% -Dpackaging=jar
FOR %%A IN (%ARTIFACTS%) DO CALL "%MVN%" install:install-file "-Dfile=%ETCH_HOME%\lib\%%A-%VERSION%-src.zip" -DgroupId=%GROUP% -DartifactId=%%A -Dversion=%VERSION% -Dpackaging=jar -Dclassifier=sources

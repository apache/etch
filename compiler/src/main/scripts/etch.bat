@rem Licensed to the Apache Software Foundation (ASF) under one
@rem or more contributor license agreements. See the NOTICE file
@rem distributed with this work for additional information
@rem regarding copyright ownership. The ASF licenses this file
@rem to you under the Apache License, Version 2.0 (the
@rem "License"); you may not use this file except in compliance
@rem with the License. You may obtain a copy of the License at
@rem
@rem   http://www.apache.org/licenses/LICENSE-2.0
@rem
@rem Unless required by applicable law or agreed to in writing,
@rem software distributed under the License is distributed on an
@rem "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
@rem KIND, either express or implied. See the License for the
@rem specific language governing permissions and limitations
@rem under the License.
@echo off
setlocal

rem Find the etch home directory

set ETCH_HOME=%~dp0..

rem Make enough of a classpath to get to first base

set ETCH_CP=
for %%i in ("%ETCH_HOME%\lib\apache-etch-compiler-*.jar") do set ETCH_CP=%ETCH_CP%;%%i

rem add clover to classpath if defined and exists

if "%CLOVER_HOME%" == "" goto skip_clover
if not exist "%CLOVER_HOME%\lib\clover.jar" goto skip_clover
set ETCH_CP=%ETCH_CP%;%CLOVER_HOME%\lib\clover.jar
:skip_clover

java -cp "%ETCH_CP%" org.apache.etch.compiler.EtchMain %*

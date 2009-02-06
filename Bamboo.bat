@echo off
:: * Licensed to the Apache Software Foundation (ASF) under one   *
:: * or more contributor license agreements.  See the NOTICE file *
:: * distributed with this work for additional information        *
:: * regarding copyright ownership.  The ASF licenses this file   *
:: * to you under the Apache License, Version 2.0 (the            *
:: * "License"); you may not use this file except in compliance   *
:: * with the License.  You may obtain a copy of the License at   *
:: *                                                              *
:: *   http://www.apache.org/licenses/LICENSE-2.0                 *
:: *                                                              *
:: * Unless required by applicable law or agreed to in writing,   *
:: * software distributed under the License is distributed on an  *
:: * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY       *
:: * KIND, either express or implied.  See the License for the    *
:: * specific language governing permissions and limitations      *
:: * under the License.                                           *

:: This batch file is a wrapper so that we can set up our environment, and then
:: kick off a build in bamboo
::  (dixson) this really only useful in the Etch committers build-env

:: Run our env setup script
call %TOOLS_DIR%\Environment.bat -qnc -p ETCH
set ETCH_HOME=

:: Hack to get DOTNET defined. Environment.bat should do it.
set DOTNET_HOME=C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727

:: Call ant with the arguments that we pass in
call ant %*


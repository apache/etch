@rem  Licensed to the Apache Software Foundation (ASF) under one   *
@rem  or more contributor license agreements.  See the NOTICE file *
@rem  distributed with this work for additional information        *
@rem  regarding copyright ownership.  The ASF licenses this file   *
@rem  to you under the Apache License, Version 2.0 (the            *
@rem  "License"); you may not use this file except in compliance   *
@rem  with the License.  You may obtain a copy of the License at   *
@rem                                                               *
@rem    http://www.apache.org/licenses/LICENSE-2.0                 *
@rem                                                               *
@rem  Unless required by applicable law or agreed to in writing,   *
@rem  software distributed under the License is distributed on an  *
@rem  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY       *
@rem  KIND, either express or implied.  See the License for the    *
@rem  specific language governing permissions and limitations      *
@rem  under the License.                                           *
@echo off
:: This batch file is a wrapper so that we can set up our environment, and then
:: kick off a build in bamboo
::  (dixson) this really only useful in the Etch committers build-env


:: Run our env setup script
call %TOOLS_DIR%\Environment.bat -qnc -p ETCH
set ETCH_HOME=

:: Hack to get DOTNET defined. Environment.bat should do it.
set DOTNET_HOME=C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727

:: Call ant with the arguments that we pass in
cd ..
call ant %*


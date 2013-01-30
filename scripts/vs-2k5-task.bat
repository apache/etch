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

@rem -------------------------------------------------------------
@rem Build a standard Visual Studio .NET solution.
@rem
@rem Command line arguments:
@rem    %1 -> The filename of the solution to build
@rem    %2 -> The configuration to build
@rem -------------------------------------------------------------

@setlocal
@set LIB=
@set INCLUDE=
@set VsDotNet=devenv.com
@if "%VsDotNetDir%"==""  set VsDotNetDir=%VS80COMNTOOLS%
call "%VsDotNetDir%\vsvars32.bat" >nul

@if "%3"=="" goto withoutproject

@if /i "%2"=="clean" goto clean
@if /i "%2"=="Clean" goto clean

%VsDotNet% %1 /build %2 /project %3
@goto end

:clean
%VsDotNet% %1 /clean Debug /project %3
@if NOT %ERRORLEVEL%==0 @goto end

%VsDotNet% %1 /clean Release /project %3
@goto end

:withoutproject

@if /i "%2"=="clean" goto cleannoproject
@if /i "%2"=="Clean" goto cleannoproject

%VsDotNet% %1 /build %2
@goto end

:cleannoproject
%VsDotNet% %1 /clean Debug
@if NOT %ERRORLEVEL%==0 @goto end

%VsDotNet% %1 /clean Release
@goto end

:end
@echo.
@echo ERRORLEVEL = %ERRORLEVEL%
@exit %ERRORLEVEL%

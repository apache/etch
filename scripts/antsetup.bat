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

rem -- java builds --

set TOOLS_DIR=C:\wks\tools
set ANT_HOME=%TOOLS_DIR%\apache-ant\1.7.0
rem set JAVA_HOME=C:\Program Files\Java\jdk1.6.0_06
rem optional
rem set CLOVER_HOME=%TOOLS_DIR%\clover-ant\2.3.2
rem /optional
PATH %ANT_HOME%\bin;%JAVA_HOME%\bin;%PATH%

rem -- dotnet builds --

rem optional
set DOTNET_HOME=C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727
set NUNIT_HOME=%TOOLS_DIR%\NUnit\2.4.7
PATH %DOTNET_HOME%;%NUNIT_HOME%\bin;%PATH%
rem /optional

rem -- mono builds --

rem optional
rem set MONO_HOME=...
rem /optional

rem -- installer builds --

rem optional
set NSIS_HOME=%TOOLS_DIR%\nsis\2.23
rem /optional

rem -- standalone example builds --

rem set ETCH_HOME=...

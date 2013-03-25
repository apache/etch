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
@rem  under the License.      

@echo off
@echo Building Apache Etch %ETCH_BINDING%-binding. 

call antsetup.bat

set tests=-Dskip.tests=true
if "%2" == "tests" set tests=

IF "%1" == "release" GOTO :release
GOTO :debug

:debug
@echo Build type set to DEBUG
ant -f ..\build.xml debug-binding -Dbinding=%ETCH_BINDING% %ADDITIONAL_BINDING_PARAMS% %tests%

:release
@echo Build type set to RELEASE
ant -f ..\build.xml release-binding -Dbinding=%ETCH_BINDING% %ADDITIONAL_BINDING_PARAMS% %tests%
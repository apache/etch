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


IF "%ETCH_EXTERNAL_DEPENDS%" == "" set ETCH_EXTERNAL_DEPENDS=C:\etch-external-depends\
set ANT_HOME=%ETCH_EXTERNAL_DEPENDS%\apache-ant\1.7.0
set JAVA_HOME=c:\Program Files (x86)\Java\jdk1.6.0_18\
set NUNIT_HOME=%ETCH_EXTERNAL_DEPENDS%\NUnit\2.4.7

rem uncomment this line / unset this variable if you don't want to build csharp
set DOTNET_HOME=C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727

set PATH=%ANT_HOME%\bin;%JAVA_HOME%\bin;%DOTNET_HOME%;%NUNIT_HOME%\bin;%PATH%

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

@REM run this from the top of the cunit source directory, e.g.,
@REM g:\wks\tools\cunit\2.1\CUnit-2.1-0

@REM set INSTALL_PREFIX=C:\dev\projects\Etch\externals

REM check if environment is set
IF "%INSTALL_PREFIX%" == "" (
set ERROR_MSG="Please set env INSTALL_PREFIX to install location"
goto error
)

IF "%1%" == "" (
set ERROR_MSG="Please give command parameter"
call :help
goto error
)

IF "%1%" == "install" (
call :install
goto ende
)

set ERROR_MSG="Invalid command parameter"
call :help
goto error

:install
set INSTALL_PREFIX_CUNIT=%INSTALL_PREFIX%\cunit\2.1

set CYGWIN=nodosfilewarning
rm -rf "%INSTALL_PREFIX_CUNIT%\bin"
rm -rf "%INSTALL_PREFIX_CUNIT%\include"
rm -rf "%INSTALL_PREFIX_CUNIT%\lib"
rm -rf "%INSTALL_PREFIX_CUNIT%\res"

mkdir "%INSTALL_PREFIX_CUNIT%\bin"
mkdir "%INSTALL_PREFIX_CUNIT%\include"
mkdir "%INSTALL_PREFIX_CUNIT%\lib"
mkdir "%INSTALL_PREFIX_CUNIT%\res"

REM copy artifacts
xcopy /Y "CUnit\Headers\*.h" "%INSTALL_PREFIX_CUNIT%\include\"
xcopy /Y "VC8\Release - Static Lib\libcunit.lib" "%INSTALL_PREFIX_CUNIT%\lib\"
xcopy /Y "VC8\Release - Static Lib\vc80_libcunit.pdb" "%INSTALL_PREFIX_CUNIT%\lib\"
xcopy /Y "Share\*.dtd" "%INSTALL_PREFIX_CUNIT%\res\"
xcopy /Y "Share\*.xsl" "%INSTALL_PREFIX_CUNIT%\res\"
goto :EOF

:help:
@echo ==================================================
@echo Help
@echo ==================================================
@echo Following commands are available.
@echo cunit-install.bat install // Install CUNIT to given location
goto :EOF

:error
@echo ==================================================
@echo Error: %ERROR_MSG%
@echo ==================================================
goto ende

:ende
goto :EOF	




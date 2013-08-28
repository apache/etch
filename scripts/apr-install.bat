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

@REM set INSTALL_PREFIX=C:\dev\projects\Etch\externals
@REM set VC_HOME=C:\Program Files ^(x86^)\Microsoft Visual Studio 8\VC
@REM use this instead on 32-bit windows:
@REM set VC_HOME=C:\Program Files\Microsoft Visual Studio 8\VC

REM check if environment is set
IF "%INSTALL_PREFIX%" == "" (
set ERROR_MSG="Please set env INSTALL_PREFIX to install location"
goto error
)

IF "%VC_HOME%" == "" (
set ERROR_MSG="Please set env VC_HOME to VC install location"
goto error
)

IF "%1%" == "" (
set ERROR_MSG="Please give command parameter"
call :help
goto error
)

IF "%1%" == "download" (
call :download
goto ende
)
IF "%1%" == "convert" (
call :convert
goto ende
)
IF "%1%" == "build" (
call :build
goto ende
)
IF "%1%" == "install" (
call :install
goto ende
)

set ERROR_MSG="Invalid command parameter"
call :help
goto error

:download:
@echo ==================================================
@echo Downloading APR ...
@echo ==================================================
rm -rf apr
mkdir apr
cd apr

REM apr
rem svn export https://svn.apache.org/repos/asf/apr/apr/tags/1.4.6/ apr
wget http://apache.imsam.info//apr/apr-1.4.8-win32-src.zip
unzip apr-1.4.8-win32-src.zip
mv apr-1.4.8 apr
rm -rf apr-1.4.8-win32-src.zip

REM apr-util
rem svn export https://svn.apache.org/repos/asf/apr/apr-util/tags/1.4.1/ apr-util
wget http://apache.imsam.info//apr/apr-util-1.5.2-win32-src.zip
unzip apr-util-1.5.2-win32-src.zip
mv apr-util-1.5.2 apr-util
rm -rf apr-util-1.5.2-win32-src.zip

REM apr-iconv
rem svn export https://svn.apache.org/repos/asf/apr/apr-iconv/tags/1.2.1/ apr-iconv
wget http://mirror.synyx.de/apache//apr/apr-iconv-1.2.1-win32-src-r2.zip
unzip apr-iconv-1.2.1-win32-src-r2.zip
mv apr-iconv-1.2.1 apr-iconv
rm -rf apr-iconv-1.2.1-win32-src-r2.zip

cd ..
@echo ==================================================
@echo Downloading APR done.
@echo ==================================================
goto :EOF

:build-init
@echo ==================================================
@echo Init VS environment...
@echo ==================================================

call "%VC_HOME%\bin\vcvars32.bat"

@echo ==================================================
@echo Init VS environment done.
@echo ==================================================
goto :EOF

:convert
call :build-init

@echo ==================================================
@echo Converting APR to new VS version...
@echo This is a manual step and have to be done only oncs. After VS is
@echo opened, please accept conversion for all projects and save the
@echo solution. After that , please close VS.
@echo ==================================================
PAUSE
cd apr\apr-util
devenv aprutil.dsw
cd ..\..

@echo ==================================================
@echo Converting APR to new VS version done.
@echo Building APR done.
@echo ==================================================

goto :EOF

:build
call :build-init

@echo ==================================================
@echo Building APR...
@echo ==================================================

cd apr\apr-util
devenv aprutil.sln /build Release /project apr
devenv aprutil.sln /build Release /project apriconv
devenv aprutil.sln /build Release /project libapr
devenv aprutil.sln /build Release /project libapriconv
devenv aprutil.sln /build Release /project libapriconv_ccs_modules
devenv aprutil.sln /build Release /project libapriconv_ces_modules
cd ..\..

@echo ==================================================
@echo Building APR done.
@echo ==================================================
goto :EOF

:install
set INSTALL_PREFIX_APR=%INSTALL_PREFIX%\apr\1.4.8
mkdir "%INSTALL_PREFIX_APR%\bin"
mkdir "%INSTALL_PREFIX_APR%\iconv"
mkdir "%INSTALL_PREFIX_APR%\include\apr-1"
mkdir "%INSTALL_PREFIX_APR%\lib"
REM copy artifacts
xcopy /Y apr\apr\include\*.h "%INSTALL_PREFIX_APR%\include\apr-1\"
xcopy /Y apr\apr\Release\libapr-1.dll "%INSTALL_PREFIX_APR%\bin\"
xcopy /Y apr\apr\Release\libapr-1.pdb "%INSTALL_PREFIX_APR%\bin\"
xcopy /Y apr\apr\Release\libapr-1.lib "%INSTALL_PREFIX_APR%\lib\"
xcopy /Y apr\apr-iconv\include\*.h "%INSTALL_PREFIX_APR%\include\apr-1\" 
xcopy /Y apr\apr-iconv\Release\libapriconv-1.lib "%INSTALL_PREFIX_APR%\lib\"
xcopy /Y apr\apr-iconv\Release\libapriconv-1.dll "%INSTALL_PREFIX_APR%\bin\"
xcopy /Y apr\apr-iconv\Release\libapriconv-1.pdb "%INSTALL_PREFIX_APR%\bin\"
xcopy /Y apr\apr-iconv\Release\iconv\*.so "%INSTALL_PREFIX_APR%\iconv\"
xcopy /Y apr\apr-iconv\Release\iconv\*.pdb "%INSTALL_PREFIX_APR%\iconv\"
goto :EOF

:help:
@echo ==================================================
@echo Help
@echo ==================================================
@echo Following commands are available.
@echo apr-install.bat download // Dowload all APR sources to ./apr/
@echo apr-install.bat convert // Start VS to convert APR projects
@echo apr-install.bat build // Build APR
@echo apr-install.bat install // Install APR to given location
goto :EOF

:error
@echo ==================================================
@echo Error: %ERROR_MSG%
@echo ==================================================
goto ende

:ende
goto :EOF	




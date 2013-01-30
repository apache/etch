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
setlocal

if "%1" == "" goto usage

call "%VS71COMNTOOLS%\vsvars32.bat" >nul
set DeployDir=..\installer

if not exist %DeployDir%	mkdir %DeployDir%

if exist postbuild.txt del postbuild.txt /F /Q

REM Clear shadow copies of dlls
gacutil /cdl                                                            >> postbuild.txt

@echo ****** Removing assemblies from GAC ******                        >> postbuild.txt
gacutil /uf EtchAddIn                                                   >> postbuild.txt

@echo.                                                                  >> postbuild.txt

@echo ****** Installing common assemblies ******                        >> postbuild.txt
xcopy ..\etch-addin\bin\EtchAddIn.dll                                   %DeployDir%     /Y     >> postbuild.txt
xcopy ..\etch-addin\Etch.AddIn                                          %DeployDir%     /Y     >> postbuild.txt

@echo.                                                                  >> postbuild.txt

gacutil /i %DeployDir%\EtchAddIn.dll                                    >> postbuild.txt

echo.                                                                   >> postbuild.txt

goto done

goto done

:usage
echo Usage: PostBuild.bat [CONFIGURATION]
echo Configuration is any valid VS.NET configuration (Debug, Release, etc).

goto done

:done

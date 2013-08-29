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

rem This .bat file installs the etch-java-runtime jar file to the local maven
rem repository. It assumes either M2_HOME or MAVEN_HOME is set in the
rem environment to match the maven installation, or "mvn" command is in the
rem search path.

setlocal

rem Define the etch home directory and artifact particulars

set ETCH_HOME=%~dp0..
set GROUP=org.apache.etch
set ARTIFACTS_COMPILER=apache-etch-ant-plugin, apache-etch-compiler, apache-etch-csharp-compiler, apache-etch-java-compiler, apache-etch-c-compiler, apache-etch-xml-compiler
set ARTIFACTS_RUNTIME_BINDING-JAVA=apache-etch-java-runtime
set ARTIFACTS_RUNTIME_BINDING-CSHARP=apache-etch-csharp-runtime
set ARTIFACTS_RUNTIME_BINDING-C=apache-etch-c-runtime
set VERSION=@EtchVersion@

if "%M2_HOME%" == "" goto try_maven_home
set MVN=%M2_HOME%\bin\mvn.bat
if not exist "%MVN%" goto try_maven_home
goto run_mvn

:try_maven_home
if "%MAVEN_HOME%" == "" goto try_path
set MVN=%MAVEN_HOME%\bin\mvn.bat
if not exist "%MVN%" goto try_path
goto run_mvn

:try_path
set MVN=mvn.bat

:run_mvn
rem compiler bin and source
FOR %%A IN (%ARTIFACTS_COMPILER%) DO CALL "%MVN%" install:install-file "-Dfile=%ETCH_HOME%\lib\%%A-%VERSION%.jar" -DgroupId=%GROUP% -DartifactId=%%A -Dversion=%VERSION% -Dpackaging=jar
FOR %%A IN (%ARTIFACTS_COMPILER%) DO CALL "%MVN%" install:install-file "-Dfile=%ETCH_HOME%\lib\%%A-%VERSION%-src.zip" -DgroupId=%GROUP% -DartifactId=%%A -Dversion=%VERSION% -Dpackaging=zip -Dclassifier=src
rem runtime java bin and source
FOR %%A IN (%ARTIFACTS_RUNTIME_BINDING-JAVA%) DO CALL "%MVN%" install:install-file "-Dfile=%ETCH_HOME%\binding-java\lib\%%A-%VERSION%.jar" -DgroupId=%GROUP% -DartifactId=%%A -Dversion=%VERSION% -Dpackaging=jar
FOR %%A IN (%ARTIFACTS_RUNTIME_BINDING-JAVA%) DO CALL "%MVN%" install:install-file "-Dfile=%ETCH_HOME%\binding-java\lib\%%A-%VERSION%-src.zip" -DgroupId=%GROUP% -DartifactId=%%A -Dversion=%VERSION% -Dpackaging=zip -Dclassifier=src

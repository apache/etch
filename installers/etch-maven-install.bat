@echo off

rem Copyright 2008 (c) Cisco Systems, Inc.

rem This .bat file installs the etch-java-runtime jar file to the local maven
rem repository. It assumes either M2_HOME or MAVEN_HOME is set in the
rem environment to match the maven installation, or "mvn" command is in the
rem search path.

setlocal

rem Define the etch home directory and artifact particulars

set ETCH_HOME=%~dp0..
set GROUP=etch.etch
set ARTIFACT=etch-java-runtime
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
"%MVN%" install:install-file "-Dfile=%ETCH_HOME%\lib\etch-java-runtime-%VERSION%.jar" -DgroupId=%GROUP% -DartifactId=%ARTIFACT% -Dversion=%VERSION% -Dpackaging=jar

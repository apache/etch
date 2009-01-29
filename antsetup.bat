# example setup script for ant build environment.
# optional stuff may be omitted by prefixing every
# line between the brackets with rem.

# look for additional information in build.dependencies.

rem -- java builds --

set TOOLS_DIR=C:\wks\tools
set ANT_HOME=%TOOLS_DIR%\apache-ant\1.7.0
ren set JAVA_HOME=C:\Program Files\Java\jdk1.6.0_06
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

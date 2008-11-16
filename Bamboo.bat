@echo off
:: This batch file is a wrapper so that we can set up our environment, and then
:: kick off a build in bamboo

:: Run our env setup script
call %TOOLS_DIR%\Environment.bat -qnc -p ETCH
set ETCH_HOME=

:: Hack to get DOTNET defined. Environment.bat should do it.
set DOTNET_HOME=C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727

:: Call ant with the arguments that we pass in
call ant %*


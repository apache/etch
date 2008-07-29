@echo off
:: This batch file is a wrapper so that we can set up our environment, and then
:: kick off a build in bamboo

:: Run our env setup script
call %TOOLS_DIR%\Environment.bat -qnc -p ETCH

:: Call ant with the arguments that we pass in
call ant %*


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

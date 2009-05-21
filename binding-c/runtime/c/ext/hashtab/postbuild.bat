@echo OFF
echo postBuild.bat start
echo postBuild.bat copying jenkhash.lib debug symbols to lib directory ...

if exist .\debug\vc80.pdb goto debug
if exist .\release\vc80.pdb goto release
echo no debug symbol file found - assuming C7 embedded symbols
goto done  

:release
xcopy .\release\vc80.pdb ..\lib /y /f /i
del .\release\vc80.pdb 
goto done

:debug
xcopy .\debug\vc80.pdb ..\lib /y /f /i
del .\debug\vc80.pdb

:done
echo postBuild.bat exit


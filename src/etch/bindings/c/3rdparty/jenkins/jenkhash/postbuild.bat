@echo OFF
echo postBuild.BAT start
echo postBuild.BAT copying jenkhash.lib debug symbols to LIB directory ...

if exist .\debug\vc80.pdb goto debug
if exist .\release\vc80.pdb goto release
goto done  

:release
xcopy .\release\vc80.pdb ..\lib /y /f /i
del .\release\vc80.pdb 
goto done

:debug
xcopy .\debug\vc80.pdb ..\lib /y /f /i
del .\debug\vc80.pdb

:done
echo postBuild.BAT exit


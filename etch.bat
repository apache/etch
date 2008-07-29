@if not exist %~f0\..\etch.jar goto ERROR
@java -jar %~f0\..\etch.jar %*
@goto DONE

:ERROR
@echo Cannot find etch jar files
@exit /b 1

:DONE

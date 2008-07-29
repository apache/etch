pushd bin
jar -xf ..\3rdparty\velocity-1.5\velocity-dep-1.5.jar
jar -cmf ..\compilerMain.txt etch.jar etch org
popd
copy etch.bat bin

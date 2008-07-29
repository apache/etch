pushd build
jar -xf ..\libs\velocity-dep-1.5.jar
jar -cmf ..\compilerMain.txt etch.jar etch metreos org
popd
copy etch.bat build

This example demonstrate basic @Signal usage

To generate sources, build and run tests execute:

cd $ETCH_HOME
.\scripts\antSetup.bat
cd examples
cd signals
ant Debug

This will generate sources for all csharp, java and c. It will also build the csharp and java sources. See below
regarding the build of the C binding sources.

JAVA INSTRUCTIONS:
******************
To run java Signals example for java, run these commands:

> cd target\bin
> start java -cp signals.jar org.apache.etch.examples.signals.MainSignalsListener
> start java -cp signals.jar org.apache.etch.examples.signals.MainSignalsClient

CSHARP INSTRUCTIONS:
********************
To run csharp example:

> cd target\bin
> start SignalsListener.exe
> start SignalsClient.exe

C INSTRUCTIONS:
***************
To run the c example:
> build using cmake (see below)
> on windows using visual studio: build the generated solution and execute server and client targets
> on linux: use make to build using generated makefiles, execute signals-client 
            and signals-server executables.

To build the C sources, use cmake. Start cmake-gui, set source to example/signals folder and "build" to a subfolder (e.g. "ctarget").
You have to set the following variable in CMake:
- ETCH_HOME must point to your Etch directory (this is were you installed Etch, or target/Installers/dist if building from trunk)

on the shell, you could do this using
cd examples/signals/target
cmake .. -DETCH_HOME:string=<YOUR ETCH INSTALLATION DIR>
make (or open visual studio on win32...)

You also have to set the following environment variable:
- APR_ICONV_PATH to <YOUR ETCH INSTALLATION DIR>/binding-c/extern/apr/iconv/

To run c example:

> cd target\bin
> start signals-server.exe
> start signals-client.exe




You can mix and match the various clients and listeners.

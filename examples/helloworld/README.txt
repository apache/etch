Helloworld is the classic example and shows basic usage of etch and the interoperability of the bindings.

To generate sources, build and run tests execute:

cd $ETCH_HOME
.\scripts\antSetup.bat
cd examples
cd helloworld
ant Debug

This will generate sources for all csharp, java and c. It will also build the csharp and java sources. See below
regarding the build of the C binding sources.

JAVA INSTRUCTIONS:
******************
To run java HelloWorld example for java, run these commands:

> cd target\bin
> start java -cp helloworld.jar org.apache.etch.examples.helloworld.MainHelloWorldListener
> start java -cp helloworld.jar org.apache.etch.examples.helloworld.MainHelloWorldClient

CSHARP INSTRUCTIONS:
********************
To run csharp example:

> cd target\bin
> start HelloWorldListener.exe
> start HelloWorldClient.exe

C INSTRUCTIONS:
***************
To run the c example:
> build using cmake (see below)
> on windows using visual studio: build the generated solution and execute server and client targets
> on linux: use make to build using generated makefiles, execute etch-c-helloworld-client 
            and etch-c-helloworld-server executables.

To build the C sources, use cmake. Start cmake-gui, set source to example/helloworld folder and "build" to a subfolder (e.g. "ctarget").
You have to set the following variable in CMake:
- ETCH_HOME must point to your Etch directory (this is were you installed Etch, or target/Installers/dist if building from trunk)

on the shell, you could do this using
cd examples/helloworld/target
cmake .. -DETCH_HOME:string=<YOUR ETCH INSTALLATION DIR>
make (or open visual studio on win32...)

You also have to set the following environment variable:
- APR_ICONV_PATH to <YOUR ETCH INSTALLATION DIR>/binding-c/extern/apr/iconv/

To run c example:

> cd target\bin
> start helloworld-server.exe
> start helloworld-client.exe




You can mix and match the various clients and listeners.

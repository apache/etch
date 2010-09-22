Helloworld is the classic example and shows basic usage of etch and the interoperability of the bindings.

To generate sources, build and run tests execute:

cd $ETCH_HOME
.\scripts\antSetup.bat
cd examples
cd helloworld
ant Debug

This will generate sources for all csharp, java and c. It will also build the csharp and java sources.
To build the C sources, use cmake. Start cmake-gui, set source to this folder, build to a subfolder (e.g. "ctarget").
You have to set to string variables in CMake:
- ETCH_EXTERNAL_DEPENDS must point to your dependency directory (see binding-c/runtime/c/README.txt for details)
- ETCH_HOME must point to your etch installation directory (if you are doing this on trunk, then use
  your path to trunk\target\Installers\dist\)

To run java HelloWorld example for java, run these commands:

> cd target\bin
> start java -cp helloworld.jar org.apache.etch.examples.helloworld.MainHelloWorldListener
> start java -cp helloworld.jar org.apache.etch.examples.helloworld.MainHelloWorldClient

To run csharp example:

> cd target\bin
> start HelloWorldListener.exe
> start HelloWorldClient.exe

To run the c example:
> build using cmake (see above)
> on windows using visual studio: build the generated solution and execute server and client targets
> on linux: use make to build using generated makefiles, execute etch-c-helloworld-client 
            and etch-c-helloworld-server executables.

You can mix and match the various clients and listeners.

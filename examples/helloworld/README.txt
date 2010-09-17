Helloworld is the classic example and shows basic usage of etch and the interoperability of the bindings.

To generate sources, build and run tests execute:

cd $ETCH_HOME
.\scripts\antSetup.bat
cd examples
cd helloworld
ant Debug

This will generate sources for all csharp, java and c. It will also build the csharp and java sources.
To build the C sources, please use the instructions and cmake files provided in the src/main/c folder.

To run java HelloWorld example for java, run these commands:

> cd target\bin
> start java -cp helloworld.jar org.apache.etch.examples.helloworld.MainHelloWorldListener
> start java -cp helloworld.jar org.apache.etch.examples.helloworld.MainHelloWorldClient

To run csharp example:

> cd target\bin
> start HelloWorldListener.exe
> start HelloWorldClient.exe

You can mix and match the various clients and listeners.


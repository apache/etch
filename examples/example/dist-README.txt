To get started with the example you will need jdk 1.5.0_x,
where x is pretty recent (>= 15), or any release of jdk 1.6.
You will also need ant 1.8.2. 
For building the csharp examples you need the Microsoft .NET Framework v4.0 or higher. 
Furthermore you need to add the .NET Framework path to your PATH environment variable:
set PATH=C:\WINDOWS\Microsoft.NET\Framework\v4.0.30319;%PATH%

Call 
> ant
to generate sources, build and run tests.

This will run the default target which will build both java
and csharp programs. You can also use build-java or build-csharp
targets to build only java or only csharp.

To run java example example, open a cmd window and run these
commands:

> cd target\bin
> start java -cp example.jar org.apache.etch.examples.example.MainExampleListener
> start java -cp example.jar org.apache.etch.examples.example.MainExampleClient

To run csharp example:

> cd target\bin
> start exampleListener.exe
> start exampleClient.exe

Now modify the example to implement the service however you like.

You can mix and match the various clients and listeners.

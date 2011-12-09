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

To run java performance example, open a cmd window and run these
commands:

> cd target\bin
> start java -cp perf.jar org.apache.etch.examples.perf.MainPerfListener
> start java -cp perf.jar org.apache.etch.examples.perf.MainPerfClient

To run csharp example:

> cd target\bin
> start perfListener.exe
> start perfClient.exe

You can mix and match the various clients and listeners.

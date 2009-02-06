To get started with the example you will need jdk 1.5.0_x,
where x is pretty recent (>= 15), or any release of jdk 1.6.
You will also need ant 1.7.0, with ant .net plugin if you
want to build csharp. You will also need microsoft .net
sdk 2.0 for csharp builds.

To build:

> ant

This will run the default target which will build both java
and csharp programs. You can also use build-java or build-csharp
targets to build only java or only csharp.

To run java example example, open a cmd window and run these
commands:

> cd target\bin
> start java -cp example.jar etch.examples.example.MainExmapleListener
> start java -cp example.jar etch.examples.example.MainExampleClient

To run csharp example:

> cd target\bin
> start exampleListener.exe
> start exampleClient.exe

Now modify the example to implement the service however you like.

You can mix and match the various clients and listeners.

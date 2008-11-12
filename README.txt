Etch
----

This is the source tree for Etch. It is organized as follows:

   build.xml         - top-level ant build script
   build.dependecies - locations for jars this compile depends on
   etch.properties   - static build properties 
   
   compiler/      - core compiler
   build-support/ - common ant scripts shared by all modules
   plugins/       - extensions that embed the compiler, e.g. ant, maven, etc.
   scripts/       - common scripts for eclipse
   util/          - shared java classes
   tests/         - functional tests
   examples/      - Etch application examples
      chat/       - simple IM application
      distmap/    - example implementation of a distributed map in etch
      perf/       - etch client/server performance test
      example/    - minimal example
   installers/    - project for NSIS installer and tar.gz, .zip archives
       
   binding-xml    - xml binding
   binding-java   - java-language binding
   binding-csharp - C#-language binding
   

Etch bindings
-------------

Each binding is factored into its own structure into the build tree. The intent
is to provide an easy pattern for potential binding authors to emulate.
Essentially each binding has two components:

   - compiler/
   - runtime/
   
The 'compiler' is always implemented in Java and is coded to implement a
backend interface for the target of choice.

The 'runtime' is coded in the target language. The most useful language
bindings implement identical functionality as the Java and C# bindings.
Non-languages bindings (like binding-xml) may not have a runtime component
at all.

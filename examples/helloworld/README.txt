The Helloworld example is a very very simple example to illustrate the interoperability 
of the c and the java binding. 

Of course you can also compile every ETCH IDL from the official Etch examples using the 
ETCH C IDL Compiler.

This example comes already pre-generated for you, so you should have minimal effort getting it 
running. You can re-generate the code using the generate-c-and-java.bat in \helloworld\etch\

To run the example, run the java server (an eclipse  project is located in \helloworld\etch)
and run the c client (a VS solution is provided in \helloworld\c). Feel free to play with IDL 
and sources to try out other Etch features. Currently the Helloworld c example implements
only the client part. 

****************************************************
ATTENTION: The C Binding needs libapr-1.dll and libapriconv-1.dll either in your PATH or
in the working directory of the client executable!
****************************************************

Please note that the C Compiler does not overwrite impl-files (they might contain your Implementation).
A side-effect is that you have to delete all impl headers by hand when adding e.g. a new function to the IDL.
This will be changed in a later release.


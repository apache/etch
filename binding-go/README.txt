Google GO Binding for Apache Etch
*********************************

This is an alpha implementation of a Go binding (runtime + compiler) for Apache Etch.
You can use most basic features of Etch:
- non-blocking oneway calls
- blocking twoway calls
- server directed calls
- client directed calls
- user defined struct types
- arrays 

-----------------------------------------------------------------------------------
Building the Binding
-----------------------------------------------------------------------------------

There are two parts: 
- RUNTIME:    libraries to execute Go Source using Etch
- COMPILER:   code generator for e.g. stubs and proxies from an Etch IDL

COMPILER BUILD:
***************

If you build Etch using ant, you can enable the Go Binding COMPILER build by setting 
   include.experimental=true
in build.dependencies in the top level directory.
See BUILD.txt on the top-level of the Etch SVN trunk for more details on the ant-based build 
of Apache Etch.

RUNTIME BUILD:
**************

To build the Go binding RUNTIME for Linux (X86) just do:

> cd binding-go/runtime/
> make

This will build all required files to binding-go/runtime/src/main/go/etch/etch.8 and 
will execute the go binding testsuite.

-----------------------------------------------------------------------------------
Generating Go Source from Etch IDLs
-----------------------------------------------------------------------------------

If you have built the compiler, you can generate Go Code from an Etch IDL with: 

> etch -b go -d . -w INTF,MAIN,IMPL,BOTH SomeFile.etch

This will generate all neccessary files and a Makefile for Linux (x86)
Build your generated source code with

> make ETCHLIBPATH=/some/path/binding-go/runtime/etch

ETCHLIBPATH must contain the Go Etch runtime (etch.8)

-----------------------------------------------------------------------------------
Example
-----------------------------------------------------------------------------------

The Binding comes with an example IDL based on Etch's HelloWorld Example. It illustrates 
bidirectional calls (client -> server and server->client), oneway calls, twoway calls, 
user defined types, arrays of primitives and arrays of structs.

Check the IDL with

> cd binding-go/runtime/src/test/go/example/
> less HelloWorld.etch

For your convenience the generated sources are already contained in the "generated" folder 
and some method implementations have been made already (check Impl*.go and Main*.go)

If you have already built the Etch Go RUNTIME (see above), then proceed by 
building the example source (on Linux x86) by executing:

> cd binding-go/runtime/src/test/go/example/generated
> make ETCHLIBPATH=../../../../main/go/etch

Execute the example server:

> cd binding-go/runtime/src/test/go/example/generated
> ./HelloWorldServer

Execute the example client:

> cd binding-go/runtime/src/test/go/example/generated
> ./HelloWorldClient

-----------------------------------------------------------------------------------
Unsupported Features
-----------------------------------------------------------------------------------

This binding is alpha and supports not all features of Etch, some things are
known not to work or unstable.

If you like the binding and need one of those features or want to get involved otherwise, 
feel free to post to etch-dev@incubator.apache.org, send patches or create Jira issues 
at https://issues.apache.org/jira/browse/ETCH

We are very happy about your help!

Currently unsupported features:
- exceptions
- mixins
- constants & enums
- multi-dimensional arrays
- builtin types: set, list, map, datetime
- object parameter type (experimental)
- security (Auth)
- filters 



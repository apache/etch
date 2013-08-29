Welcome to the 1.3.0 release of Apache Etch.

= What is Etch? =

Etch is a cross-platform, language- and transport-independent framework for
building and consuming network services. The Etch toolset includes a network
service description language, a compiler, and binding libraries for a variety
of programming languages. Etch is also transport-independent, allowing for a
variety of different transports to used based on need and circumstance. The
goal of Etch is to make it simple to define small, focused services that can
be easily accessed, combined, and deployed in a similar manner. With Etch,
service development and consumption becomes no more difficult than library
development and consumption.

= Online Help =

The jumping off point for Etch information and help is here:

  http://etch.apache.org

= Binary Distribution =

The top-level structure of the install image is:

  ChangeLog.txt
  DISCLAIMER.txt
  LICENSE.txt
  LICENSE_THIRD_PARTY.txt
  NOTICE.txt
  NOTICE_THIRD_PARTY.txt
  README.txt
  RELEASE_NOTES.txt
  bin/
  examples/
  lib/
  binding-c/
  binding-csharp/
  binding-java/
  uninst.exe (for windows use only)

Please take a moment to review the RELEASE_NOTES.txt, ChangeLog.txt and
LICENSE.txt files.

The Windows installer (apache-etch-1.3.0-setup.exe) has created a 
user environment variable (ETCH_HOME) which points to the Etch installation
directory. If you installed Etch using either

  apache-etch-1.3.0-windows-x86-bin.zip

or

  apache-etch-1.3.0-linux-x86-bin.tgz

you will want to create this environment variable yourself:

windows:
  set ETCH_HOME=C:\Program Files (x86)\Apache Software Foundation\apache-etch-1.3.0

*nix:
  export ETCH_HOME=/path/to/apache-etch-1.3.0

The bin directory has also been put on your path by the Windows installer. If
you are using the archives, you will need to do this yourself:

windows:
  PATH %PATH%;%ETCH_HOME%\bin

*nix:
  export PATH="$PATH:$ETCH_HOME/bin"

The bin/ directory contains a Windows bat script and a unix shell script.

You will also need to have a java runtime installed (later versions of sun jre
or jdk 1.5 or any version of 1.6 or later). A JAVA_HOME environment variable
should point to the installation directory of the java runtime. If you are going
to compile java source you will need the jdk.

After the installer has been run and the environment setup, at a windows
command line or shell prompt, you should be able to run the etch compiler and
see some basic output:

windows:
  C:\>etch -v
  Etch version is Apache Etch 1.3.0 (some build info)

unix:
  bash-3.2$ etch -v
  Etch version is Apache Etch 1.3.0 (some build info)

The lib directory contains the various jar files for the compiler and the Apache Velocity dependency jar:

  apache-etch-java-compiler-1.3.0.jar
  apache-etch-java-compiler-1.3.0-src.jar
  ...

The top level folders binding-c, binding-java and binding-csharp contain the language specific 
runtime libraries:

= Binding for Java =

binding-java is organized as follows:

  apache-etch-java-runtime-1.3.0-src.zip
  lib/
    apache-etch-java-runtime-1.3.0.jar

The generated java source from the etch compiler needs apache-etch-java-runtime-1.3.0.jar.
There is source code to go with it in apache-etch-java-runtime-1.3.0-src.zip.

= Binding for C# =

binding-csharp is organized as follows:

  apache-etch-csharp-runtime-1.3.0-src.zip  
  lib/
    ApacheEtchCsharp.dll
    ApacheEtchCsharp.pdb

The generated c# source from the etch compiler needs ApacheEtchCsharp.dll
The C# (.NET 2.0 and later) ApacheEtchCsharp.dll should be added to any C# projects.
There is source code to go with it in apache-etch-csharp-runtime-1.3.0-src.zip.

= Binding for C =

binding-c is organized as follows:

  bin/
    apr-1-config		(on linux)
    apriconv			(on linux)
  include/
    [etch-*].h			(on win32)
    apr-1			(on linux)
  lib/
    etch.lib			(on win32)
    etch.pdb			(on win32)
    libetch.a			(on linux)
  extern/
    apr/
      bin/
        libapr-1.dll		(on win32)
        libapr-1.pdb		(on win32)
        libapriconv-1.dll	(on win32)
        libapriconv-1.pdb	(on win32)
      iconv/
        [iconv*]-stuff
      include/
        [apr-*].h
      lib/
        libapr-1.lib		(on win32)
	libapriconv-1.lib	(on win32)
	libapr-1.so		(on linux)
	libapriconv-1.so	(on linux)
  apache-etch-c-runtime-1.3.0-src.zip

Generated C source depends on the dlls/so/a/h files mentioned above. The best choice to 
build generated code using the c binding is cmake. You can use the examples/helloworld 
example as a reference. The C binding has external dependencies to the Apache Portable 
Runtime (APR). We ship dll/so/h for the apr in the extern folder. There is source code 
to go with it in apache-etch-c-runtime-1.3.0-src.zip

= Ant Plugin =

There is an ant plugin which you can use with ant 1.7.0 or later to invoke the
compiler as a task. It is documented on the wiki referenced above. You can also see examples 
of its usage in the build.xml files in the examples folder

= Maven =

If you are using maven, then you might want to install the Etch artifacts into
your local maven repository. There is a Windows bat script to do this in the
maven directory:

  etch-maven-install.bat

You need to have maven on your path when you run this script. The file
etch-java-runtime-1.3.0.jar will be installed into your local repository with
group "etch.etch", artifact id "etch-java-runtime", and version "1.3.0". You
may then reference Etch from your maven projects.

= Python and XML Bindings =

The source distribution includes code to support XML and python bindings, but
these bindings are not yet complete, and there is no build products associated
with those. They are mainly there to 1) capture the code contribution from
cisco systems, and 2) inspire any fence sitters, ah, helpers to jump in and
contribute.

= Examples =

Examples have been provided in the examples/ subdirectory. Start with the HelloWorld
example, it contains code for all three language bindings. There is a readme
file in each example's directory which explains how to build and run it.

= Tests =

Unit tests can be had by checking out the source code from the subversion
repository and performing a complete build:

  http://svn.apache.org/repos/asf/etch/releases/release-1.3.0

= Help =

For help, see the wiki or send email to user@etch.apache.org. See
RELEASE_NOTES.txt for specific information for this release.

Welcome to the 1.1.0-incubating release of Apache Etch.

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

  http://cwiki.apache.org/ETCH

= Binary Distribution =

The top-level structure of the install image is:

  ChangeLog.txt
  DISCLAIMER.txt
  LICENSE.txt
  NOTICE.txt
  README.txt
  RELEASE_NOTES.txt
  bin/
  examples/
  lib/
  maven/
  uninst.exe (for windows use only)

Please take a moment to review the RELEASE_NOTES.txt, ChangeLog.txt and
LICENSE.txt files.

The Windows installer (apache-etch-1.1.0-incubating-setup.exe) has created a 
user environment variable (ETCH_HOME) which points to the Etch installation
directory. If you installed Etch using either

  apache-etch-1.1.0-incubating-bin.tar.gz

or

  apache-etch-1.1.0-incubating-bin.zip

you will want to create this environment variable yourself:

windows:
  set ETCH_HOME=C:\Program Files\Apache Software Foundation\apache-etch-1.1.0

*nix:
  export ETCH_HOME=/path/to/apache-etch-1.1.0

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
  Etch version is Apache Etch 1.1.0-incubating (some build info)

unix:
  bash-3.2$ etch -v
  Etch version is Apache Etch 1.1.0-incubating (some build info)

The lib directory contains the various jar files, zipped sources, and a C# dll
with associated symbols and source:

  ApacheEtchCsharpIncubating.dll
  ApacheEtchCsharpIncubating.pdb
  ApacheEtchCsharpIncubatingSrc.zip
  ...
  apache-etch-java-runtime-1.1.0-incubating-src.zip
  apache-etch-java-runtime-1.1.0-incubating.jar
  ...

The main items of interest, shown above, are the language binding runtimes,
which you need to link with your projects.

For java you need to put the etch java runtime jar on the classpath of your
projects:

  apache-etch-java-runtime-1.1.0-incubating.jar

There is source code to go with it in apache-etch-java-runtime-1.1.0-incubating-src.zip.

The C# (.NET 2.0 and later) ApacheEtchCsharpIncubating.dll should be added to
any C# projects.

There is an ant plugin which you can use with ant 1.7.0 or later to invoke the
compiler as a task. It is documented on the wiki referenced above.

= Maven =

If you are using maven, then you might want to install the Etch artifacts into
your local maven repository. There is a Windows bat script to do this in the
maven directory:

  etch-maven-install.bat

You need to have maven on your path when you run this script. The file
etch-java-runtime-1.1.0.jar will be installed into your local repository with
group "etch.etch", artifact id "etch-java-runtime", and version "1.1.0". You
may then reference Etch from your maven projects.

= C and Python Bindings =

The source distribution includes code to support c and python bindings, but
these bindings are not yet complete, and there is no build products associated
with those. They are mainly there to 1) capture the code contribution from
cisco systems, and 2) inspire any fence sitters, ah, helpers to jump in and
contribute.

= Examples =

Examples have been provided in the examples/ subdirectory. There is a readme
file in each example's directory which explains how to build and run it.

= Tests =

Unit tests can be had by checking out the source code from the subversion
repository and performing a complete build:

  https://svn.apache.org/repos/asf/incubator/etch/releases/release-1.1.0

= Help =

For help, see the wiki or send email to etch-user@incubator.apache.org. See
RELEASE_NOTES.txt for specific information for this release.

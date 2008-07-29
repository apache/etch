Getting started with etch compiler development
==============================================

Setup development environment
-----------------------------

1. Install java (J2SE JDK 1.6 - 6u1). This must be at least J2SE SDK 1.6. Anything
   earlier than 1.6 update 1 will not work when you try to build Etch. Java can be
   downloaded from: http://java.sun.com/javase/downloads/index.jsp . Make certain that
   you set your JAVA_HOME environment variable to point to the correct location.
   
2. Install cygwin.  The cygwin installer/updater is located at: 
   'http://www.cygwin.com/setup.exe'.  The setup program will provide you with a tree of 
   packages that you may include in your installation.  The default selection of packages
   includes all the packages required for etch development.  Also, you will need to add 
   the "bin" subdirectory of the cygwin installation (e.g. C:\cygwin\bin) into your 
   environment's path.

Binding development
-------------------

1. Ruby for Ruby Binding

TBD

2. Python for Python Binding

TBD

3. .NET/Mono for C# 

TBD

4. gcc/Visual Studio for C/C++ binding

TBD

Ant-based development
---------------------

1. Install apache ANT (version 1.7 or later). 

2. Put /path/to/apache-ant-1.7/bin in your PATH. Confirm JAVA_HOME is set to the
   JDK 1.6 home directory.



Eclipse-based development
-------------------------

1. Install Eclipse SDK (version 3.2.2). This must be exactly version 3.2.2. Version 3.3
   will not work. Some of the eclipse plug-ins have not yet been ported to 3.3. 
   Eclipse can be downloaded from: http://www.eclipse.org/downloads/

2. Install Eclipse plug-ins. There are several that are needed. Your best bet is
   to first add all of the plug-in update sites to Eclipse, then use Eclipse's Update
   Manager to install the latest/greatest. Here are the plug-ins and their respective
   update sites:

   JavaCC       : http://eclipse-javacc.sourceforge.net    (at least version 1.5.9)
   Subclipse    : http://subclipse.tigris.org/update    (at least version 1.2.3)
   Improve      : http://www.improve-technologies.com/alpha/updates/site.xml
   Callisto     : http://download.eclipse.org/callisto/releases
   RDT          : http://updatesite.rubypeople.org/release
   Solareclipse : http://solareclipse.sourceforge.net/updates/

   Install all of these packages and their dependencies through Eclipse. This could
   take a while on a new Eclipse install (30 minutes or more).  Checking "Automatically
   select mirrors" under Window->Preferences->Install/Update can speed up the process, and
   requires less interaction during the installation process.


Checkout and Build the Tree
---------------------------

Using Ant
---------

All "official" builds are done with the ANT system. If you are modifying code, you must make certain builds work under ANT. Each of the 'clean', 'debug', and 'release' targets must do the correct things.

1. Checkout this project into the workspace of your choice (e.g. /my/workspace ):

     http://cuae.cisco.com/repos/etch/etch/trunk

2. Change to the '/my/workspace' subdirectory and type 'ant' (note: 'ant.bat' needs to be
   in your path *and* JAVA_HOME needs to be set to the JDK 1.6 home directory).

     c:\my\workspace> ant
     
3. Ant will compile etch using the default 'debug' target and the binding support library's and 
   generate a .zip than can be installed wherever you want. The .zip file can be found in:

     ./build/Installers/etch-<Version>.zip
   
   Just unzip this file wherever you would like (e.g. /path/to/ ) and put /path/to/etch-<version>/bin in your PATH.

4. Cleaning. Use the target 'clean':
     
     c:\my\workspace> ant clean

Using Eclipse
-------------

1. Check out this projects (using the SVN Repository Exploring perspective):

http://ast-metreos-lnx1/repos/etch/etch/trunk

2. Change to the Java perspective.

3. The check out process should have also built the compiler. Sometimes
it doesn't fully get a grip on things, so if there are build errors (red
flags) on the etch project, open the etch project, open src, open etch.compiler
package, right click on EtchGrammar.jj, and select Compile with JavaCC (at
the bottom).

An alternative is to pick the project menu at the top, pick clean, then
clean all projects.

4) The etch project will have some errors, because you haven't compiled
the example idls yet. that's ok.

5) To compile the idls in the workspace, run the 'doit.bat' file located in the
project root directory.  

6) You can also compile a single idl through eclipse. For the chat example, 
open the 'etch.compiler.chat' package under src (etc/src/etch.examples.chat)
and select 'Chat.etch'.  Open the 'External Tools' dialog from Run->External Tools
or by clicking on the toolbar icon with the red briefcase. In the dialog, expand
'Program', select the 'Etch (java)' child, and then select run (lower right).

The compiler will run, compiling the idl and putting the result in the etch
project at src/etch/examples/chat/. Since these are new files, you will have to let 
eclipse know they are there:

Select the etch project, hit F5. or right click on it, and pick refresh. The
compile errors for the etch project should go away.

7) unit tests

Most projects have some unit tests. In the java view, you can run these using
the java perspecting project view. Right click on the etch project,
select run as (junit). All the unit tests for the project will run.


Making a Binding
----------------

the bindings are defined in the etch project, in the package etch.bindings.blah.
there is a java binding and a ruby one in progress.

in the compiler subpackage of the binding is the  script helper and binding
driver (Compiler.java) and the scripts invoked by the driver which do the
heavy work.

in the msg subpackage is the message abstraction used by the compiler to encode
the calls and results.

in the support subpackage are the external interfaces and stuff to glue the
bits together.

Transports
----------

The transports (in etch, etch.io) are in a state of extended demo and mostly
serve as examples, demos, and tests.

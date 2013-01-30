Welcome to the 1.0.2-incubating release of Apache Etch.

The jumping off point for Etch information and help is here:

http://cwiki.apache.org/ETCH

The top-level structure of the install image is:

	ChangeLog.txt
	LICENSE.txt
	README.txt
    NOTICE.txt
    RELEASE_NOTES.txt
	bin/
	lib/
    examples/
	maven/
    uninst.exe (windows only)

Please take a moment to review the RELEASE_NOTES.txt, ChangeLog.txt and
LICENSE.txt files.

The Windows installer (apache-etch-1.0.2-incubating-setup.exe) has created a 
user environment variable (ETCH_HOME) which points to the Etch installation
directory. If you installed Etch using either apache-etch-1.0.2-incubating-bin.tar.gz
or apache-etch-1.0.2-incubating-bin.zip, you will want to create this environment
variable yourself:

windows:
	set ETCH_HOME=C:\Program Files\Apache Software Foundation\apache-etch-1.0.2-incubating

*nix:
	export ETCH_HOME=/path/to/apache-etch-1.0.2-incubating

The bin directory has also been put on your path by the Windows installer. If
you are using the archives, you will need to do this yourself:

windows:
	PATH %PATH%;%ETCH_HOME%\bin

*nix:
	export PATH="$PATH:$ETCH_HOME/bin"

The bin/ directory contains a Windows bat script and a unix shell script.

You will also need to have a java sdk installed (later versions of 1.5 or
any version of 1.6). A JAVA_HOME environment variable should point to the
installation directory of the java sdk.

In the end, at a command line or shell, you should be able to run the etch
compiler and see some basic output:

windows:
	C:\>etch
	etch: option '-b binding' must be specified on the command line

unix:
	bash-3.2$ etch
	etch: option '-b binding' must be specified on the command line

The lib directory contains the various jar files, zipped sources, and a C# dll:

	Etch.dll
	etch-ant-plugin-1.0.2-src.zip
	etch-ant-plugin-1.0.2.jar
	etch-java-runtime-1.0.2-src.zip
	etch-java-runtime-1.0.2.jar
	...

The main items of interest are the language binding runtimes, which you need to
link with your projects.

For java you need to put on the classpath of your projects: etch-java-runtime-1.0.2.jar.
There is source code to go with it in etch-java-runtime-1.0.2-src.zip.

The C# (.NET 2.0) Etch.dll should be added to any C# projects.

There is an ant plugin which you can use with ant 1.7.0 or later to invoke the
compiler as a task. It is documented on the wiki referenced above.

If you are using maven, then you might want to install the Etch artifacts into
your local maven repository. There is a Windows bat script to do this in the
maven directory:

	etch-maven-install.bat

You need to have maven on your path when you run this script. The file
etch-java-runtime-1.0.2.jar will be installed into your local repository with
group "etch.etch", artifact id "etch-java-runtime", and version "1.0.2". You
may then reference Etch from your maven projects.

Examples have been provided in the examples/ subdirectory.

Unit tests can be had by checking out the source code from the
subversion repository and performing a complete build:

 https://svn.apache.org/repos/asf/incubator/etch/releases/release-1.0.2

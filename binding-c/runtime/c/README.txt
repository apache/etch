==================================================
Introduction to binding-c
==================================================
To build the binding-c components, the following dependencies
are needed.
- Apache APR Source Version 1.4.8
  http://apr.apache.org/download.cgi
- Apache APR Util Sources Version 1.5.2
  http://apr.apache.org/download.cgi
- Apache APR Iconv Sources Version 1.2.1
  http://apr.apache.org/download.cgi
- CUnit 2.1
  http://sourceforge.net/projects/cunit/
- CMake 2.8 or higher
  http://www.cmake.org/

==================================================
Build for windows
==================================================
The following instructions were tested using MS Visual Studio 2005 SP1
on Windows 7. It should work with newer Visual Studio versions too, but
was not tested.

You should have the following folder and file structure inside your external
etch directory "ETCH_EXTERNAL_DEPENDS".

APR and APR-ICONV libraries
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/bin
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/bin/libapr-1.dll
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/bin/libapr-1.pdb
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/bin/libapriconv-1.dll
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/bin/libapriconv-1.pdb
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/iconv
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/iconv/*.so + *.pdb (coding tables)
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/include
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/include/apr-1
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/include/apr-1/*.h (header files)
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/lib
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/lib/libapr-1.lib
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/lib/libapriconv-1.lib

APR and APR-ICONV can be build via MS Visual Studio GUI. After that, all needed
artifacts have to be copied to above folder and file structure. If you do so, please
follow the instructions on http://apr.apache.org/compiling_win32.html. The installation
to <ETCH_EXTERNAL_DEPENDS> have to be done by hand. Needed projects are: apr,
apriconv, libapr, libapriconv, libapriconv_ccs_modules and libapriconv_ces_modules.

A much more easy way is to use our apr-install.bat script (../trunk/scrips/). To run the
script a cygwin installation with wget, unzip, rm, mv is required. Please check also if
cygwin/bin is in your environment path. For build apr libraries, follow the next steps:
1. create a new folder beside your <ETCH_EXTERNAL_DEPENDS> etc. c:/projects/etch/external_sources
2. edit apr-install.bat script and set INSTALL_PREFIX to your <ETCH_EXTERNAL_DEPENDS> folder and
   VC_HOME to your MS Visual Studio VS folder. You can also set this variables as environment
   variables.
3. enter your created folder (see 1.) and execute the following commands:
   - apr-install.bat download (downloading apr sources)
   - apr-install.bat convert (converting VS 6.0. dsw files to newer sln format)
   - apr-install.bat build (build all needed libraries)
   - apr-install.bat install (install all needed files to the <ETCH_EXTERNAL_DEPENDS> folder)

CUnit
- <ETCH_EXTERNAL_DEPENDS>/cunit/2.1/bin
- <ETCH_EXTERNAL_DEPENDS>/cunit/2.1/include
- <ETCH_EXTERNAL_DEPENDS>/cunit/2.1/include/*.h (header files)
- <ETCH_EXTERNAL_DEPENDS>/cunit/2.1/lib
- <ETCH_EXTERNAL_DEPENDS>/cunit/2.1/lib/libcunit.lib
- <ETCH_EXTERNAL_DEPENDS>/cunit/2.1/lib/vc80_libcunit.pdb
- <ETCH_EXTERNAL_DEPENDS>/cunit/2.1/res
- <ETCH_EXTERNAL_DEPENDS>/cunit/2.1/res/*.dtd + *.xsl (shared files, nedded to show cunit results)

Open the CUNIT solution in VS ([external_source]/cunit/VS8/CUnit.sln) and build
libcunit project. After build, copy nedded artificts to <ETCH_EXTERNAL_DEPENDS>
see above.

Etch
For building Etch you need cmake on your system to generate the build templates
for MS Visual Studio, Make. Use the cmake-gui or command line version to configure
and generate the project artifacts. In both cases you have to set the source and
binaries folders via CMake.
- source to ../trunk/binding-c/runtime/c
- binary to ../trunk/binding-c/runtime/c/target
To tell CMake how to find all the depending libraries, you have to set a CMake
variable called ETCH_EXTERNAL_DEPENDS, that points to your <ETCH_EXTERNAL_DEPENDS>
path.

After CMake has generated all project artifacts you can build the binding-c runtime
etc. via. MS Visual Studio (../trunk/binding-c/runtime/c/target/etch-c.sln). The Etch
libary and unit tests can be found under ../trunk/binding-c/runtime/c/target/src/main
and ../trunk/binding-c/runtime/c/target/src/test.

Please note that tests need libapr-1.dll and libapriconv-1.dll either in your path environment
variable or in case of MS Visual Studio under the ..\binding-c\runtime\c\target\src\test\ folder.
Additionally, you have to set APR_ICONV_PATH to <ETCH_EXTERNAL_DEPENDS>\apr\1.4.8\iconv\ to make
character conversion run. 

==================================================
Build for linux
==================================================
The following instructions were tested using gcc 4.3.x on Ubuntu Linux 10. It should work
with other Linux distributions as well.

You should have the following folder and file structure inside your external
etch directory "ETCH_EXTERNAL_DEPENDS".

APR and APR-ICONV libraries
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/bin
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/include
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/include/apr-1
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/include/apr-1/*.h (header files)
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/lib
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/lib/libapr-1.so
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/lib/libapriconv-1.so
- <ETCH_EXTERNAL_DEPENDS>/apr/1.4.8/lib/iconv/*.so (coding tables)

APR and APR-ICONV can be build via "configure" and make. Please set correct install
location to configure script configure --prefix=$INSTALL_PREFIX/apr/1.4.8/ for apr
and configure --prefix=$INSTALL_PREFIX/apr/1.4.8/ --with-apr=$INSTALL_PREFIX/apr/1.4.8/
for apr-iconv.

A much more easy way to build apr is to use our apr-install.sh script (). For build the apr libraries,
follow the next steps:
1. create a new folder beside your <ETCH_EXTERNAL_DEPENDS> e.g. home/user/projects/etch/external_sources
2. edit apr-install.sh script and set INSTALL_PREFIX to your <ETCH_EXTERNAL_DEPENDS> folde. You can also
   set this variables as environment variable.
3. enter to build folder (e.g., ../etch/external_sources, see 1.) and execute the following
   commands:
   - apr-install.sh (downloading apr sources, build apr libraries, install libraries)
 
The CUnit framework can be install via the packet installer.
- sudo apt-get install libcunit1
- sudo apt-get install libcunit1-dev

Etch
For building Etch you need cmake on your system to generate the build templates
for Make. Use the cmake-gui or command line version to configure and generate the
project artifacts. In both cases you have to set the source and binaries folders
via CMake.
- source to ../trunk/binding-c/runtime/c
- binary to ../trunk/binding-c/runtime/c/target
To tell CMake how to find all the depending libraries, you have to set a CMake
variable called ETCH_EXTERNAL_DEPENDS, that points to your <ETCH_EXTERNAL_DEPENDS>
path.

After CMake has generated all project artifacts you can build the binding-c runtime
etc. via. Make (../trunk/binding-c/runtime/c/target/Makefile). The Etch libary and
unit tests can be found under ../trunk/binding-c/runtime/c/target/src/main and 
../trunk/binding-c/runtime/c/target/src/test.

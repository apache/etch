
Configuring properties for a test project in visual studio
=======================================================================================
NOTE: also see "troubleshooting" section below.
Make these changes to project/properties defaults:

Environment Variables
  APRPATH   must identify the directory in which the Apache Portable Runtime is installed.
            At this writing this is C:\workspace\tools\apache-apr\apr-1-2-11
  CUNITPATH must identify the directory in which Cunit is installed.
            At this writing this is C:\workspace\tools\cunit\cunit-2.1-0

Configuration Properties
  C/C++

    General
      * Additional Include Directories:  
         ..\..\..\inc;
         ..\..\..\ext\hashtab;
         $(APRPATH)\inc
         $(CUNITPATH)\inc
      * Debug Information Format: C7 Compatible (/Z7)
        (to intergate debug info such that we can step into multiple libs)
      * Treat Warnings as Errors: Yes (/WX)
        (since MS C compiler "warnings" are frequently serious errors)
        
    Preprocessor 
      * Preprocessor Definitions: add APR_DECLARE_STATIC to this list
		for example, WIN32;_DEBUG;_CONSOLE; APR_DECLARE_STATIC
        (targets the static APR library)

    Code Generation
      * Enable Minimal Rebuild: No
        (must be off in order for /Z7 to take effect)
      * Enable C++ Exceptions:  No

    Precompiled Headers
      * Create/User Precompiled Header: Not Using Precompiled Headers

    Advanced
      * Compile As: Compile as C Code (/TC)

  Linker
  
    General
      * Enable Incremental Linking: No (/INCREMENTAL:NO)  
      
    Additional Library Directories
      ..\..\..\lib
      ..\..\..\ext\lib
      $(APRPATH)\lib
      $(CUNITPATH)\lib\win32
      
    Input
      * Additional Dependencies: 
          libcunit.lib etch.lib jenkhash.lib apr-1.lib WS2_32.LIB Mswsock.lib       

    Command Line
      * Additional options: /NODEFAULTLIB:LIBCMTD


Additional linker import libraries
=======================================================================================
We might have copied references to libraries into the "Resource Files" folder of a test
project. Libs may include libcunit.lib, common.lib, message.lib. Contro/drag from a lib 
folder, or from another test project. This should not be the case, all link libs should
be specified in linker options, however there may be early projects with references
specified in Resource Files.


Linker warning notes:
=======================================================================================

Rebuilding a test project may elicit this linker warning
  LNK4099: PDB 'vc80.pdb' was not found with '..\..\..\lib\common.lib' 

However we are compiling libraries (common, message, etc) with compatibility mode (/Z7),
which means we do not need .pdb files for debug information. The linker warning LNK4099
apparently cannot be suppressed however. This warning notwithstanding, you can step   
into or breakpoint the library code from any test.


Troubleshooting:
=======================================================================================
1. Problem:  Link errors on APR symbols, and the test's .obj file has a "1" appended.
   Solution: Ensure APR_DECLARE_STATIC is included in Preprocessor Definitions 
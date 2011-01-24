The current state of binding-cpp is under development. 

BUILD:
*****
The Build is cmake based. To build the sources, do:

cd binding-cpp/runtime
mkdir target
cd target
cmake -DETCH_EXTERNAL_DEPENDS:string=/your/path/to/external/dependencies ..

NOTES: 
******

External dependencies: 
- Google Test 1.5.0 (http://code.google.com/p/googletest/)

Directory structure:

/your/path/to/external/dependencies/
	/gtest/1.5.0			contents of the Google Test tgz
	

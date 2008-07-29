to compile the idl file, change to the directory where the idl file is:

C:\workspace\scott\src\corba\test>idlj -fall -td ../.. -pkgPrefix HelloApp corba.test Hello.idl

once compiled, you will have to go back and refresh the corba.test package in eclipse. (select it and hit F5.)

before you can run it, you will need to run orbd:

C:\Program Files\Java\jdk1.5.0_10\bin>orbd


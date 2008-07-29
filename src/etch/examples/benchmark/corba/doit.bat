set cp=.;..\..\..\..\..\build

idlj -fall Hello.idl

cd HelloApp
javac *.java
cd ..

javac -cp %cp% *.java

start orbd
@pause

start java -cp %cp% HelloServer
@pause

java -cp %cp% HelloClient

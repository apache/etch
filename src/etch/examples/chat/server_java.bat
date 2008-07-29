mkdir build
javac *.java -classpath ../../lib/etch-java-runtime.jar -d build
java -classpath build;../../lib/etch-java-runtime.jar etch.examples.chat.TestRemoteChatListener
rmdir  build /s /q

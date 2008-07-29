mkdir build
javac -classpath ../../lib/etch-java-runtime.jar *.java -d build
java -classpath build;../../lib/etch-java-runtime.jar etch.examples.distmap.DistributedHashTableListener
rmdir build \q \s

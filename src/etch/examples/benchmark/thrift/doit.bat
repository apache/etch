set cp=..\..\..\..\..\build;..\..\..\..\..\libs\libthrift.jar

start java -cp %cp% etch.examples.benchmark.thrift.PerfListener

@pause

java -cp %cp% etch.examples.benchmark.thrift.RunPerf


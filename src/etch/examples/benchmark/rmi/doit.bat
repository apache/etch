@rem you might have to edit this classpath to
@rem include the etch runtime support library
@rem for java. this path assumes you're using
@rem eclipse and so references the eclipse build
@rem directory.

set cp=.;../../../../../build

javac SampleServer.java
javac SampleServerImpl.java
rmic SampleServerImpl
javac -cp %cp% SampleClient.java

@echo next we will start some auxillary programs which
@echo will open windows. after each window opens, you
@echo will need to come back to this window and confirm
@echo to proceed to the next step. when done, you may
@echo close the other windows.
@pause

start rmiregistry
@echo wait for rmiregistry to start
@pause 

start java -Djava.security.policy=policy.all SampleServerImpl
@echo wait for server to start
@pause 

@echo next we will run the benchmark. please shutdown all
@echo other applications which may interfere with the
@echo benchmark. avoid using this computer while the benchmark
@echo is running.
@pause

java -Djava.security.policy=policy.all -cp %cp% SampleClient

@echo benchmark is done. you may close the other windows now.
@del *.class

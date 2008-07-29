this java rmi benchmark is automated. please shutdown all
non-essential programs. best results will be obtained if
you connect directly to the console (instead of using rdc
or vnc).

running the benchmark

using a windows command prompt, change into the directory
where this readme file is located. there you will find a
file doit.bat. run it.

example:

C:\workspace-etch\etch\src\etch\examples\benchmark\rmi>dir
 Volume in drive C has no label.
 Volume Serial Number is 6C44-FAE6

 Directory of C:\workspace-etch\etch\src\etch\examples\benchmark\rmi

10/23/2007  09:19 AM    <DIR>          .
10/23/2007  09:19 AM    <DIR>          ..
10/23/2007  09:12 AM               881 doit.bat
08/14/2007  10:53 AM               128 policy.all
10/23/2007  09:19 AM               368 README.txt
08/14/2007  10:53 AM            98,816 rmi.doc
08/14/2007  10:58 AM             1,566 SampleClient.java
08/14/2007  10:56 AM               134 SampleServer.java
08/14/2007  10:56 AM             1,210 SampleServerImpl.java
               7 File(s)        103,103 bytes
               2 Dir(s)  143,397,904,384 bytes free

C:\workspace-etch\etch\src\etch\examples\benchmark\rmi>doit

C:\workspace-etch\etch\src\etch\examples\benchmark\rmi>javac SampleServer.java

C:\workspace-etch\etch\src\etch\examples\benchmark\rmi>javac SampleServerImpl.java

C:\workspace-etch\etch\src\etch\examples\benchmark\rmi>rmic SampleServerImpl

C:\workspace-etch\etch\src\etch\examples\benchmark\rmi>javac SampleClient.java
next we will start some auxillary programs which
will open windows. after each window opens, you
will need to come back to this window and confirm
to proceed to the next step. when done, you may
close the other windows.
Press any key to continue . . .

C:\workspace-etch\etch\src\etch\examples\benchmark\rmi>start rmiregistry
wait for rmiregistry to start
Press any key to continue . . .

C:\workspace-etch\etch\src\etch\examples\benchmark\rmi>start java -Djava.security.policy=policy.all SampleServerImpl
wait for server to start
Press any key to continue . . .
next we will run the benchmark. please shutdown all
other applications which may interfere with the
benchmark. avoid using this computer while the benchmark
is running.
Press any key to continue . . .

C:\workspace-etch\etch\src\etch\examples\benchmark\rmi>java -Djava.security.policy=policy.all SampleClient
Security Manager loaded
Got remote object
 1 + 2 = 3
took 14014564003 nanos
benchmark is done. you may close the other windows now.

C:\workspace-etch\etch\src\etch\examples\benchmark\rmi>
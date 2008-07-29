After you checkout the project in eclipse, you will likely be told that there are
build path problems. There are two dependent projects you need to get going with
etch for eclipse, and one plugin:

javacc 4.0
junit 4.3.1
velocity 1.5

javacc is an eclipse plugin, get it from here:

http://eclipse-javacc.sourceforge.net/

On that page are directions to install the plugin from within eclipse.

The other two you download:

http://sourceforge.net/projects/junit/

http://velocity.apache.org/engine/releases/velocity-1.5/

Once downloaded, you will need to create an environment variable (for example):

ETCH_DEPENDENT_JARS=C:\workspace-etch\tools\velocity\1.5\velocity-dep-1.5.jar

You will need to restart eclipse if it is running.

Once eclipse is started, you will need to right click on the etch project,
and select Build Path / Configure Build Path...

Select the Libraries tab on the right.

If ETCH_DEPENDENT_JARS is here, delete it.

Select Add Library... on the right.

Select User Library and click Next.

Select User Libraries...

Select New...

Enter the name ETCH_DEPENDENT_JARS and click Ok.

With ETCH_DEPENDENT_JARS selected, click Add JARs...

Navigate to junit-4.3.1.jar and select it.

With ETCH_DEPENDENT_JARS selected, click Add JARs... again...

Navigate to velocity-dep-1.5.jar and select it.

Click Ok.

Click Finish.

Click Ok.

Eclipse should rebuild the project. There may still be errors, that's ok.

Open Window / Preferences / Java / Compiler / Building.

Expand Build path problems.

Set Incomplete build path to be a warning.
Set Circular dependencies to be an error.
Set Incompatible required binaries to warning.

Click Ok.

Make sure JavaCC compiled EtchGrammar.jj. There should be a JavaCC console
view open. If not, open it (Window / Show View / Other... / JavaCC console /
JavaCC console).

If JavaCC console is empty, Select Project / Clean... and clean all projects.
A few JavaCC messages should appear. It is ok if JavaCC warns about creating
a directory.

Now that the compiler is built, you will still have errors for etch build
products which are missing. you will need to recompile all the etch files.
You can do this by:

Select the etch project.

Select Run / External Tools / Compile Java Etch Files.

(You may want to configure the external tools first. Select Run / External
Tools / Organize Favorites... Click Add... Select All. Ok. Ok.)

You should see a nice output in the Console reporting successful compilation
of a bunch of etch files. Eclipse should then rebuilt the project.

Finally, you can check things out by running the unit tests.

Right click on the etch project, select Run as... / Junit test.

You'll get some output on the console window, including scary looking stack
traces. That's ok. JUnit runner should tell you that 1197 tests passed, 22
ignored, with 0 errors and 0 failures. This takes 66 seconds for me.

You're done, start exploring. I suggest you check out examples, perf or chat.
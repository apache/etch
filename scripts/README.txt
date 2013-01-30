These scripts and launch configurations are used in conjunction with eclipse.

NOTE: These are windows versions. If you are using unix, you'll have to come up
with your own versions of these.

In order for these scripts to work properly, you also need to add a variable to
your environment before starting eclipse:

ETCH_DEPENDENT_JARS=C:\workspace-etch\tools\velocity\1.5\velocity-dep-1.5.jar

If you're using clover, you'll need to add clover to that:

ETCH_DEPENDENT_JARS=C:\workspace-etch\tools\velocity\1.5\velocity-dep-1.5.jar;C:\workspace-etch\tools\clover-ant\2.3.2\lib\clover.jar

In the above, please edit the path as appropriate for your local environment.

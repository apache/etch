to run the standard etch / java perf test:

run PerfListener, wait for it to start and print its happy message.

then run RunPerfServer. it will run for awhile, printing stats.
mainly you want to do this on a very quiet machine, and report
the median numbers for each of the tests add, sum, and report.

(it is easiest to run these tests in eclipse, but you can
run them from a standard build, too, like this:

each of these commands should be run in a separate command tool window:

java -cp <blah> etch.examples.benchmark.etch.PerfListener

java -cp <blah> etch.examples.benchmark.etch.RunPerfServer

in both cases, <blah> is the path to the build tree.
@set q=-q
@set d=src
@set i=src
@set b=java

@set wall=-w all
@set wallforce=-w all,force
@set wimpl=-w impl

call bin\etch %q% -b %b% -d %d% -I %i% %wallforce% src/etch/bindings/java/compiler/test/Test1.etch
call bin\etch %q% -b %b% -d %d% -I %i%             src/etch/bindings/java/compiler/test/Test2.etch
call bin\etch %q% -b %b% -d %d% -I %i%             src/etch/bindings/java/compiler/test/Test3.etch
call bin\etch %q% -b %b% -d %d% -I %i%             src/etch/bindings/java/compiler/test/Test4.etch
call bin\etch %q% -b %b% -d %d% -I %i%             src/etch/bindings/java/compiler/test/Test6.etch
call bin\etch %q% -b %b% -d %d% -I %i%             src/etch/bindings/java/compiler/test/TestIncludes.etch
call bin\etch %q% -b %b% -d %d% -I %i% %wimpl%     src/etch/bindings/java/compiler/test/Inheritance.etch

call bin\etch %q% -b %b% -d %d% -I %i% %wimpl%     src/etch/examples/async/Async.etch
call bin\etch %q% -b %b% -d %d% -I %i% %wimpl%     src/etch/examples/benchmark/etch/Perf.etch
call bin\etch %q% -b %b% -d %d% -I %i% %wimpl%     src/etch/examples/chat/Chat.etch
call bin\etch %q% -b %b% -d %d% -I %i% %wimpl%     src/etch/examples/closing/Closing.etch
call bin\etch %q% -b %b% -d %d% -I %i% %wimpl%     src/etch/examples/cuae/Cuae.etch
call bin\etch %q% -b %b% -d %d% -I %i% %wimpl%     src/etch/examples/distmap/DistributedHashTable.etch
call bin\etch %q% -b %b% -d %d% -I %i% %wallforce% src/etch/examples/example/Example.etch
call bin\etch %q% -b %b% -d %d% -I %i% %wimpl%     src/etch/examples/mixin/Foo.etch
call bin\etch %q% -b %b% -d %d% -I %i% %wall%      src/etch/examples/types/Types.etch

@set b=xml

call bin\etch %q% -b %b% -d %d% -I %i% src/etch/bindings/xml/compiler/test/Test.etch
call bin\etch %q% -b %b% -d %d% -I %i% src/etch/bindings/xml/compiler/test/mixin/Foo.etch

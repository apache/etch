@set q=-q

@set b=java
call build\etch %q% -b %b% -d src src/etch/bindings/java/compiler/test/Test.etch
call build\etch %q% -b %b% -d src src/etch/bindings/java/compiler/test/Test2.etch
call build\etch %q% -b %b% -d src src/etch/bindings/java/compiler/test/Test3.etch
call build\etch %q% -b %b% -d src src/etch/bindings/java/compiler/test/Test4.etch
call build\etch %q% -b %b% -d src src/etch/examples/authentication/Authenticator.etch
call build\etch %q% -b %b% -d src src/etch/examples/benchmark/etch/Perf.etch
call build\etch %q% -b %b% -d src src/etch/examples/chat/Chat.etch
call build\etch %q% -b %b% -d src src/etch/examples/configuration/Configuration.etch
call build\etch %q% -b %b% -d src src/etch/examples/distmap/DistributedHashTable.etch
call build\etch %q% -b %b% -d src src/etch/examples/example/Example.etch
call build\etch %q% -b %b% -d src src/etch/examples/locator/Locator.etch
call build\etch %q% -b %b% -d src src/etch/examples/log/Log.etch

@set b=xml
call build\etch %q% -b %b% -d src src/etch/bindings/xml/compiler/test/Test.etch

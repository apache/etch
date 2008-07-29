@set q=-q
@set d=src
@set i=src
@set b=csharp

@set wall=all,force
@set ALL=all


call bin\etch %q% -b %b% -d %d% -f  -I %i% -w %wall% src/etch/bindings/csharp/compiler/test/Test1.etch

call bin\etch %q% -b %b% -d %d% -f -I %i% -w %ALL% src/etch/bindings/csharp/examples/async/Async.etch
call bin\etch %q% -b %b% -d %d% -f  -I %i% -w %ALL% src/etch/bindings/csharp/examples/closing/Closing.etch
call bin\etch %q% -b %b% -d %d% -f  -I %i% -w both,impl src/etch/bindings/csharp/examples/cuae/Cuae.etch
call bin\etch %q% -b %b% -d %d% -f  -I %i% -w %ALL% src/etch/bindings/csharp/examples/inheritance/Inheritance.etch
call bin\etch %q% -b %b% -d %d%   -f  -I %i%  -w %ALL% src/etch/bindings/csharp/examples/mixin/Foo.etch
call bin\etch %q% -b %b% -d %d% -f  -I %i% -w %ALL% src/etch/bindings/csharp/examples/types/Types.etch


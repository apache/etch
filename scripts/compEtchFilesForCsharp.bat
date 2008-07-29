@set q=-q
@set b=-b csharp
@set d=-d ../../../target/generated-sources/main/etch/csharp
@set i=-I .
@set n=-n

@set walf=-w all,force

@set testsdir=tests\src\main\etch
@set examplesdir=examples

@set x=%CD%\
@pushd %testsdir%

@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        Async.etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        Bar.etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        Baz.etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        BigIdl.etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        Closing.etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        Cuae.etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        Foo.etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        Inheritance.etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i% %walf% Test1.etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        Test2.etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        Test3.etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        Test4.etch
@rem *** Test5.etch is a negative test ***
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        Test6.etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i% %walf% TestIncludes.etch
@rem *** TestReservedWords.etch is a negative test ***
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        Types.etch

@popd
@cd %examplesdir%

@pushd chat\src\main\etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        Chat.etch
@popd

@pushd distmap\src\main\etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        DistributedHashTable.etch
@popd

@pushd example\src\main\etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        Example.etch
@popd

@pushd perf\src\main\etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i%        Perf.etch
@popd

@popd
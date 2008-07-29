call ant clean

del binding-csharp\compiler\src\main\java\etch\bindings\csharp\compiler\CompilerVersion.java
del binding-xml\compiler\src\main\java\etch\bindings\xml\compiler\CompilerVersion.java
del binding-java\compiler\src\main\java\etch\bindings\java\compiler\CompilerVersion.java
del compiler\src\main\java\etch\compiler\Version.java
del binding-csharp\runtime\src\main\csharp\Properties\AssemblyInfo.cs
svn update .

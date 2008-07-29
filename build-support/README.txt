Understanding the build-support scripts
---------------------------------------

The master build script 'build.xml' is located in the workspace root directory. The 'build.xml'
script will call all other build scripts.

Two files are included by all scripts:

* build.common.xml   -- standard properties (derived from build-env) and macros
* build.includes.xml -- "virtual" targets used as baseline for all other scripts

The scripts are broken down as follows:

* compiler.xml          -- builds the Etch compiler and the Etch-Java binding, all other 'support' scripts depend on this one
* csharp-support.xml    -- builds the Etch-C# support libraries
* ruby-support.xml      -- builds the Etch-Ruby support libraries
* c-support.xml         -- builds the Etch-C support libraries
* xml-support.xml       -- builds the Etch-XML support libraries
* example-FOO.xml       -- builds the 'FOO' example
* examples.xml          -- THIS IS A TEMPLATE, it should be copied to a new name, e.g. example-FOO.xml, and modified
* docs.xml              -- builds the documentation set
* packages.xml          -- builds the binary distribution kits



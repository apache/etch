This is the Etch binding library for Python.

When the Etch compiler generates Python code from an Etch IDL file,
that generated code references this module.

This module depends upon Python version 2.6 or later and will run under
Win32, *Nix and Mac OSX.

To build, the following modules are required:

 * setuptools
 * nose
 * NoseXUnit

This module assumes that the Etch compiler can be found at the location
pointed to by the environment variable ETCH_HOME.

To install this module execute:

> python setup.py install

Enjoy!


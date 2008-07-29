be sure and define this:

	CXF_HOME=C:\apache-cxf-2.0.1-incubator

in a command window, run the server:

	ant SimpleServicePortTypeServer

in a separate command window, run the client:

	ant SimpleServicePortTypeClient

to build the source files from the wsdl, use this:

	wsdl2java -d . -all SimpleWebService.wsdl

be careful with that -all, as it might overwrite your
existing implementations.

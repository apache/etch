set CXF_HOME=C:\apache-cxf-2.0.1-incubator

start ant SimpleServicePortTypeServer
@pause

ant SimpleServicePortTypeClient

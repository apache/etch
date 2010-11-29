FILES= \
	BaseHelloWorldClient.go\
	BaseHelloWorldServer.go\
	HelloWorldClient.go\
	HelloWorld.go\
	HelloWorldHelper.go\
	HelloWorldServer.go\
	ImplHelloWorldClient.go\
	ImplHelloWorldServer.go\
	RemoteHelloWorldClient.go\
	RemoteHelloWorld.go\
	RemoteHelloWorldServer.go\
	StubHelloWorldClient.go\
	StubHelloWorld.go\
	StubHelloWorldServer.go\
	ValueFactoryHelloWorld.go\

all: server
	8g -o HelloWorldclient.8 -I ${ETCHLIBPATH} -I . MainHelloWorldClient.go
	8l -L ${ETCHLIBPATH} -L . -o HelloWorldClient HelloWorldclient.8

server: 
ifndef ETCHLIBPATH
	@echo Please call \"make ETCHLIBPATH=/some/path\",where /some/path contains etch.8
	false
endif
	8g -o org_apache_etch_examples_helloworld.8 -I ${ETCHLIBPATH} -I . ${FILES}
	8g -o HelloWorldserver.8 -I ${ETCHLIBPATH} -I . MainHelloWorldListener.go
	8l -L ${ETCHLIBPATH} -L . -o HelloWorldServer HelloWorldserver.8

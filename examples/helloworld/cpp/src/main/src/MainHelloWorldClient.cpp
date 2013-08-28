
// This file automatically generated by:
//   Apache Etch 1.3.0-incubating (LOCAL-0) / c 1.3.0-incubating (LOCAL-0)
//   Fri Jul 27 11:35:36 CEST 2012
// This file is automatically created for your convenience and will not be
// overwritten once it exists! Please edit this file as necessary to implement
// your service logic.

#include "common/EtchTypes.h"
#include "support/EtchRuntime.h"
#include "RemoteHelloWorldServer.h"
#include "MainHelloWorldClient.h"
#include "ImplHelloWorldClient.h"
#include "transport/EtchTransportData.h"


using namespace org_apache_etch_examples_helloworld_HelloWorld;

HelloWorldClient* MainHelloWorldClientFactory::newHelloWorldClient(RemoteHelloWorldServer* server)
{
  return new ImplHelloWorldClient(server);
}

/**
 * Main program for HelloWorldClient.
 *
 * @param args command line arguments.
 * @throws Exception
 */
capu::int32_t main(int argc, const char* argv[])
{
  EtchRuntime* runtime = new EtchRuntime();
  runtime->start();

  // TODO Change to correct URI
  EtchString uri("tcp://127.0.0.1:4001");

  MainHelloWorldClientFactory mc;

  status_t result;
  RemoteHelloWorldServer *remote = NULL;
  result = HelloWorldHelper::newServer(runtime, uri, NULL, mc, remote);
  if (result != ETCH_OK) {
    //TODO Handle error
  }

  // Connect to the service
  result = remote->transportControl(new EtchString(EtchTransportData::START_AND_WAIT_UP()), new EtchInt32(4000));
  if (result != ETCH_OK) {
    //TODO Handle error
  }

  HelloWorld::userPtr myUser = new HelloWorld::user(new EtchInt32(1),new EtchString("World"));
  HelloWorldServer::say_helloAsyncResultPtr say_helloResult = remote->say_hello(myUser);
  if(say_helloResult->hasException()) {
    capu::SmartPointer<EtchException> exception = say_helloResult->getException();
    printf("say_hello FAILED: exception=%s \n", exception->getErrorMessage().c_str());
  }
  if(say_helloResult->hasResult()) {
    capu::SmartPointer<EtchString> result = say_helloResult->getResult();
    printf("say_hello OK: result=%s\n", result->c_str());
  }

  // Disconnect from the service
  remote->transportControl(new EtchString(EtchTransportData::STOP_AND_WAIT_DOWN()), new EtchInt32(4000));

  //waiting
  printf("\n\npress any key to close client\n");
  getchar();

  //delete remote
  delete remote;

  //delete runtime
  result = runtime->shutdown();
  if (result != ETCH_OK) {
    //TODO Handle error
  }

  delete runtime;

  return 0;
}


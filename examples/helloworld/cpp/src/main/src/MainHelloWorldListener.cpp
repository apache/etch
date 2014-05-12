// This file automatically generated by:
//   Apache Etch 1.4.0 (LOCAL-0) / cpp 1.4.0 (LOCAL-0)
//   Fri Apr 04 11:35:36 CEST 2014
// This file is automatically created for your convenience and will not be
// overwritten once it exists! Please edit this file as necessary to implement
// your service logic.

#include "MainHelloWorldListener.h"
#include "ImplHelloWorldServer.h"
#include "RemoteHelloWorldClient.h"

#include "capu/os/Thread.h"
#include "common/EtchTypes.h"
#include "support/EtchRuntime.h"
#include "transport/EtchTransportData.h"

#ifdef BUILD_CHECK_MEMORY
#include "vld.h"
#endif

using namespace org_apache_etch_examples_helloworld_HelloWorld;

HelloWorldServer* MainHelloWorldListener::newHelloWorldServer(RemoteHelloWorldClient* client)
{
  return new ImplHelloWorldServer(client);
}

/**
 * Main program for HelloWorldServer.
 *
 * @param args command line arguments.
 * @throws Exception
 */
int main(int argc, const char* argv[])
{
  status_t status;
  EtchRuntime* runtime = new EtchRuntime();
  status = runtime->start();

  // TODO Change to correct URI
  EtchString uri("tcp://0.0.0.0:4001");

  MainHelloWorldListener mainHelloWorldlistener;

  EtchServerFactory *listener;
  status = HelloWorldHelper::newListener(runtime, uri, NULL, &mainHelloWorldlistener, listener);


  //start the Listener
  status = listener->transportControl(new EtchString(EtchTransportData::START_AND_WAIT_UP()), new EtchInt32(4000));
  if (status == ETCH_OK) {
    printf("Hello World Server started successfully. Listening on port 4001\n\n");
  } else {
    printf("Error while starting Hello World Server. Errorcode %d \n\n", status);
    return -1;
  }
  
  //waiting
  printf("press any key to stop server\n");
  getchar();

  //stop the Listener
  listener->transportControl(new EtchString(EtchTransportData::STOP_AND_WAIT_DOWN()), new EtchInt32(4000));

  //delete listener
  delete listener;

  //delete runtime
  status = runtime->shutdown();
  
  delete runtime;
  return 0;
}


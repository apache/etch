using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Etch.Transport;
using etch.bindings.csharp.examples.async;

namespace Etch.Compiler.Test
{
    [TestFixture]
    public class TestAsyncClient
    {

        private static RemoteAsyncServer server;
        private static  Etch.Transport.Transport<SessionListener> listener;

        [TestFixtureSetUp]
        public void Setup()
        {
      //      MainAsyncListener.Main(new String[] { });
  
            String uri = "tcp://localhost:4010";         

            MainAsyncListener implFactory = new MainAsyncListener();
            listener = AsyncHelper.NewListener(uri, null, implFactory);

            listener.TransportControl(TransportConsts.START_AND_WAIT_UP, 4000);

            MainAsyncClient client = new MainAsyncClient();
            server = AsyncHelper.NewServer(uri, null, client);
          

            server._StartAndWaitUp(4000);
        }


        [Test]
        public void sync1()
        {

            Assert.AreEqual(0, server.sync(0));

        }

       

        [Test]
        public void async_queued1()
        {
            for (int i = 0; i < 5; i++)
                Assert.AreEqual(i, server.async_queued(i));
        }
        
        
        

        
        [Test]
	    public void async_free() 
	    {
		    for (int i = 0; i < 10; i++)
                Assert.AreEqual(i, server.async_free(i));
	    }

        [TestFixtureTearDown]
        public void Method_CloseNewTestListener()
        {
           
                server._StopAndWaitDown(4000);
                listener.TransportControl(TransportConsts.STOP_AND_WAIT_DOWN,4000);
        }
 
    } 
}

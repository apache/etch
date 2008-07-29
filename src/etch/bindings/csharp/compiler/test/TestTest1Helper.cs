using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Etch.Transport;
using Etch.Util;
using etch.bindings.csharp.compiler.test;

namespace Etch.Compiler.Test
{
    [TestFixture]
    public class TestTest1Helper
    {
        public static Etch.Transport.Transport<SessionListener> listener;
        
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestHelper" );
        }

        [SetUp]
        public void Method_OpenNewTestListener()
        {
            TransportFactory.Add( "blah", new TcpTransportFactory() );

            string uri = "blah://localhost:4040";

            MainTest1Listener implFactory = new MainTest1Listener();

            listener = Test1Helper.NewListener( uri, null, implFactory );
            listener.TransportControl( TransportConsts.START_AND_WAIT_UP, 4000 );
        }

        [Test]
        public void Method_NewTestServer()
        {
            Boolean flag = true;

            string uri = "blah://localhost:4040";

            MainTest1Client client = new MainTest1Client();

            RemoteTest1Server server;

            try
            {
                server = Test1Helper.NewServer( uri, null, client );
                server._StartAndWaitUp( 4000 );

                // Insert your code here
                // -----------------------------------------------------------------------

                server._Stop();
            }
            catch ( Exception e )
            {
                flag = false;
                Console.WriteLine( e.StackTrace );
                Assert.IsTrue(flag);
            }

            Assert.IsTrue( flag );
        }

        [TearDown]
        public void Method_CloseNewTestListener()
        {
            listener.TransportControl( TransportConsts.STOP_AND_WAIT_DOWN, 0 );
        }
    }
}

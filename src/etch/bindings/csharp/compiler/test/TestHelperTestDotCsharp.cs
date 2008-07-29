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
    public class TestHelperTestDotCsharp
    {
        public static Etch.Transport.Transport listener;

        [SetUp]
        public static void Method_OpenNewTestListener()
        {
            TransportFactory.Add( "blah", new TcpTransportFactory() );

            URL uri = new URL( "blah://127.0.0.1:4000" );

            MainTestListener implFactory = new MainTestListener();

            listener = TestHelper.NewTestListener( uri, null, implFactory );
            listener.TransportControl( TransportConsts.START_AND_WAIT_UP, 4000 );
        }

        [Test]
        public void Method_NewTestServer()
        {
            Boolean flag = true;

            URL uri = new URL( "blah://localhost:4000" );

            ImplTestClient client = new ImplTestClient( null );

            RemoteTestServer server;

            try
            {
                server = TestHelper.NewTestServer( uri, null, client );
                server._StartAndWaitUp( 4000 );

                // Insert your code here
                // -----------------------------------------------------------------------

                server._Stop();
            }
            catch ( Exception e )
            {
                flag = false;
                Console.WriteLine( e.StackTrace );
                Assert.Fail();
            }

            Assert.IsTrue( flag );
        }

        [TearDown]
        public static void Method_CloseNewTestListener()
        {
            listener.TransportControl( TransportConsts.STOP_AND_WAIT_DOWN, 0 );
        }
    }
}

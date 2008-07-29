using System;
using System.Collections.Generic;
using System.Text;

using Etch.Msg;
using Etch.Support;
using Etch.Transport;
using NUnit.Framework;
using Etch.Util;

namespace etch.examples.distmap
{
    [TestFixture]
    /// <summary>
    /// Instructions:
    /// --------------
    /// 
    /// 1) Start the TestRemoteDistributedHashTableListener.
    /// 2) Run the Nunit tests in this file.
    /// 
    /// </summary>
    public class TestRemoteDistributedHashTableServer
    {
        RemoteDistributedHashTableServer server;
        TcpConnection c;
        bool myBool1 = Constants.BOOL2;
        sbyte myByte1 = Constants.BYTE5;
        short myShort1 = Constants.SHORT5;
        int myInt1 = Constants.INT5;
        long myLong1 = Constants.LONG5;
        float myFloat1 = Constants.FLOAT5;
        double myDouble1 = Constants.DOUBLE5;
        string myString1 = Constants.STRING3;

        List<Object> myList = new List<object>();

        [TestFixtureSetUp]
        public void BeforeClass()
        {
            // connect to service //
            URL uri = new URL("tcp://localhost:4001");

            ImplDistributedHashTableClient client = new ImplDistributedHashTableClient( null );
            server = DistributedHashTableHelper.NewDistributedHashTableServer( uri, null, client );

            server._StartAndWaitUp( 4000 );

            // define constants
            myList.Add( myBool1 );
            myList.Add( myByte1 );
            myList.Add( myShort1 );
            myList.Add( myInt1 );
            myList.Add( myLong1 );
            myList.Add( myFloat1 );
            myList.Add( myDouble1 );
            myList.Add( myString1 );
            
        }

        [Test]
        public void TestInitialSize()
        {
            Assert.AreEqual( 0, server.size() );
        }

        [Test]
        public void TestGetAllInitial()
        {
            Assert.IsEmpty( server.getAll() );
        }

        public void TestPutObject()
        {
            // test against null because if its a new object in the map, 
            // the value returned is null 

            Assert.AreEqual( null, server.putObject( "boolean1", myBool1 ) );

            Assert.AreEqual( null, server.putObject( "byte1", myByte1 ) );
            
            Assert.AreEqual( null, server.putObject( "short1", myShort1 ) );
            
            Assert.AreEqual( null, server.putObject( "int1", myInt1 ) );
            
            Assert.AreEqual( null, server.putObject( "long1", myLong1 ) );
            
            Assert.AreEqual( null, server.putObject( "float1", myFloat1 ) );
            
            Assert.AreEqual( null, server.putObject( "double1", myDouble1 ) );
            
            Assert.AreEqual( null, server.putObject( "string1", myString1 ) );

            // verify size 

            Assert.AreEqual( 8, server.size() );
        }

        [Test]
        public void TestGetAll()
        {
            
            foreach ( etch.examples.distmap.ConstsDistributedHashTable.Entry entry in server.getAll() )
            {
                // one has to do a contains operation and not an equal operation 
                // because the list returned by the hashmap in java is not ordered. 

                // just to make sure that the same object is not being returned over 
                // and over again, remove the current returned object from your list. 

                Assert.Contains( entry.value, myList );
                myList.Remove( entry.value );
                
            }

        }

        [Test]
        public void TestGetObject()
        {
            Object myObj;

            myObj = server.getObject( "boolean1" );
            Assert.AreEqual( myBool1, myObj );

            myObj = server.getObject( "byte1" );
            Assert.AreEqual( myByte1, myObj );

            myObj = server.getObject( "short1" );
            Assert.AreEqual( myShort1, myObj );

            myObj = server.getObject( "int1" );
            Assert.AreEqual( myInt1, myObj );

            myObj = server.getObject( "long1" );
            Assert.AreEqual( myLong1, myObj );

            myObj = server.getObject( "float1" );
            Assert.AreEqual( myFloat1, myObj );

            myObj = server.getObject( "double1" );
            Assert.AreEqual( myDouble1, myObj );

            myObj = server.getObject( "string1" );
            Assert.AreEqual( myString1, myObj );
        }

        [Test]
        public void TestRemoveAllObjects()
        {
            Object myObj;

            myObj = server.removeObject( "boolean1" );
            Assert.AreEqual( myBool1, myObj );

            myObj = server.removeObject( "byte1" );
            Assert.AreEqual( myByte1, myObj );
            
            myObj = server.removeObject( "short1" );
            Assert.AreEqual( myShort1, myObj );

            myObj = server.removeObject( "int1" );
            Assert.AreEqual( myInt1, myObj );

            myObj = server.removeObject( "long1" );
            Assert.AreEqual( myLong1, myObj );

            myObj = server.removeObject( "float1" );
            Assert.AreEqual( myFloat1, myObj );

            myObj = server.removeObject( "double1" );
            Assert.AreEqual( myDouble1, myObj );

            myObj = server.removeObject( "string1" );
            Assert.AreEqual( myString1, myObj );

        }

        [Test]
        public void TestSizeEnd()
        {
            Assert.AreEqual( 0, server.size() );
        }

        [Test]
        public void TestGetAllEnd()
        {
            Assert.IsEmpty( server.getAll() );
        }

        [TestFixtureTearDown]
        public void ShutDownAtEnd()
        {
            Console.WriteLine( "shutting down" );

            ( ( RemoteDistributedHashTableServer ) server )._StopAndWaitDown( 4000 );
        }

        /// <summary>
        /// Set of constants to fiddle with
        /// </summary>
        public class Constants
        {
            public static bool BOOL1 = false;

            ///
            public static bool BOOL2 = true;

            ///
            public static sbyte BYTE1 = -128;

            ///
            public static sbyte BYTE2 = -1;

            ///
            public static sbyte BYTE3 = 0;

            ///
            public static sbyte BYTE4 = 1;

            ///
            public static sbyte BYTE5 = 127;

            ///
            public static short SHORT1 = -32768;

            ///
            public static short SHORT2 = -1;

            ///
            public static short SHORT3 = 0;

            ///
            public static short SHORT4 = 1;

            ///
            public static short SHORT5 = 32767;

            ///
            public static int INT1 = -2147483648;

            ///
            public static int INT2 = -1;

            ///
            public static int INT3 = 0;

            ///
            public static int INT4 = 1;

            ///
            public static int INT5 = 2147483647;

            ///
            public static long LONG1 = -9223372036854775808L;

            ///
            public static long LONG2 = -1L;

            ///
            public static long LONG3 = 0L;

            ///
            public static long LONG4 = 1L;

            ///
            public static long LONG5 = 9223372036854775807L;

            ///
            public static float FLOAT1 = 1.4e-45F;

            ///
            public static float FLOAT2 = -1F;

            ///
            public static float FLOAT3 = 0F;

            ///
            public static float FLOAT4 = 1F;

            ///
            public static float FLOAT5 = 3.4028235e38F;

            ///
            public static double DOUBLE1 = 4.9e-324D;

            ///
            public static double DOUBLE2 = -1D;

            ///
            public static double DOUBLE3 = 0D;

            ///
            public static double DOUBLE4 = 1D;

            ///
            public static double DOUBLE5 = 1.7976931348623157e308D;

            ///
            public static string STRING1 = "";

            ///
            public static string STRING2 = "a";

            ///
            public static string STRING3 = "abc";

            ///
            public static string STRING4 = "a\tb\tc\r\n";

            ///
            public static string STRING5 = "x x";

            ///
            public static string STRING6 = "x\u00bfx";

            ///
            public static string STRING7 = "x\u03a9x";

            ///
            public static string STRING8 = "x\\x";

            ///
            public static string STRING9 = "x'x";

            ///
            public static string STRING10 = "x\"x";
        }
    }
}

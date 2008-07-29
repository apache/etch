using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Etch.Msg;
using Etch.Support;
using Etch.Transport;
using System.Net.Sockets;
using System.Runtime.CompilerServices;
using System.Threading;


namespace Etch.Transport.Test
{
    /// <summary>
    /// 
    /// This file performs interop tests between data types
    /// in C# and Java ( including multidimensional arrays )
    /// 
    /// Instructions: 
    /// --------------
    /// 
    /// For Java -> C# test, first start the C# listener by
    /// running the main method of the test file with the same name.
    /// Thereafter, run unit tests on the Java side.
    /// 
    /// For the C# -> Java test, first start the Java listener and
    /// then run the tests on the C# side.
    /// 
    /// </summary>
    /// 
    [TestFixture]
    public class TestInterop
    {
        private MailboxManager mmgr;
        private MyMessageHandler mh;

        private static int testCount = 0;
        private static bool DEBUG = false;

        //private XType[] mtcmd;
        //private int[] cmd;

        //private Field[] mk;
        //private int k1 = 1, k2 = 2;

        private MyValueFactory vf;

        private MyListenerHandler lh;
        private Listener l;

        private string host = "127.0.0.1";
        private int port = 4001;
        private int listenerPort = 4001;
        private int delay = 0;

        private bool CSHARPTOJAVA = true;

        TcpConnection c;

        //private Converter converter = null;

        private static int MAXFIELDS = 100;
        private Object[] verifier = new Object[ MAXFIELDS ];

        public void StartListener()
        {
            lh = new MyListenerHandler( this );
            l =  new Listener( lh, 5, host, listenerPort, 0 );
            l.Start();
            l.WaitUp( 4000 );
        }

        /// <summary>
        /// Populates the valuefactory so that both ends have parity. 
        /// </summary>
        public void PopulateValueFactory()
        {
            vf = new MyValueFactory();
        }

        public void SetUp( Socket s )
        {
            try
            {
                PopulateValueFactory();

                mh = new MyMessageHandler( this );
                mmgr = new MailboxManager( mh, 0 );

                Messagizer m = new Messagizer( mmgr, vf );
                mmgr.SetSource( m );

                Packetizer p = new Packetizer( m );
                m.SetSource( p );

                if ( CSHARPTOJAVA )
                    c = new TcpConnection( p, host, port, delay );
                else
                    c = new TcpConnection( p, s );

                p.SetSource( c );

                c.Start();
                c.WaitUp( 4000 );
            }
            catch ( Exception e )
            {
                if ( e is Exception )
                {
                    Console.WriteLine( e.ToString() );
                    Console.WriteLine( " -------------------------------------- " );
                    Console.WriteLine( " The Java listener is not UP " );
                    Console.WriteLine( " No further Interop tests will be run " );
                    Console.WriteLine( " -------------------------------------- " );
                    Environment.Exit( 0 );
                }
            }
        }

        [TestFixtureSetUp]
        public void TestFixtureSetUp()
        {
            SetUp( null );
        }

        [Test]
        public void TestMessageBool()
        {

            Message message = new Message( MyValueFactory.mtcmd[ 0 ], vf );
            bool no1 = true;
            bool no2 = false;

            message.Add( MyValueFactory.mk[ 0 ], no1 );
            message.Add( MyValueFactory.mk[ 1 ], no2 );

            // Set the object to be verified against.
            verifier[ 0 ] = no1;
            verifier[ 1 ] = no2;

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            System.Threading.Thread.Sleep( 500 );

        }

        [Test]
        public void TestMessageBool2D()
        {
            Message message = new Message( MyValueFactory.mtcmd[ 2 ], vf );

            bool[][] no = new bool[][] { new bool[] { false, true }, new bool[] { true, false } };

            // Set the object to be verified against.
            verifier[ 0 ] = no;


            message.Add( MyValueFactory.mk[ 3 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            // Sleep required so that next "test" method doesn't interfere
            System.Threading.Thread.Sleep( 500 );
            //c.Close();
        }

        [Test]
        public void TestMessageBool3D()
        {
            Message message = new Message( MyValueFactory.mtcmd[ 1 ], vf );

            bool[][][] no = new bool[][][] {
                    new bool[][] { new bool[] { true, false, false }, new bool[] { true, false, true } },
                    new bool[][] { new bool[] { false, true, false }, new bool[] { false, true, false } },
                    new bool[][] { new bool[] { false, false, false }, new bool[] { true, true, true } }
                };

            // Set the object to be verified against.
            verifier[ 0 ] = no;


            message.Add( MyValueFactory.mk[ 2 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            // Sleep required so that next "test" method doesn't interfere
            System.Threading.Thread.Sleep( 500 );
            //c.Close();
        }


        [Test]
        public void TestMessageString()
        {
            //StartListener();
            //SetUp( null );

            Message message = new Message( MyValueFactory.mtcmd[ 3 ], vf );
            String no1 = "hello java";
            String no2 = "hello world";
            message.Add( MyValueFactory.mk[ 4 ], no1 );
            message.Add( MyValueFactory.mk[ 5 ], no2 );

            verifier[ 0 ] = no1;
            verifier[ 1 ] = no2;

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            System.Threading.Thread.Sleep( 500 );

        }

        [Test]
        public void TestMessageStringArray2D()
        {
            //SetUp( null );

            Message message = new Message( MyValueFactory.mtcmd[ 4 ], vf );

            String[][] no = new String[][] { new String[] { "", "a", "ab", "abc" }, new String[] { "23" } };


            // Set the object to be verified against.
            verifier[ 0 ] = no;

            message.Add( MyValueFactory.mk[ 6 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );


            // Sleep required so that next "test" method doesn't interfere
            System.Threading.Thread.Sleep( 500 );
            //c.Close();
        }

        [Test]
        public void TestMessageStringArray3D()
        {
            Message message = new Message( MyValueFactory.mtcmd[ 5 ], vf );

            String[][][] no = 
                new String[][][]
                {
                    new String[][] { new String[] { "", "a", "ab", "abc" }, new String[] { "23" } },
                    new String[][] { new String[] { "abc", "ab", "a", "" }, new String[] { "23" } },
                    new String[][] { new String[] { "ab", "abc", "", "a" }, new String[] { "23" } }
                };

            // Set the object to be verified against.
            verifier[ 0 ] = no;

            message.Add( MyValueFactory.mk[ 7 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            // Sleep required so that next "test" method doesn't interfere
            System.Threading.Thread.Sleep( 500 );
            //c.Close();

        }


        [Test]
        public void TestMessageByte()
        {
            //StartListener();
            //SetUp( null );

            Message message = new Message( MyValueFactory.mtcmd[ 6 ], vf );
            sbyte no1 = sbyte.MaxValue;
            sbyte no2 = sbyte.MinValue;
            message.Add( MyValueFactory.mk[ 8 ], no1 );
            message.Add( MyValueFactory.mk[ 9 ], no2 );

            verifier[ 0 ] = no1;
            verifier[ 1 ] = no2;

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            System.Threading.Thread.Sleep( 1500 );

        }

        [Test]
        public void TestMessageByteArray2D()
        {
            Message message = new Message( MyValueFactory.mtcmd[ 7 ], vf );

            sbyte[][] no = 
                new sbyte[][] { new sbyte[] { SByte.MinValue, -1, 0, 1, SByte.MaxValue }, new sbyte[] { 23 } };

            // Set the object to be verified against.
            verifier[ 0 ] = no;

            message.Add( MyValueFactory.mk[ 10 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            // Sleep required so that next "test" method doesn't interfere
            System.Threading.Thread.Sleep( 500 );
            //c.Close();

        }

        [Test]
        public void TestMessageByteArray3D()
        {
            Message message = new Message( MyValueFactory.mtcmd[ 8 ], vf );

            sbyte[][][] no = 
                new sbyte[][][] 
                {
                    new sbyte[][] { new sbyte[] { SByte.MinValue, -1, 0, 1, SByte.MaxValue }, new sbyte[] { 23 } },
                    new sbyte[][] { new sbyte[] { SByte.MinValue, 1, 0, -1, SByte.MaxValue }, new sbyte[] { 23 } },
                    new sbyte[][] { new sbyte[] { SByte.MaxValue, -1, 0, 1, SByte.MinValue }, new sbyte[] { 23 } }
                };

            // Set the object to be verified against.
            verifier[ 0 ] = no;

            message.Add( MyValueFactory.mk[ 11 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            // Sleep required so that next "test" method doesn't interfere
            System.Threading.Thread.Sleep( 500 );
            //c.Close();

        }

        [Test]
        public void TestMessageShort()
        {
            //StartListener();
            //SetUp( null );

            Message message = new Message( MyValueFactory.mtcmd[ 9 ], vf );
            short no1 = short.MaxValue;
            short no2 = short.MinValue;
            message.Add( MyValueFactory.mk[ 12 ], no1 );
            message.Add( MyValueFactory.mk[ 13 ], no2 );

            verifier[ 0 ] = no1;
            verifier[ 1 ] = no2;

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            System.Threading.Thread.Sleep( 500 );

        }

        [Test]
        public void TestMessageShortArray2D()
        {
            Message message = new Message( MyValueFactory.mtcmd[ 10 ], vf );

            short[][] no = new short[][] { new short[] { short.MinValue, SByte.MinValue, -1, 0, 1,
			    SByte.MaxValue, short.MaxValue }, new short[] { 23 } };


            // Set the object to be verified against.
            verifier[ 0 ] = no;

            message.Add( MyValueFactory.mk[ 14 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );


            // Sleep required so that next "test" method doesn't interfere
            System.Threading.Thread.Sleep( 500 );
        }

        [Test]
        public void TestMessageShortArray3D()
        {
            //SetUp( null );
            Message message = new Message( MyValueFactory.mtcmd[ 11 ], vf );

            short[][][] no = new short[][][] 
                            {
                                new short[][] { new short[] { short.MinValue, SByte.MinValue, -1, 0, 1,
			                    SByte.MaxValue, short.MaxValue }, new short[] { 23 } },
                                new short[][] { new short[] { short.MaxValue, SByte.MinValue, -1, 0, 1,
			                    SByte.MaxValue, short.MinValue }, new short[] { 23 } },
                                new short[][] { new short[] { short.MaxValue, SByte.MaxValue, 1, 0, -1,
			                    SByte.MinValue, short.MinValue }, new short[] { 23 } }
                            };


            // Set array to be verified against
            verifier[ 0 ] = no;

            message.Add( MyValueFactory.mk[ 15 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            System.Threading.Thread.Sleep( 500 );
        }

        [Test]
        public void TestMessageLong()
        {
            Message message = new Message( MyValueFactory.mtcmd[ 12 ], vf );
            long no1 = long.MaxValue;
            long no2 = long.MinValue;
            message.Add( MyValueFactory.mk[ 16 ], no1 );
            message.Add( MyValueFactory.mk[ 17 ], no2 );

            verifier[ 0 ] = no1;
            verifier[ 1 ] = no2;

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            System.Threading.Thread.Sleep( 500 );

        }

        [Test]
        public void TestMessageLongArray2D()
        {
            Message message = new Message( MyValueFactory.mtcmd[ 13 ], vf );

            long[][] no = new long[][] { new long[] { long.MinValue, int.MinValue, short.MinValue,
			    SByte.MinValue, -1, 0, 1, sbyte.MaxValue, short.MaxValue,
			    int.MaxValue, long.MaxValue}, new long[] { 23 } };


            // Set the object to be verified against.
            verifier[ 0 ] = no;

            message.Add( MyValueFactory.mk[ 18 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );


            // Sleep required so that next "test" method doesn't interfere
            System.Threading.Thread.Sleep( 500 );
            //c.Close();
        }

        [Test]
        public void TestMessageLongArray3D()
        {

            Message message = new Message( MyValueFactory.mtcmd[ 14 ], vf );

            long[][][] no = new long[][][]
                            {
                                new long[][] { new long[] { long.MinValue, int.MinValue, short.MinValue,
			                    SByte.MinValue, -1, 0, 1, sbyte.MaxValue, short.MaxValue,
			                    int.MaxValue, long.MaxValue}, new long[] { 23 } },
                                new long[][] { new long[] { long.MinValue, int.MaxValue, short.MaxValue,
			                    SByte.MinValue, -1, 0, 1, sbyte.MaxValue, short.MinValue,
			                    int.MinValue, long.MaxValue}, new long[] { 23 } },
                                new long[][] { new long[] { long.MaxValue, int.MaxValue, short.MaxValue,
			                    SByte.MaxValue, -1, 0, 1, sbyte.MinValue, short.MinValue,
			                    int.MinValue, long.MinValue}, new long[] { 23 } }
                            };


            // Set array to be verified against
            verifier[ 0 ] = no;

            message.Add( MyValueFactory.mk[ 19 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            System.Threading.Thread.Sleep( 500 );
        }



        [Test]
        public void TestMessageDouble()
        {
            Message message = new Message( MyValueFactory.mtcmd[ 15 ], vf );
            double no1 = 2.4;
            double no2 = -2.4;
            message.Add( MyValueFactory.mk[ 20 ], no1 );
            message.Add( MyValueFactory.mk[ 21 ], no2 );

            verifier[ 0 ] = no1;
            verifier[ 1 ] = no2;

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            Thread.Sleep( 1000 );
            //c.Close();
        }

        [Test]
        public void TestMessageDoubleArray2D()
        {

            Message message = new Message( MyValueFactory.mtcmd[ 16 ], vf );

            double[][] no = new double[][] { new double[]{ -1, 0, 1, double.MinValue, double.Epsilon,
			    double.MaxValue, double.NaN, double.NegativeInfinity,
			    double.PositiveInfinity, -0.0f, 1.1f,
			    3.141592653589793238462643383279 }, new double[]{ 23 } };


            // Set the object to be verified against.
            verifier[ 0 ] = no;

            message.Add( MyValueFactory.mk[ 22 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );


            // Sleep required so that next "test" method doesn't interfere
            System.Threading.Thread.Sleep( 500 );
            //c.Close();
        }

        [Test]
        public void TestMessageDoubleArray3D()
        {

            Message message = new Message( MyValueFactory.mtcmd[ 17 ], vf );

            double[][][] no = new double[][][]
                {
                    new double[][] { new double[]{ -1, 0, 1, double.MinValue, double.Epsilon,
			        double.MaxValue, double.NaN, double.NegativeInfinity,
			        double.PositiveInfinity, -0.0f, 1.1f,
			        3.141592653589793238462643383279 }, new double[]{ 23 } },

                    new double[][] { new double[]{ -1, 0, 1, double.MaxValue, double.Epsilon,
			        double.MinValue, double.NaN, double.NegativeInfinity,
			        double.PositiveInfinity, 0.0f, -1.1f,
			        3.141592653589793238462643383279 }, new double[]{ 23 } },

                    new double[][] { new double[]{ -1, 0, 1, double.MaxValue, double.Epsilon,
			        double.MinValue, double.NaN, double.PositiveInfinity,
			        double.NegativeInfinity, 0.0f, -1.1f,
			        -3.141592653589793238462643383279 }, new double[]{ 23 } }
                };


            // Set the object to be verified against.
            verifier[ 0 ] = no;

            message.Add( MyValueFactory.mk[ 23 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );


            // Sleep required so that next "test" method doesn't interfere
            System.Threading.Thread.Sleep( 500 );
            //c.Close();
        }

        [Test]
        public void TestMessageFloat()
        {

            Message message = new Message( MyValueFactory.mtcmd[ 18 ], vf );
            float no1 = 1.2f;
            float no2 = -1.2f;
            message.Add( MyValueFactory.mk[ 24 ], no1 );
            message.Add( MyValueFactory.mk[ 25 ], no2 );

            verifier[ 0 ] = no1;
            verifier[ 1 ] = no2;

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            Thread.Sleep( 1000 );
            //c.Close();
        }

        [Test]
        public void TestMessageFloatArray2D()
        {

            Message message = new Message( MyValueFactory.mtcmd[ 19 ], vf );

            float[][] no = new float[][] { new float[] { -1, 0, 1, float.MinValue, float.Epsilon,
			    float.MaxValue, float.NaN, float.NegativeInfinity,
			    float.PositiveInfinity, -0.0f, 1.1f,
			    3.141592653589793238462643383279f }, new float[] { 23 } };


            // Set the object to be verified against.
            verifier[ 0 ] = no;

            message.Add( MyValueFactory.mk[ 26 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );


            // Sleep required so that next "test" method doesn't interfere
            System.Threading.Thread.Sleep( 500 );
            //c.Close();
        }

        [Test]
        public void TestMessageFloatArray3D()
        {

            Message message = new Message( MyValueFactory.mtcmd[ 20 ], vf );
            //int[] no = new int[] { int.MinValue, short.MinValue, SByte.MinValue,
            //    -1, 0, 1, sbyte.MaxValue, short.MaxValue, int.MaxValue};

            float[][][] no = new float[][][]
                {
                    new float[][] { new float[] { -1, 0, 1, float.MinValue, float.Epsilon,
			        float.MaxValue, float.NaN, float.NegativeInfinity,
			        float.PositiveInfinity, -0.0f, 1.1f,
			        3.141592653589793238462643383279f }, new float[] { 23 } },

                    new float[][] { new float[] { -1, 0, 1, float.MaxValue, float.Epsilon,
			        float.MinValue, float.NaN, float.NegativeInfinity,
			        float.PositiveInfinity, 0.0f, -1.1f,
			        3.141592653589793238462643383279f }, new float[] { 23 } },

                    new float[][] { new float[] { -1, 0, 1, float.MaxValue, float.Epsilon,
			        float.MaxValue, float.NaN, float.PositiveInfinity,
			        float.NegativeInfinity, -0.0f, 1.1f,
			        -3.141592653589793238462643383279f }, new float[] { 23 } }
                };


            // Set the object to be verified against.
            verifier[ 0 ] = no;

            message.Add( MyValueFactory.mk[ 27 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );


            // Sleep required so that next "test" method doesn't interfere
            System.Threading.Thread.Sleep( 500 );
            //c.Close();
        }

        [Test]
        public void TestMessageInt()
        {
            Message message = new Message( MyValueFactory.mtcmd[ 21 ], vf );
            int no1 = 12345;
            int no2 = -no1;
            message.Add( MyValueFactory.mk[ 28 ], no1 );
            message.Add( MyValueFactory.mk[ 29 ], no2 );

            verifier[ 0 ] = no1;
            verifier[ 1 ] = no2;

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            System.Threading.Thread.Sleep( 500 );
            //c.Close();
        }


        [Test]
        public void TestMessageIntArray2D()
        {

            Message message = new Message( MyValueFactory.mtcmd[ 22 ], vf );

            int[][] no = new int[][] { new int[] { int.MinValue, short.MinValue, SByte.MinValue,
                -1, 0, 1, sbyte.MaxValue, short.MaxValue, int.MaxValue}, new int[] { 23 } };


            // Set the object to be verified against.
            verifier[ 0 ] = no;

            message.Add( MyValueFactory.mk[ 30 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );


            // Sleep required so that next "test" method doesn't interfere
            System.Threading.Thread.Sleep( 500 );
            //c.Close();
        }

        [Test]
        public void TestMessageIntArray3D()
        {
            //SetUp( null );
            Message message = new Message( MyValueFactory.mtcmd[ 23 ], vf );

            int[][][] no = new int[][][]
                            {
                                new int[][] { new int[] { int.MinValue, short.MinValue, SByte.MinValue,
		                        -1, 0, 1, sbyte.MaxValue, short.MaxValue, int.MaxValue}, new int[] { 23 } },
                                new int[][] { new int[] { int.MaxValue, short.MinValue, SByte.MinValue,
		                        -1, 0, 1, sbyte.MaxValue, short.MinValue, int.MaxValue}, new int[] { 23 } },
                                new int[][] { new int[] { int.MaxValue, short.MaxValue, SByte.MaxValue,
		                        -1, 0, 1, sbyte.MinValue, short.MinValue, int.MinValue}, new int[] { 23 } }
                            };


            // Set array to be verified against
            verifier[ 0 ] = no;

            message.Add( MyValueFactory.mk[ 31 ], no );

            Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, message, 1, 0 );

            System.Threading.Thread.Sleep( 500 );
            //c.Close();
        }

        //[Test]
        //public void TestMessageObject()
        //{
        //    Message message = new Message( MyValueFactory.mtcmd[ 24 ], vf );
        //    Object x = new Dummy();

        //    verifier[ 0 ] = x;

        //    message.Add( MyValueFactory.mk[ 32 ], x );

        //    Mailbox mb = 
        //        ( Mailbox ) mmgr.Message( null, message, 1, 0 );

        //    System.Threading.Thread.Sleep( 500 );
        //}

        public class Dummy
        {
            //private int x;
            //private bool y;
            //public Dummy()
            //{
            //    x = 0;
            //    y = true;
            //}
        }

        [TestFixtureTearDown]
        public void TearDown()
        {
            c.Close();
            c.WaitDown( 4000 );
        }


        public enum What
        {
            MMGRMESSAGEHANDLERSTARTED,
            MMGRMESSAGEHANDLERUP,
            MMGRMESSAGEHANDLERDown,
            MMGRMESSAGEHANDLERSTOPPED,
            MYMESSAGIZERClose,
            MYMESSAGIZERLocalAddress,
            MYMESSAGIZERRemoteAddress,
            MYMESSAGIZERShutdownInput,
            MYMESSAGIZERShutdownOutput,
            MYMESSAGIZERSTOP,
            MYMESSAGIZERMESSAGE,
            MMGRMESSAGEHANDLERMESSAGE
        };

        /// <summary>
        /// Dummy message handler
        /// </summary>
        public class MyMessageHandler : Etch.Transport.MessageHandler<MailboxMessageSource>
        {
            public Enum what;
            public TestInterop _tds;

            public MyMessageHandler()
            {
                // nothing to do.
            }

            public MyMessageHandler( TestInterop tds )
            {
                _tds = tds;
            }

            //public bool Message( MailboxMessageSource src, Who sender, Message msg )
            //{
            //    if ( DEBUG )
            //        Console.WriteLine( "Message received" + msg.ToString() );
            //    _tds.Verify( msg );
            //    return true;
            //}

            public void Exception( MailboxMessageSource src, String what, Exception e )
            {
                Console.WriteLine( "myMessageHandler.exception() called ... " );
            }

            #region SourceHandler<MailboxMessageSource> Members

            public MailboxMessageSource GetSource()
            {
                return null;
            }

            public void SetSource( MailboxMessageSource src )
            {
                // ignore
            }

            #endregion

            #region Session Members

            public object SessionQuery( object query )
            {
                return null;
            }

            public void SessionControl( object control, object value )
            {
                // ignore
            }

            public void SessionNotify( object eventObj )
            {
                // ignore
            }

            #endregion

            #region MessageHandler Members

            public bool Message( Who sender, Message msg )
            {
                if ( DEBUG )
                    Console.WriteLine( "Message received" + msg.ToString() );
                _tds.Verify( msg );
                return true;
            }

            #endregion
        }

        /// <summary>
        /// Dummy Listener handler.
        /// </summary>
        public class MyListenerHandler : ListenerHandler
        {

            private TestInterop _tds;
            public MyListenerHandler( TestInterop tds )
            {
                _tds = tds;
            }

            public void Accepted( System.Net.Sockets.Socket s )
            {
                Console.WriteLine( "Connection accepted ...." );

                // only if sending data from java to csharp, 
                // receive...
                if ( !_tds.CSHARPTOJAVA )
                {
                    _tds.SetUp( s );
                }
            }

            public void Down( DataSource src )
            {
                Console.WriteLine( "Connection down ... " );
            }

            public void Up( DataSource src )
            {
                Console.WriteLine( "Connection up ... " );
            }

            #region SourceHandler<Source> Members

            public Source GetSource()
            {
                throw new Exception( "The method or operation is not implemented." );
            }

            public void SetSource( Source src )
            {
                throw new Exception( "The method or operation is not implemented." );
            }

            #endregion

            #region Session Members

            public object SessionQuery( object query )
            {
                // ignore
                return null;
            }

            public void SessionControl( object control, object value )
            {
                // ignore
            }

            public void SessionNotify( object eventObj )
            {
                // ignore
            }

            #endregion
        }

        /// <summary>
        /// Run the main function if transferring data from Java to C#.
        /// </summary>
        /// <param name="args"></param>
        public static void Main( string[] args )
        {
            // Just start the listener
            TestInterop inst = new TestInterop();
            inst.CSHARPTOJAVA = false;
            inst.StartListener();

        }

        private static void AssertEquals( Object a, Object b )
        {

            if ( a == null || b == null )
            {
                Assert.AreEqual( a, b );
                return;
            }

            Type aType = a.GetType();
            Type bType = b.GetType();

            if ( aType == bType )
            {
                if ( ( a is Array ) && ( b is Array ) )
                    AssertArrayEquals( a, b );
                else
                    Assert.AreEqual( a, b );
            }
            //else if (a instanceof Number && b instanceof Number)
            else
            {
                //Console.WriteLine( "a {0}, b {1}\n", aType, bType );
                Assert.AreEqual( a, b );
            }
        }

        private static void AssertArrayEquals( Object a, Object b )
        {
            // jagged arrays ... one dimension
            int an = ( ( Array ) a ).GetLength( 0 );
            int bn = ( ( Array ) b ).GetLength( 0 );

            AssertEquals( an, bn );

            for ( int i = 0; i < an; i++ )
                AssertEquals( ( ( Array ) a ).GetValue( 0 ), ( ( Array ) b ).GetValue( 0 ) );


        }

        [MethodImpl( MethodImplOptions.Synchronized )]
        public void Verify( Message msg )
        {
            int i = 0;
            bool nullMessage = true;

            Dictionary<Field, Object>.KeyCollection keys = msg.Keys;
            foreach ( Field f in keys )
            {
                if ( DEBUG )
                {
                    Console.WriteLine( "---------------------------------------------------" );
                    Console.WriteLine( "Message type = " + msg.GetXType );
                    Console.WriteLine( "Field = " + f );
                    Console.Write( "\t Value = " + msg[ f ] );
                    Console.WriteLine( "\t Type = " + ( msg[ f ].GetType() ) );
                }
                if ( !f.Equals( new Field( "_messageId" ) ) )
                {
                    //AssertEquals(no, msg[f]);

                    if ( CSHARPTOJAVA )
                        AssertEquals( verifier[ i ], msg[ f ] );

                    nullMessage = false;
                    i++;
                }

                if ( nullMessage == true )
                    Console.WriteLine( "Null Message !" );
            }

            if ( CSHARPTOJAVA )
            {
                if ( DEBUG )
                {
                    Console.WriteLine();
                    Console.WriteLine( "----------------------" );
                    Console.WriteLine( "Verified test ... " );
                    Console.WriteLine( "----------------------" );
                }
                testCount++;
                if ( testCount == 24 )
                    Console.WriteLine( " All interop tests pass " );
            }
            else
            {

                Mailbox mb = 
                ( Mailbox ) mmgr.Message( null, msg, 1, 0 );
            }

        }


        public class MyValueFactory : DefaultValueFactory
        {
            private readonly static TypeMap types = new TypeMap();

            private readonly static FieldMap fields = new FieldMap();

            private readonly static Dictionary<Type, XType> class2type = 
                new Dictionary<Type, XType>();

            #region initializers

            private static readonly int NUMCMDS = 30;

            public static XType[] mtcmd = new XType[ NUMCMDS ];

            //public static int[] cmd = new int[ NUMCMDS ];

            // rought estimate of fields
            public static Field[] mk = new Field[ NUMCMDS * 2 ];

            public static int k1 = 1, k2 = 2;
            
            #endregion


            static MyValueFactory()
            {
                DefaultValueFactory.Init( types, fields, class2type );

                mtcmd[ 0 ] = types.Get( "cmd0" );
                mk[ 0 ] = fields.Get( "k1" );
                mk[ 1 ] = fields.Get( "k2" );

                mtcmd[ 0 ].PutValidator( mk[ 0 ], Validator_boolean.Get( 0 ) );
                mtcmd[ 0 ].PutValidator( mk[ 1 ], Validator_boolean.Get( 0 ) );
                mtcmd[ 0 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Boolean 3D
                mtcmd[ 1 ] = types.Get( "cmd1" );
                mk[ 2 ] = fields.Get( "k1" );

                mtcmd[ 1 ].PutValidator( mk[ 2 ], Validator_boolean.Get( 3 ) );
                mtcmd[ 1 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Boolean 2D
                mtcmd[ 2 ] = types.Get( "cmd2" );
                mk[ 3 ] = fields.Get( "k1" );

                mtcmd[ 2 ].PutValidator( mk[ 3 ], Validator_boolean.Get( 2 ) );
                mtcmd[ 2 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // String
                mtcmd[ 3 ] = types.Get( "cmd3" );
                mk[ 4 ] = fields.Get( "k1" );
                mk[ 5 ] = fields.Get( "k2" );

                mtcmd[ 3 ].PutValidator( mk[ 4 ], Validator_string.Get( 0 ) );
                mtcmd[ 3 ].PutValidator( mk[ 5 ], Validator_string.Get( 0 ) );
                mtcmd[ 3 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // String 2D
                mtcmd[ 4 ] = types.Get( "cmd4" );
                mk[ 6 ] = fields.Get( "k1" );

                mtcmd[ 4 ].PutValidator( mk[ 6 ], Validator_string.Get( 2 ) );
                mtcmd[ 4 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // String 3D
                mtcmd[ 5 ] = types.Get( "cmd5" );
                mk[ 7 ] = fields.Get( "k1" );

                mtcmd[ 5 ].PutValidator( mk[ 7 ], Validator_string.Get( 3 ) );
                mtcmd[ 5 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Byte
                mtcmd[ 6 ] = types.Get( "cmd6" );
                mk[ 8 ] = fields.Get( "k1" );
                mk[ 9 ] = fields.Get( "k2" );

                mtcmd[ 6 ].PutValidator( mk[ 8 ], Validator_byte.Get( 0 ) );
                mtcmd[ 6 ].PutValidator( mk[ 9 ], Validator_byte.Get( 0 ) );
                mtcmd[ 6 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Byte 2D
                mtcmd[ 7 ] = types.Get( "cmd7" );
                mk[ 10 ] = fields.Get( "k1" );

                mtcmd[ 7 ].PutValidator( mk[ 10 ], Validator_byte.Get( 2 ) );
                mtcmd[ 7 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Byte 3D
                mtcmd[ 8 ] = types.Get( "cmd8" );
                mk[ 11 ] = fields.Get( "k1" );

                mtcmd[ 8 ].PutValidator( mk[ 11 ], Validator_byte.Get( 3 ) );
                mtcmd[ 8 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Short
                mtcmd[ 9 ] = types.Get( "cmd9" );
                mk[ 12 ] = fields.Get( "k1" );
                mk[ 13 ] = fields.Get( "k2" );

                mtcmd[ 9 ].PutValidator( mk[ 12 ], Validator_short.Get( 0 ) );
                mtcmd[ 9 ].PutValidator( mk[ 13 ], Validator_short.Get( 0 ) );
                mtcmd[ 9 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Short 2D
                mtcmd[ 10 ] = types.Get( "cmd10" );
                mk[ 14 ] = fields.Get( "k1" );

                mtcmd[ 10 ].PutValidator( mk[ 14 ], Validator_short.Get( 2 ) );
                mtcmd[ 10 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Short 3D
                mtcmd[ 11 ] = types.Get( "cmd11" );
                mk[ 15 ] = fields.Get( "k1" );

                mtcmd[ 11 ].PutValidator( mk[ 15 ], Validator_short.Get( 3 ) );
                mtcmd[ 11 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Long
                mtcmd[ 12 ] = types.Get( "cmd12" );
                mk[ 16 ] = fields.Get( "k1" );
                mk[ 17 ] = fields.Get( "k2" );

                mtcmd[ 12 ].PutValidator( mk[ 16 ], Validator_long.Get( 0 ) );
                mtcmd[ 12 ].PutValidator( mk[ 17 ], Validator_long.Get( 0 ) );
                mtcmd[ 12 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Long 2D
                mtcmd[ 13 ] = types.Get( "cmd13" );
                mk[ 18 ] = fields.Get( "k1" );

                mtcmd[ 13 ].PutValidator( mk[ 18 ], Validator_long.Get( 2 ) );
                mtcmd[ 13 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Long 3D
                mtcmd[ 14 ] = types.Get( "cmd14" );
                mk[ 19 ] = fields.Get( "k1" );

                mtcmd[ 14 ].PutValidator( mk[ 19 ], Validator_long.Get( 3 ) );
                mtcmd[ 14 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Double
                mtcmd[ 15 ] = types.Get( "cmd15" );
                mk[ 20 ] = fields.Get( "k1" );
                mk[ 21 ] = fields.Get( "k2" );

                mtcmd[ 15 ].PutValidator( mk[ 20 ], Validator_double.Get( 0 ) );
                mtcmd[ 15 ].PutValidator( mk[ 21 ], Validator_double.Get( 0 ) );
                mtcmd[ 15 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Double 2D
                mtcmd[ 16 ] = types.Get( "cmd16" );
                mk[ 22 ] = fields.Get( "k1" );

                mtcmd[ 16 ].PutValidator( mk[ 22 ], Validator_double.Get( 2 ) );
                mtcmd[ 16 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Double 3D
                mtcmd[ 17 ] = types.Get( "cmd17" );
                mk[ 23 ] = fields.Get( "k1" );

                mtcmd[ 17 ].PutValidator( mk[ 23 ], Validator_double.Get( 3 ) );
                mtcmd[ 17 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Float
                mtcmd[ 18 ] = types.Get( "cmd18" );
                mk[ 24 ] = fields.Get( "k1" );
                mk[ 25 ] = fields.Get( "k2" );

                mtcmd[ 18 ].PutValidator( mk[ 24 ], Validator_float.Get( 0 ) );
                mtcmd[ 18 ].PutValidator( mk[ 25 ], Validator_float.Get( 0 ) );
                mtcmd[ 18 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Float 2D
                mtcmd[ 19 ] = types.Get( "cmd19" );
                mk[ 26 ] = fields.Get( "k1" );

                mtcmd[ 19 ].PutValidator( mk[ 26 ], Validator_float.Get( 2 ) );
                mtcmd[ 19 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Float 3D
                mtcmd[ 20 ] = types.Get( "cmd20" );
                mk[ 27 ] = fields.Get( "k1" );

                mtcmd[ 20 ].PutValidator( mk[ 27 ], Validator_float.Get( 3 ) );
                mtcmd[ 20 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Int
                mtcmd[ 21 ] = types.Get( "cmd21" );
                mk[ 28 ] = fields.Get( "k1" );
                mk[ 29 ] = fields.Get( "k2" );

                mtcmd[ 21 ].PutValidator( mk[ 28 ], Validator_int.Get( 0 ) );
                mtcmd[ 21 ].PutValidator( mk[ 29 ], Validator_int.Get( 0 ) );
                mtcmd[ 21 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Int 2D
                mtcmd[ 22 ] = types.Get( "cmd22" );
                mk[ 30 ] = fields.Get( "k1" );

                mtcmd[ 22 ].PutValidator( mk[ 30 ], Validator_int.Get( 2 ) );
                mtcmd[ 22 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Int 3D
                mtcmd[ 23 ] = types.Get( "cmd23" );
                mk[ 31 ] = fields.Get( "k1" );

                mtcmd[ 23 ].PutValidator( mk[ 31 ], Validator_int.Get( 3 ) );
                mtcmd[ 23 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

                // Object
                mtcmd[ 24 ] = types.Get( "cmd24" );
                mk[ 32 ] = fields.Get( "k1" );

                mtcmd[ 24 ].PutValidator( mk[ 32 ], Validator_object.Get( 0 ) );
                mtcmd[ 24 ].PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );

            }

            public override XType GetType( int? id )
            {
                return types.Get( id );
            }

            public override XType GetType( string name )
            {
                return types.Get( name );
            }

            public override ICollection<XType> GetTypes()
            {
                return types.Values();
            }

            public override Field GetField( int? id )
            {
                return fields.Get( id );
            }

            public override Field GetField( string name )
            {
                return fields.Get( name );
            }

            public override ICollection<Field> GetFields()
            {
                return fields.Values();
            }

            public override StructValue ExportCustomValue( Object value )
            {
                return ExportCustomValue( this, class2type, value );
            }

            public override XType GetCustomStructType( Type c )
            {
                return class2type[ c ];
            }
        }

    }

}

using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Etch.Transport.Fmt.Binary;
using Etch.Msg;
using Etch.Support;
using Etch.Transport;
using Etch.Util;
using Etch.Support.Test;

namespace Etch.Transport.Fmt.Test
{
    /// <summary>
    /// Test binary input and output.
    /// </summary>
    [TestFixture]
    public class TestBinaryTaggedDataInOut
    {
        MyValueFactory vf;
        private XType mt_foo;
        private Field mf_x;

        public TestBinaryTaggedDataInOut()
        {
            vf = new MyValueFactory();
            mt_foo = vf.GetType( "foo" );
            mf_x = vf.GetField( "x" );
        }
        
        [Test]
        public void Test_Check_Value() 
        {
            BinaryTaggedDataOutput btdo = new BinaryTaggedDataOutput( (FlexBuffer) null, null );

            // positive small sbytes:
            Assert.AreEqual( ( sbyte ) 0, btdo.CheckValue( ( sbyte ) 0 ) );
            Assert.AreEqual( ( sbyte ) 1, btdo.CheckValue( ( sbyte ) 1 ) );
            Assert.AreEqual( ( sbyte ) 31, btdo.CheckValue( ( sbyte ) 31 ) );

            // positive small shorts:
            Assert.AreEqual( ( sbyte ) 32, btdo.CheckValue( ( short ) 0 ) );
            Assert.AreEqual( ( sbyte ) 33, btdo.CheckValue( ( short ) 1 ) );
            Assert.AreEqual( ( sbyte ) 63, btdo.CheckValue( ( short ) 31 ) );

            // positive small ints:
            Assert.AreEqual( ( sbyte ) 64, btdo.CheckValue( 0 ) );
            Assert.AreEqual( ( sbyte ) 65, btdo.CheckValue( 1 ) );
            Assert.AreEqual( ( sbyte ) 95, btdo.CheckValue( 31 ) );

            // positive small longs:
            Assert.AreEqual( ( sbyte ) 96, btdo.CheckValue( 0L ) );
            Assert.AreEqual( ( sbyte ) 97, btdo.CheckValue( 1L ) );
            Assert.AreEqual( ( sbyte ) 127, btdo.CheckValue( 31L ) );

            // negative small sbytes:
            Assert.AreEqual( ( sbyte ) -128, btdo.CheckValue( ( sbyte ) -1 ) );
            Assert.AreEqual( ( sbyte ) -127, btdo.CheckValue( ( sbyte ) -2 ) );
            Assert.AreEqual( ( sbyte ) -113, btdo.CheckValue( ( sbyte ) -16 ) );

            // negative small shorts:
            Assert.AreEqual( ( sbyte ) -112, btdo.CheckValue( ( short ) -1 ) );
            Assert.AreEqual( ( sbyte ) -111, btdo.CheckValue( ( short ) -2 ) );
            Assert.AreEqual( ( sbyte ) -97, btdo.CheckValue( ( short ) -16 ) );

            // negative small ints:
            Assert.AreEqual( ( sbyte ) -96, btdo.CheckValue( -1 ) );
            Assert.AreEqual( ( sbyte ) -95, btdo.CheckValue( -2 ) );
            Assert.AreEqual( ( sbyte ) -81, btdo.CheckValue( -16 ) );

            // negative small longs:
            Assert.AreEqual( ( sbyte ) -80, btdo.CheckValue( -1L ) );
            Assert.AreEqual( ( sbyte ) -79, btdo.CheckValue( -2L ) );
            Assert.AreEqual( ( sbyte ) -65, btdo.CheckValue( -16L ) );

            // null value
            Assert.AreEqual( ( sbyte ) -1, btdo.CheckValue( null ) );

            // boolean values
            Assert.AreEqual( ( sbyte ) -2, btdo.CheckValue( false ) );
            Assert.AreEqual( ( sbyte ) -3, btdo.CheckValue( true ) );

            // normal sbytes
            Assert.AreEqual( ( sbyte ) -4, btdo.CheckValue( ( sbyte ) -45 ) );
            Assert.AreEqual( ( sbyte ) -4, btdo.CheckValue( ( sbyte ) 45 ) );

            // normal shorts
            Assert.AreEqual( ( sbyte ) -5, btdo.CheckValue( ( short ) -45 ) );
            Assert.AreEqual( ( sbyte ) -5, btdo.CheckValue( ( short ) 45 ) );

            Assert.AreEqual( ( sbyte ) -6, btdo.CheckValue( ( short ) -4545 ) );
            Assert.AreEqual( ( sbyte ) -6, btdo.CheckValue( ( short ) 4545 ) );

            // normal ints
            Assert.AreEqual( ( sbyte ) -7, btdo.CheckValue( -45 ) );
            Assert.AreEqual( ( sbyte ) -7, btdo.CheckValue( 45 ) );

            Assert.AreEqual( ( sbyte ) -8, btdo.CheckValue( -4545 ) );
            Assert.AreEqual( ( sbyte ) -8, btdo.CheckValue( 4545 ) );

            Assert.AreEqual( ( sbyte ) -9, btdo.CheckValue( -45454545 ) );
            Assert.AreEqual( ( sbyte ) -9, btdo.CheckValue( 45454545 ) );

            // normal longs
            Assert.AreEqual( ( sbyte ) -10, btdo.CheckValue( -45L ) );
            Assert.AreEqual( ( sbyte ) -10, btdo.CheckValue( 45L ) );

            Assert.AreEqual( ( sbyte ) -11, btdo.CheckValue( -4545L ) );
            Assert.AreEqual( ( sbyte ) -11, btdo.CheckValue( 4545L ) );

            Assert.AreEqual( ( sbyte ) -12, btdo.CheckValue( -45454545L ) );
            Assert.AreEqual( ( sbyte ) -12, btdo.CheckValue( 45454545L ) );

            Assert.AreEqual( ( sbyte ) -13, btdo.CheckValue( -4545454545454545L ) );
            Assert.AreEqual( ( sbyte ) -13, btdo.CheckValue( 4545454545454545L ) );

            // float
            Assert.AreEqual( ( sbyte ) -14, btdo.CheckValue( 3.14159f ) );

            // double
            Assert.AreEqual( ( sbyte ) -15, btdo.CheckValue( 3.14159 ) );

            // sbyte array
            Assert.AreEqual( ( sbyte ) -16, btdo.CheckValue( new sbyte[ 0 ] ) );
            Assert.AreEqual( ( sbyte ) -16, btdo.CheckValue( new sbyte[ 1 ] ) );

            // string
            Assert.AreEqual( ( sbyte ) -17, btdo.CheckValue( "" ) );
            Assert.AreEqual( ( sbyte ) -18, btdo.CheckValue( "abc" ) );

            // struct
            Assert.AreEqual( ( sbyte ) -19, btdo.CheckValue( new StructValue( new XType( "foo" ) ) ) );

            // custom
            Assert.AreEqual( ( sbyte ) -21, btdo.CheckValue( new DateTime() ) );

            // none
            Assert.AreEqual( ( sbyte ) -22, btdo.CheckValue( BinaryTaggedData.NONE ) );
        }

        [Test]
        public void Test_boolean() 
	    {
            // 2 dimensional
            bool[][] array1 = new bool[][] { new bool[] { false, true }, new bool[] { true, false } };
            TestX( array1 , Validator_boolean.Get( 2 ) );

            // 3 dimensional
            TestX(
                new bool[][][] {
                    new bool[][] { new bool[] { true, false, false }, new bool[] { true, false, true } },
                    new bool[][] { new bool[] { false, true, false }, new bool[] { false, true, false } },
                    new bool[][] { new bool[] { false, false, false }, new bool[] { true, true, true } }
                },
                Validator_boolean.Get( 3 ) );
	    }

        [Test]
        public void Test_byte() 
	    {
            // 2 dimensional
            Test( new sbyte[][] { new sbyte[] { SByte.MinValue, -1, 0, 1, SByte.MaxValue }, new sbyte[] { 23 } },
                Validator_byte.Get( 2 ) );

            // 3 dimensional
            Test(
                new sbyte[][][] 
                {
                    new sbyte[][] { new sbyte[] { SByte.MinValue, -1, 0, 1, SByte.MaxValue }, new sbyte[] { 23 } },
                    new sbyte[][] { new sbyte[] { SByte.MinValue, 1, 0, -1, SByte.MaxValue }, new sbyte[] { 23 } },
                    new sbyte[][] { new sbyte[] { SByte.MaxValue, -1, 0, 1, SByte.MinValue }, new sbyte[] { 23 } }
                },
                Validator_byte.Get( 3 ) );
	    }

        [Test]
        public void Test_short() 
	    {
            // 2 dimensional
            Test( new short[][] { new short[] { short.MinValue, SByte.MinValue, -1, 0, 1,
			    SByte.MaxValue, short.MaxValue }, new short[] { 23 } },
                Validator_short.Get( 2 ) );

            // 3 dimensional
            Test( 
                new short[][][] 
                {
                    new short[][] { new short[] { short.MinValue, SByte.MinValue, -1, 0, 1,
			        SByte.MaxValue, short.MaxValue }, new short[] { 23 } },
                    new short[][] { new short[] { short.MaxValue, SByte.MinValue, -1, 0, 1,
			        SByte.MaxValue, short.MinValue }, new short[] { 23 } },
                    new short[][] { new short[] { short.MaxValue, SByte.MaxValue, 1, 0, -1,
			        SByte.MinValue, short.MinValue }, new short[] { 23 } }
                },
                Validator_short.Get( 3 ) );
   
	    }

        [Test]
        public void Test_int() 
	    {

            // 2 Dimensional
            Test( new int[][] { new int[] { int.MinValue, short.MinValue, SByte.MinValue,
			    -1, 0, 1, sbyte.MaxValue, short.MaxValue, int.MaxValue}, new int[] { 23 } },
                Validator_int.Get( 2 ) );

            // 3 dimensional
            Test(
                new int[][][]
                {
                    new int[][] { new int[] { int.MinValue, short.MinValue, SByte.MinValue,
			        -1, 0, 1, sbyte.MaxValue, short.MaxValue, int.MaxValue}, new int[] { 23 } },
                    new int[][] { new int[] { int.MaxValue, short.MinValue, SByte.MinValue,
			        -1, 0, 1, sbyte.MaxValue, short.MinValue, int.MaxValue}, new int[] { 23 } },
                    new int[][] { new int[] { int.MaxValue, short.MaxValue, SByte.MaxValue,
			        -1, 0, 1, sbyte.MinValue, short.MinValue, int.MinValue}, new int[] { 23 } }
                },
                Validator_int.Get( 3 ) );
	    }

        [Test]
        public void Test_long() 
	    {
            // 2 dimensional
            Test( new long[][] { new long[] { long.MinValue, int.MinValue, short.MinValue,
			    SByte.MinValue, -1, 0, 1, sbyte.MaxValue, short.MaxValue,
			    int.MaxValue, long.MaxValue}, new long[] { 23 } },
                Validator_long.Get( 2 ) );

            // 3 dimensional
            Test(
                new long[][][]
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
                },
                Validator_long.Get( 3 ) );
	    }

        [Test]
        public void Test_float() 
	    {
            // 2 dimensional
            Test( new float[][] { new float[] { -1, 0, 1, float.MinValue, float.Epsilon,
			    float.MaxValue, float.NaN, float.NegativeInfinity,
			    float.PositiveInfinity, -0.0f, 1.1f,
			    3.141592653589793238462643383279f }, new float[] { 23 } },
                Validator_float.Get( 2 ) );

            // 3 dimensional
            Test(
                new float[][][]
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
                },
                Validator_float.Get( 3 ) );
	    }

        [Test]
        public void Test_double() 
	    {
            // 2 dimensional
            Test( new double[][] { new double[]{ -1, 0, 1, double.MinValue, double.Epsilon,
			    double.MaxValue, double.NaN, double.NegativeInfinity,
			    double.PositiveInfinity, -0.0f, 1.1f,
			    3.141592653589793238462643383279 }, new double[]{ 23 } },
                Validator_double.Get( 2 ) );

            // 3 dimensional
            Test(
                new double[][][]
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
                },
                Validator_double.Get( 3 ) );
	    }

        [Test]
        public void Test_String() 
	    {
            
            // 2 dimensional
            Test( new String[][] { new String[] { "", "a", "ab", "abc" }, new String[] { "23" } },
                Validator_string.Get( 2 ) );

            // 3 dimensional
            Test(
                new String[][][]
                {
                    new String[][] { new String[] { "", "a", "ab", "abc" }, new String[] { "23" } },
                    new String[][] { new String[] { "abc", "ab", "a", "" }, new String[] { "23" } },
                    new String[][] { new String[] { "ab", "abc", "", "a" }, new String[] { "23" } }
                },
                Validator_string.Get( 3 ) );
	    }

        [Test]
        public void Test_Add()
        {
            XType add = vf.GetType( "add" );
            
            Field x = vf.GetField( "x" );
            add.PutValidator( x, Validator_int.Get( 0 ) );

            Field y = vf.GetField( "y" );
            add.PutValidator( y, Validator_int.Get( 0 ) );

            Field _mf__messageId = DefaultValueFactory._mf__messageId;
            add.PutValidator( _mf__messageId, Validator_long.Get( 0 ) );

            long msgid = 0x0123456789abcdefL;

            Message msg = new Message( add, vf );
            msg.Add( x, 1 );
            msg.Add( y, 2 );
            msg.Add( _mf__messageId, msgid );
            /*byte[] buf = */ Msg2bytes( msg );

            msg = new Message( add, vf );
            msg.Add( x, 1000000000 );
            msg.Add( y, 2000000000 );
            msg.Add( _mf__messageId, msgid );

            /*buf = */Msg2bytes( msg );

        }

        [Test]
        public void test_add_in() 
	    {
            XType add = vf.GetType( "add" );
    		
		    Field x = vf.GetField( "x" );
		    add.PutValidator( x, Validator_int.Get( 0 ) );
    		
		    Field y = vf.GetField( "y" );
		    add.PutValidator( y, Validator_int.Get( 0 ) );
    		
		    Field _mf__messageId = DefaultValueFactory._mf__messageId;
		    add.PutValidator( _mf__messageId, Validator_long.Get( 0 ) );
    		
		    long msgid = 0x0123456789abcdefL;
    		
		    //sbyte[] _buf = { 1, -9, -100, -73, -23, 39, -9, 104, -76, 6, 99, -13, -17, -51, -85, -119, 103, 69, 35, 1, -9, -76, 44, 10, 21, 66, -9, -77, 44, 10, 21, 65, -22 } ;
            sbyte[] _buf = { 1, // version
                             -9, // INT4
			                39, -23, -73, -100, // add
			                -9, // INT4
			                99, 6, -76, 104,
			                -13, // LONG8
			                1, 35, 69, 103, -119, -85, -51, -17,
			                -9, // INT4
			                21, 10, 44, -76, // y
			                66, // INT4=2
			                -9, // INT4
			                21, 10, 44, -77, // x
			                65, // INT4=1
			                -22 // NONE 
                            };

            byte[] buf = new byte[ _buf.Length ];
            Buffer.BlockCopy( _buf, 0, buf, 0, _buf.Length );

            Message msg = Bytes2msg( buf );
		    msg.CheckType( add );
		    Assert.AreEqual( 3, msg.Count );
		    Assert.AreEqual( 1, msg[ x ] );
		    Assert.AreEqual( 2, msg[ y ] );
		    Assert.AreEqual( msgid, msg[ _mf__messageId ] );
    		
		    //_buf = new sbyte[] { 1, -9, -100, -73, -23, 39, -9, 104, -76, 6, 99, -13, -17, -51, -85, -119, 103, 69, 35, 1, -9, -76, 44, 10, 21, -9, 0, -108, 53, 119, -9, -77, 44, 10, 21, -9, 0, -54, -102, 59, -22 };
            _buf = new sbyte[] { 1, -9, 39, -23, -73, -100, -9, 99, 6, -76, 104, -13, 1, 35, 69, 103, -119, -85, -51, -17, -9, 21, 10, 44, -76, -9, 119, 53, -108, 0, -9, 21, 10, 44, -77, -9, 59, -102, -54, 0, -22 };
            buf = new byte[ _buf.Length ];
            Buffer.BlockCopy( _buf, 0, buf, 0, _buf.Length );
            
            msg = Bytes2msg( buf ); 
		    msg.CheckType( add );
		    Assert.AreEqual( 3, msg.Count );
		    Assert.AreEqual( 1000000000, msg[x ] );
		    Assert.AreEqual( 2000000000, msg[ y ] );
		    Assert.AreEqual( msgid, msg[ _mf__messageId ] );
	    }



        [Test]
        public void Test_add_perf()
        {
            vf = new MyValueFactory();

            XType add = vf.GetType( "add" );

            Field x = vf.GetField( "x" );
            add.PutValidator( x, Validator_int.Get( 0 ) );

            Field y = vf.GetField( "y" );
            add.PutValidator( y, Validator_int.Get( 0 ) );

            Field _mf__messageId = DefaultValueFactory._mf__messageId;
            add.PutValidator( _mf__messageId, Validator_long.Get( 0 ) );

            long msgid = 0x0123456789abcdefL;

            Message msg = new Message( add, vf );
            msg.Add( x, 123456789 );
            msg.Add( y, 876543210 );
            msg.Add( _mf__messageId, msgid );

            Messagizer m = new Messagizer( null, vf );
            MyPacketSource ps = new MyPacketSource();
            m.SetSource( ps );
            m.Messagex( null, msg );

            int n = 9009730;
            for ( int i = 0; i < 1; i++ )
            {
                TestPerf( "test_add_perf", i, m, msg, n );
            }
        }

        [Test]
        public void Test_sum_perf()
        {
            XType sum = vf.GetType( "sum" );

            Field values = vf.GetField( "values" );
            sum.PutValidator( values, Validator_int.Get( 1 ) );

            Field _mf__messageId = DefaultValueFactory._mf__messageId;
            sum.PutValidator( _mf__messageId, Validator_long.Get( 0 ) );

            long msgid = 0x0123456789abcdefL;

            int[] array = new int[ 1000 ];
            for ( int i = 0; i < 1000; i++ )
                array[ i ] = 123456789;

            Message msg = new Message( sum, vf );
            msg.Add( values, array );
            msg.Add( _mf__messageId, msgid );

            Messagizer m = new Messagizer( null, vf );
            MyPacketSource ps = new MyPacketSource();
            m.SetSource( ps );
            m.Messagex( null, msg );

            int n = 29340;
            for (int i = 0; i < 1; i++)
                TestPerf( "test_sum_perf", i, m, msg, n );
        }

        private static void TestPerf( String name, int iter, Messagizer m, Message msg, int n )
        {
            long t0 = HPTimer.Now();

            for ( int i = 0; i < n; i++ )
                m.Messagex( null, msg );

            long t1 = HPTimer.NsSince( t0 );

            double timeDiff = t1 / 1000000000.0;
            double r = n / timeDiff;

            Console.WriteLine( name + " -- iteration : " + iter + " time : " + timeDiff + 
                " count : " + n + " rate : " + r );
        }

        #region MyPacketSource
        public class MyPacketSource : PacketSource
        {

            public int HeaderSize()
            {
                return 0;
            }

            public void Packet( Who recepient, FlexBuffer buf )
            {
                // ignore
            }

            #region Source Members

            public object GetHandler()
            {
                return null;
            }

            public void SetHandler( object handler )
            {
                // ignore
            }

            #endregion

            #region Transport Members

            public object TransportQuery( object query )
            {
                return null;
            }

            public void TransportControl( object control, object value )
            {
                // ignore
            }

            public void TransportNotify( object eventObj )
            {
                // ignore
            }

            #endregion
        }
        #endregion MyPacketSource

        private byte[] Msg2bytes( Message msg )
        {
            FlexBuffer buf = new FlexBuffer();
            BinaryTaggedDataOutput btdo = new BinaryTaggedDataOutput( buf, vf );
            btdo.WriteMessage( msg );
            buf.SetIndex( 0 );
            return buf.GetAvailBytes();
        }

        private Message Bytes2msg( byte[] buf ) 
	    {
            return BinaryTaggedDataInput.ReadMessage( vf, new FlexBuffer( buf ), null );
	    }

        private void TestX( Object x, Validator v )
	    {
            
            if ( x is Array )
		    {
                int n = ( ( Array ) x ).GetLength( 0 );
			    for (int i = 0; i < n; i++)
			    {
                    Object getValue = ( ( Array ) x ).GetValue( i );
                    TestX( getValue, v.ElementValidator() );
			    }
		    }
		    Object y = Test( x, v );
		    // y = DefaultValueFactory.ArrayValue2Native( y, x.GetType()/*, cnvrtr*/ );
		    AreEqual( x, y );
	    }


        private Object Test( Object x, Validator v )
	    {
		    //Console.WriteLine( "-----------------------------------------" );

            mt_foo.ClearValidator( mf_x );
            mt_foo.PutValidator( mf_x, v );

		    Message msg = new Message( mt_foo, vf );
		    msg.Add( mf_x, x );
		    //Console.WriteLine( "msg = "+msg );

            byte[] bufx = Msg2bytes( msg );
            Message msg2 = Bytes2msg( bufx );
    		
		    //Console.WriteLine( "msg2 = "+msg2 );
		    msg2.CheckType( mt_foo );
		    Assert.AreEqual( 1, msg2.Count );
		    msg.ContainsKey( mf_x );
		    return msg2.Get( mf_x );
	    }

        //public void Dump( byte[] buf )
        //{
        //    Console.Write( "buf = {" );
        //    for (int i = 0; i < buf.Length; i++)
        //    {
        //        if (i > 0)
        //            Console.Write( ", " );
        //        else
        //            Console.Write( " " );
        //        Console.Write( buf[i] );
        //    }
        //    Console.WriteLine( " }" );
        //}

        private void AreEqual( Object a, Object b )
        {
            if (a == null || b == null)
		    {
			    Assert.AreEqual( a, b );
			    return;
		    }
    		
            Type aType = a.GetType();
            Type bType = b.GetType();
    		
		    if (aType == bType)
		    {
                if ( ( a is Array ) && ( b is Array ) )
                    AssertArrayEquals( a, b );
                else
                    Assert.AreEqual( a, b );
		    }
		    //else if (a instanceof Number && b instanceof Number)
		    else
		    {
                Console.WriteLine( "a {0}, b {1}\n", aType, bType );
			    Assert.AreEqual( a, b );
		    }
        }

        private void AssertArrayEquals( Object a, Object b )
        {
            // jagged arrays ... one dimension
            int an = ( ( Array ) a ).GetLength( 0 );
            int bn = ( ( Array ) b ).GetLength( 0 );

            AreEqual( an, bn );

            for ( int i = 0; i < an; i++ )
                AreEqual( ( ( Array ) a ).GetValue( 0 ), ( ( Array ) b ).GetValue( 0 ) );
        }


        public class MyValueFactory : DefaultValueFactory
        {
	        private readonly static TypeMap types = new TypeMap();

	        private readonly static FieldMap fields = new FieldMap();

	        static MyValueFactory()
	        {
		        DefaultValueFactory.Init( types, fields );
	        }

	        /**
	         * Constructs the ValueFactoryFake.
	         */
	        public MyValueFactory()
	        {
		        // nothing to do.
	        }
            
            public override Field GetField( int? id )
            {
                return fields.Get( id );
            }

	        public override Field GetField( String name )
	        {
		        return fields.Get( name );
	        }

	        public override ICollection<Field> GetFields()
	        {
		        return fields.Values();
	        }

	        public override XType GetType( String name )
	        {
		        return types.Get( name );
	        }

	        public override ICollection<XType> GetTypes()
	        {
		        return types.Values();
	        }

	        public override XType GetType( int? id )
	        {
		        return types.Get( id );
	        }
        }

    }
}

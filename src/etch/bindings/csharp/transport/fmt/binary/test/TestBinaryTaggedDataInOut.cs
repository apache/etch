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

    //    private static readonly double DOUBLE_MIN_NORMAL = 0x1.0-1022; // Double.MIN_NORMAL;
	 //   private static readonly float FLOAT_MIN_NORMAL = 0x1.0p-126f; // Float.MIN_NORMAL;
         
         [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine( "TestBinaryTaggedDataInOut" );
        }

        MyValueFactory vf;
        private XType mt_foo;
        private Field mf_x;

        public TestBinaryTaggedDataInOut()
        {
            vf = new MyValueFactory();
            mt_foo = vf.GetType( "foo" );
            mf_x = new Field( "x" );
        }
        
        [Test]
        public void Test_Check_Value() 
        {
            BinaryTaggedDataOutput btdo = new BinaryTaggedDataOutput( null );

            foreach (sbyte i in GetBytes( SByte.MinValue, 256 ))
			if (i >= TypeCode.MIN_TINY_INT && i <= TypeCode.MAX_TINY_INT)
				Assert.AreEqual( i, btdo.CheckValue( i ) );
			else
                Assert.AreEqual(TypeCode.BYTE, btdo.CheckValue(i));

            // short values
		    foreach (short i in GetShorts( short.MinValue, 65536 ))
			if (i >= TypeCode.MIN_TINY_INT && i <= TypeCode.MAX_TINY_INT)
				Assert.AreEqual( (sbyte) i, btdo.CheckValue( i ) );
			else if (i >= SByte.MinValue && i <= SByte.MaxValue)
				Assert.AreEqual( TypeCode.BYTE, btdo.CheckValue( i ) );
			else
				Assert.AreEqual( TypeCode.SHORT, btdo.CheckValue( i ) );
		
		// int values
		foreach (int i in GetTestInts())
			if (i >= TypeCode.MIN_TINY_INT && i <= TypeCode.MAX_TINY_INT)
				Assert.AreEqual( (sbyte) i, btdo.CheckValue( i ) );
			else if (i >= SByte.MinValue && i <= SByte.MaxValue)
				Assert.AreEqual( TypeCode.BYTE, btdo.CheckValue( i ) );
			else if (i >= short.MinValue && i <= short.MaxValue)
				Assert.AreEqual( TypeCode.SHORT, btdo.CheckValue( i ) );
			else
				Assert.AreEqual( TypeCode.INT, btdo.CheckValue( i ) );
		
		// long values
		foreach (long i in GetTestLongs())
			if (i >= TypeCode.MIN_TINY_INT && i <= TypeCode.MAX_TINY_INT)
				Assert.AreEqual( (sbyte) i, btdo.CheckValue( i ) );
			else if (i >= SByte.MinValue && i <= SByte.MaxValue)
				Assert.AreEqual( TypeCode.BYTE, btdo.CheckValue( i ) );
			else if (i >= short.MinValue && i <= short.MaxValue)
				Assert.AreEqual( TypeCode.SHORT, btdo.CheckValue( i ) );
			else if (i >= int.MinValue && i <= int.MaxValue)
				Assert.AreEqual( TypeCode.INT, btdo.CheckValue( i ) );
			else
				Assert.AreEqual( TypeCode.LONG, btdo.CheckValue( i ) );

        // null value
        Assert.AreEqual(TypeCode.NULL, btdo.CheckValue(null));

        // boolean values
        Assert.AreEqual(TypeCode.BOOLEAN_FALSE, btdo.CheckValue(false));
        Assert.AreEqual(TypeCode.BOOLEAN_TRUE, btdo.CheckValue(true));

        // float
        Assert.AreEqual(TypeCode.FLOAT, btdo.CheckValue(3.14159f));

        // double
        Assert.AreEqual(TypeCode.DOUBLE, btdo.CheckValue(3.14159));

        // byte array
        Assert.AreEqual(TypeCode.BYTES, btdo.CheckValue(new sbyte[0]));
        Assert.AreEqual(TypeCode.BYTES, btdo.CheckValue(new sbyte[1]));

        // string
        Assert.AreEqual(TypeCode.EMPTY_STRING, btdo.CheckValue(""));
        Assert.AreEqual(TypeCode.STRING, btdo.CheckValue("abc"));

        // struct
        Assert.AreEqual(TypeCode.CUSTOM, btdo.CheckValue(new StructValue(new XType("foo"))));

        // custom
        Assert.AreEqual(TypeCode.CUSTOM, btdo.CheckValue(new DateTime()));

        // none
        Assert.AreEqual(TypeCode.NONE, btdo.CheckValue(BinaryTaggedData.NONE));


         
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
            
            Field x = new Field( "x" );
            add.PutValidator( x, Validator_int.Get( 0 ) );

            Field y = new Field ("y" );
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
    		
		    Field x = new Field( "x" );
		    add.PutValidator( x, Validator_int.Get( 0 ) );
    		
		    Field y = new Field( "y" );
		    add.PutValidator( y, Validator_int.Get( 0 ) );
    		
		    Field _mf__messageId = DefaultValueFactory._mf__messageId;
		    add.PutValidator( _mf__messageId, Validator_long.Get( 0 ) );
    		
		    long msgid = 0x0123456789abcdefL;
    		
		    //sbyte[] _buf = { 1, -9, -100, -73, -23, 39, -9, 104, -76, 6, 99, -13, -17, -51, -85, -119, 103, 69, 35, 1, -9, -76, 44, 10, 21, 66, -9, -77, 44, 10, 21, 65, -22 } ;
            sbyte[] _buf = { 3, // version
                             -122, // INT (Type)
			                39, -23, -73, -100, // add
			                3, // length
                            -122,
			                99, 6, -76, 104,
			                -121, // LONG (value)
			                1, 35, 69, 103, -119, -85, -51, -17,
			                -122, // INT (key)
			                21, 10, 44, -76, // y
			                2, // tiny int = 2 (value)
			                -122, // INT (key)
			                21, 10, 44, -77, // x
			                1, // tiny int =1 
			                -127 // NONE 
                            };
            byte[] buf = new byte[_buf.Length];
            Buffer.BlockCopy(_buf,0,buf,0,_buf.Length);

            Message msg = Bytes2msg( buf );
		    msg.CheckType( add );
		    Assert.AreEqual( 3, msg.Count );
		    Assert.AreEqual( 1, msg[ x ] );
		    Assert.AreEqual( 2, msg[ y ] );
		    Assert.AreEqual( msgid, msg[ _mf__messageId ] );
    		
		    //_buf = new sbyte[] { 1, -9, -100, -73, -23, 39, -9, 104, -76, 6, 99, -13, -17, -51, -85, -119, 103, 69, 35, 1, -9, -76, 44, 10, 21, -9, 0, -108, 53, 119, -9, -77, 44, 10, 21, -9, 0, -54, -102, 59, -22 };
           // _buf = new sbyte[] { 3, -9, 39, -23, -73, -100, -9, 99, 6, -76, 104, -13, 1, 35, 69, 103, -119, -85, -51, -17, -9, 21, 10, 44, -76, -9, 119, 53, -108, 0, -9, 21, 10, 44, -77, -9, 59, -102, -54, 0, -22 };
            _buf = new sbyte[]
		{
			3, // version
			-122, // INT (type)
			39, -23, -73, -100, // add
			3, // length
			-122, // INT (key)
			99, 6, -76, 104,
			-121, // LONG (value)
			1, 35, 69, 103, -119, -85, -51, -17,
			-122, // INT (key)
			21, 10, 44, -76, // y
			-122, // INT (value)
			119, 53, -108, 0,
			-122, // INT (key)
			21, 10, 44, -77, // x
			-122, // INT (value)
			59, -102, -54, 0,
			-127 // NONE
		};

          buf = new byte[_buf.Length];
            Buffer.BlockCopy(_buf, 0, buf, 0, _buf.Length);

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

            Field x = new Field( "x" );
            add.PutValidator( x, Validator_int.Get( 0 ) );

            Field y = new Field( "y" );
            add.PutValidator( y, Validator_int.Get( 0 ) );

            Field _mf__messageId = DefaultValueFactory._mf__messageId;
            add.PutValidator( _mf__messageId, Validator_long.Get( 0 ) );

            long msgid = 0x0123456789abcdefL;

            Message msg = new Message( add, vf );
            msg.Add( x, 123456789 );
            msg.Add( y, 876543210 );
            msg.Add( _mf__messageId, msgid );
            
            Resources res = new Resources();
            res.Add("Messagizer.valueFactory", vf);
             MyPacketSource ps = new MyPacketSource();
            Messagizer m = new Messagizer(ps,"foo:?Messagizer.format=binary", res);
            m.TransportMessage(null,msg);
          

            int n = 900973;
            for ( int i = 0; i < 3; i++ )
            {
                TestPerf( "test_add_perf", i, m, msg, n );
            }
        }

        [Test]
        public void Test_sum_perf()
        {
            XType sum = vf.GetType( "sum" );

            Field values = new Field( "values" );
            sum.PutValidator( values, Validator_int.Get( 1 ) );

            Field _mf__messageId = DefaultValueFactory._mf__messageId;
            sum.PutValidator( _mf__messageId, Validator_long.Get( 0 ) );

            long msgid = 0x0123456789abcdefL;

            int[] array = new int[ 2 ];
            for ( int i = 0; i < array.Length; i++ )
                array[ i ] = 123456789;

            Message msg = new Message( sum, vf );
            msg.Add( values, array );
            msg.Add( _mf__messageId, msgid );

            Resources res = new Resources();
            res.Add("Messagizer.valueFactory", vf);
            MyPacketSource ps = new MyPacketSource();
            Messagizer m = new Messagizer(ps,"foo:?Messagizer.format=binary", res);
           
            m.TransportMessage( null, msg );

            int n = 509520;
            for (int i = 0; i < 1; i++)
                TestPerf( "test_sum_perf", i, m, msg, n );
        }

        /** @throws Exception */
        [Test]
	    public void testValueToBytes() 
	    {
		assertValueToBytes( null, new sbyte[] { -128 } );
		
		assertValueToBytes( false, new sbyte[] { -126 } );
		assertValueToBytes( true, new sbyte[] { -125 } );
		
		// tiny int:
		assertValueToBytes( 0, new sbyte[] { 0 } );
		assertValueToBytes( 1, new sbyte[] { 1 } );
		assertValueToBytes( -1, new sbyte[] { -1 } );
		
		// byte:
		assertValueToBytes( -100, new sbyte[] { -124, -100 } );
		
		// short:
		assertValueToBytes( 10000, new sbyte[] { -123, 39, 16 } );
		assertValueToBytes( -10000, new sbyte[] { -123, -40, -16 } );
		
		// int:
		assertValueToBytes( 1000000000, new sbyte[] { -122, 59, -102, -54, 0 } );
		assertValueToBytes( -1000000000, new sbyte[] { -122, -60, 101, 54, 0 } );
		
		// long:
		assertValueToBytes( 1000000000000000000L, new sbyte[] { -121, 13, -32, -74, -77, -89, 100, 0, 0 } );
		assertValueToBytes( -1000000000000000000L, new sbyte[] { -121, -14, 31, 73, 76, 88, -100, 0, 0 } );
		
		// float:
		assertValueToBytes( 12345f, new sbyte[] { -120, 70, 64, -28, 0 } );
		
		// double:
		assertValueToBytes( 23456d, new sbyte[] { -119, 64, -42, -24, 0, 0, 0, 0, 0 } );
		
		// boolean[]:
		assertValueToBytes( new Boolean[] { true, false }, new sbyte[] { -111, -125, 1, 2, -125, -126, -127 } );
		
		// byte[]:
		assertValueToBytes( new sbyte[] { 1, 2, 3 }, new sbyte[] { -117, 3, 1, 2, 3 } );

		// byte[][]:
		assertValueToBytes( new sbyte[][] { new sbyte[] { 1, 2, 3 }, new sbyte[] { 2, 3, 4 } }, new sbyte[] { -111, -124, 2, 2, -117, 3, 1, 2, 3, -117, 3, 2, 3, 4, -127 } );

		// short[]:
		assertValueToBytes( new short[] { 1, 2, 3 }, new sbyte[] { -111, -123, 1, 3, 1, 2, 3, -127 } );

		// int[]:
		assertValueToBytes( new int[] { 1, 2, 3 }, new sbyte[] { -111, -122, 1, 3, 1, 2, 3, -127 } );

		// long[]:
		assertValueToBytes( new long[] { 1, 2, 3 }, new sbyte[] { -111, -121, 1, 3, 1, 2, 3, -127 } );

		// float[]:
		assertValueToBytes( new float[] { 1, 2, 3 }, new sbyte[] { -111, -120, 1, 3, -120, 63, -128, 0, 0, -120, 64, 0, 0, 0, -120, 64, 64, 0, 0, -127 } );

		// double[]:
		assertValueToBytes( new double[] { 1, 2, 3 }, new sbyte[] { -111, -119, 1, 3, -119, 63, -16, 0, 0, 0, 0, 0, 0, -119, 64, 0, 0, 0, 0, 0, 0, 0, -119, 64, 8, 0, 0, 0, 0, 0, 0, -127 } );

		// empty string:
		assertValueToBytes( "", new sbyte[] { -110 } );
		
		// string:
		assertValueToBytes( "abc", new sbyte[] { -109, 3, 97, 98, 99 } );
		
		// string[]:
		assertValueToBytes( new String[] { "a", "", "c" }, new sbyte[] { -111, -109, 1, 3, -109, 1, 97, -110, -109, 1, 99, -127 } );

        // Date:
        assertValueToBytes(new DateTime(2008, 7, 3, 15, 24, 37, 507, DateTimeKind.Utc),
            new sbyte[] { -107, -122, 43, 57, 107, -52, 1, -122, 102, 0, 26, 64, -121, 0, 0, 1, 26, -23, -122, 105, 3, -127 });
        
        // Date[]:
        assertValueToBytes(new DateTime[] { new DateTime(2008, 7, 3, 15, 24, 37, 507, DateTimeKind.Utc),
                new DateTime(2007, 6, 2, 14, 23, 36, 506, DateTimeKind.Utc) },
            new sbyte[] { -111, -107, -122, 43, 57, 107, -52, 1, 2, -107, -122, 43, 57, 107, -52, 1, -122, 102, 0, 26, 64, -121, 0, 0, 1, 26, -23, -122, 105, 3, -127, -107, -122, 43, 57, 107, -52, 1, -122, 102, 0, 26, 64, -121, 0, 0, 1, 18, -20, -47, -32, 58, -127, -127 });
    }
	
	private void assertValueToBytes( Object value, sbyte[] expectedBytes ) 
	{
		FlexBuffer buf = new FlexBuffer();
		BinaryTaggedDataOutput btdo = new BinaryTaggedDataOutput( vf );
        btdo.SetBuffer(buf);
		btdo.WriteValue( Validator_object.Get( 0 ), value );
		buf.SetIndex( 0 );
		byte[] b = buf.GetAvailBytes();
        sbyte[] b1 = new sbyte[b.Length];
        Buffer.BlockCopy(b,0,b1,0,b.Length);
        //Dump( b1 );
		AssertArrayEquals( expectedBytes, b1 );
	}

        private static void TestPerf( String name, int iter, Messagizer m, Message msg, int n )
        {
            long t0 = HPTimer.Now();

            for ( int i = 0; i < n; i++ )
                m.TransportMessage( null, msg );

            long t1 = HPTimer.NsSince( t0 );

            double timeDiff = t1 / 1000000000.0;
            double r = n / timeDiff;

            Console.WriteLine( name + " -- iteration : " + iter + " time : " + timeDiff + 
                " count : " + n + " rate : " + r );
        }

        #region MyPacketSource
        public class MyPacketSource : TransportPacket
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

            #region TransportPacket Members


            public void TransportPacket(Who recipient, FlexBuffer buf)
            {
                
            }

            #endregion

            #region Transport<SessionPacket> Members


            public void SetSession(SessionPacket session)
            {
               
            }

            #endregion

            #region Transport<SessionPacket> Members


            public SessionPacket GetSession()
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion
        }
        #endregion MyPacketSource

        private byte[] Msg2bytes( Message msg )
        {
            FlexBuffer buf = new FlexBuffer();
            BinaryTaggedDataOutput btdo = new BinaryTaggedDataOutput( vf );
            btdo.WriteMessage( msg,buf );
            buf.SetIndex( 0 );
            return buf.GetAvailBytes();
        }

        private Message Bytes2msg( byte[] buf ) 
	    {
            return BinaryTaggedDataInput.ReadMessage( vf, new FlexBuffer( buf ));
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

        //public void Dump(sbyte[] buf)
        //{
        //    Console.Write("buf = {");
        //    for (int i = 0; i < buf.Length; i++)
        //    {
        //        if (i > 0)
        //            Console.Write(", ");
        //        else
        //            Console.Write(" ");
        //        Console.Write((sbyte) buf[i]);
        //    }
        //    Console.WriteLine(" }");
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

        private long[] GetTestLongs()
        {
            int n = 65536 + 2 + 6;
            int k = 65536 + 2;
            int min = short.MinValue - 1;
            long[] vals = new long[n];
            int i = 0;
            while (k-- > 0)
                vals[i++] = min++;
            vals[i++] = int.MinValue;
            vals[i++] = int.MaxValue;
            vals[i++] = int.MinValue - 1L;
            vals[i++] = int.MaxValue + 1L;
            vals[i++] = long.MinValue;
            vals[i++] = long.MaxValue;
            return vals;
        }

        private int[] GetTestInts()
        {
            int n = 65536 + 2 + 2;
            int k = 65536 + 2;
            int min = short.MinValue - 1;
            int[] vals = new int[n];
            int i = 0;
            while (k-- > 0)
                vals[i++] = min++;
            vals[i++] = int.MinValue;
            vals[i++] = int.MaxValue;
            return vals;
        }

        private short[] GetShorts(short min, int n)
        {
            short[] vals = new short[n];
            int i = 0;
            while (n-- > 0)
                vals[i++] = min++;
            return vals;
        }


        private sbyte[] GetBytes(sbyte min, int n)
        {
            sbyte[] vals = new sbyte[n];
            int i = 0;
            while (n-- > 0)
                vals[i++] = min++;
            return vals;
        }


        public class MyValueFactory : DefaultValueFactory
        {
	        private readonly static TypeMap types = new TypeMap();

	      

            private readonly static Class2TypeMap class2type = 
                new Class2TypeMap();

	        static MyValueFactory()
            {
                DefaultValueFactory.Init( types, class2type );
	        }

	        /**
	         * Constructs the ValueFactoryFake.
	         */
	        public MyValueFactory() : base (types,class2type)
	        {
		        // nothing to do.
	        }
            
            

	       
        }

    }
}

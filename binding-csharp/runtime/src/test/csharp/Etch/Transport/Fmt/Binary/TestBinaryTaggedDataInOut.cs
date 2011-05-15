// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Transport.Fmt.Binary
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
            BinaryTaggedDataOutput btdo = new BinaryTaggedDataOutput(vf, "none:");

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
        Assert.AreEqual(TypeCode.CUSTOM, btdo.CheckValue(new StructValue(new XType("foo"), vf)));

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
            TestX( new sbyte[][] { new sbyte[] { SByte.MinValue, -1, 0, 1, SByte.MaxValue }, new sbyte[] { 23 } },
                Validator_byte.Get( 2 ) );

            // 3 dimensional
            TestX(
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
            TestX( new short[][] { new short[] { short.MinValue, SByte.MinValue, -1, 0, 1,
			    SByte.MaxValue, short.MaxValue }, new short[] { 23 } },
                Validator_short.Get( 2 ) );

            // 3 dimensional
            TestX( 
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
            TestX( new int[][] { new int[] { int.MinValue, short.MinValue, SByte.MinValue,
			    -1, 0, 1, sbyte.MaxValue, short.MaxValue, int.MaxValue}, new int[] { 23 } },
                Validator_int.Get( 2 ) );

            // 3 dimensional
            TestX(
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
            TestX( new long[][] { new long[] { long.MinValue, int.MinValue, short.MinValue,
			    SByte.MinValue, -1, 0, 1, sbyte.MaxValue, short.MaxValue,
			    int.MaxValue, long.MaxValue}, new long[] { 23 } },
                Validator_long.Get( 2 ) );

            // 3 dimensional
            TestX(
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
            TestX( new float[][] { new float[] { -1, 0, 1, float.MinValue, float.Epsilon,
			    float.MaxValue, float.NaN, float.NegativeInfinity,
			    float.PositiveInfinity, -0.0f, 1.1f,
			    3.141592653589793238462643383279f }, new float[] { 23 } },
                Validator_float.Get( 2 ) );

            // 3 dimensional
            TestX(
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
            TestX( new double[][] { new double[]{ -1, 0, 1, double.MinValue, double.Epsilon,
			    double.MaxValue, double.NaN, double.NegativeInfinity,
			    double.PositiveInfinity, -0.0f, 1.1f,
			    3.141592653589793238462643383279 }, new double[]{ 23 } },
                Validator_double.Get( 2 ) );

            // 3 dimensional
            TestX(
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
            TestX( new String[][] { new String[] { "", "a", "ab", "abc" }, new String[] { "23" } },
                Validator_string.Get( 2 ) );

            // 3 dimensional
            TestX(
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

            long msgid = 0x0102030405060708L;

            Message msg = new Message( add, vf );
            msg.Add( x, 1 );
            msg.Add( y, 2 );
            msg.Add( _mf__messageId, msgid );
            testmsg2bytes(msg, null, new sbyte[] { 3, -122, 39, -23, -73, -100, 3, -122, 21, 10, 44, -77, 1, -122, 99, 6, -76, 104, -121, 1, 2, 3, 4, 5, 6, 7, 8, -122, 21, 10, 44, -76, 2, -127 });
            testmsg2bytes(msg, false, new sbyte[] { 3, -122, 39, -23, -73, -100, 3, -122, 21, 10, 44, -77, 1, -122, 99, 6, -76, 104, -121, 1, 2, 3, 4, 5, 6, 7, 8, -122, 21, 10, 44, -76, 2, -127 });
            testmsg2bytes(msg, true, new sbyte[] { 3, -109, 3, 97, 100, 100, 3, -109, 1, 120, 1, -109, 10, 95, 109, 101, 115, 115, 97, 103, 101, 73, 100, -121, 1, 2, 3, 4, 5, 6, 7, 8, -109, 1, 121, 2, -127 });

            msg = new Message( add, vf );
            msg.Add( x, 1000000000 );
            msg.Add( y, 2000000000 );
            msg.Add( _mf__messageId, msgid );
            testmsg2bytes(msg, null, new sbyte[] { 3, -122, 39, -23, -73, -100, 3, -122, 21, 10, 44, -77, -122, 59, -102, -54, 0, -122, 99, 6, -76, 104, -121, 1, 2, 3, 4, 5, 6, 7, 8, -122, 21, 10, 44, -76, -122, 119, 53, -108, 0, -127 });
            testmsg2bytes(msg, false, new sbyte[] { 3, -122, 39, -23, -73, -100, 3, -122, 21, 10, 44, -77, -122, 59, -102, -54, 0, -122, 99, 6, -76, 104, -121, 1, 2, 3, 4, 5, 6, 7, 8, -122, 21, 10, 44, -76, -122, 119, 53, -108, 0, -127 });
            testmsg2bytes(msg, true, new sbyte[] { 3, -109, 3, 97, 100, 100, 3, -109, 1, 120, -122, 59, -102, -54, 0, -109, 10, 95, 109, 101, 115, 115, 97, 103, 101, 73, 100, -121, 1, 2, 3, 4, 5, 6, 7, 8, -109, 1, 121, -122, 119, 53, -108, 0, -127 });
        }
	
	    private void testmsg2bytes( Message msg, bool? stringTypeAndField,
		    sbyte[] sexpected )
	    {
            byte[] expected = ToSByteArray(sexpected);
		    byte[] actual = Msg2bytes( msg, stringTypeAndField );
		    try
		    {
                AssertArrayEquals(expected, actual);
		    }
		    catch ( Exception )
		    {
			    Dump( expected );
			    Dump( actual );
			    throw;
		    }
	    }

        private byte[] ToSByteArray(sbyte[] a)
        {
            return (byte[])(Array)a;
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
            res.Add(TransportConsts.VALUE_FACTORY, vf);
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
            res.Add(TransportConsts.VALUE_FACTORY, vf);
            MyPacketSource ps = new MyPacketSource();
            Messagizer m = new Messagizer(ps,"foo:?Messagizer.format=binary", res);
           
            m.TransportMessage( null, msg );

            int n = 509520;
            for (int i = 0; i < 1; i++)
                TestPerf( "test_sum_perf", i, m, msg, n );
        }

        [Test]
	    public void testValueToBytes()
        {
            assertValueToBytes(null,
                new sbyte[] { 3, 1, 0, -127 });

            assertValueToBytes(new object[] { null },
                new sbyte[] { 3, 1, 1, 2, -111, -106, 1, 1, -128, -127, -127 });

            assertValueToBytes(false,
                new sbyte[] { 3, 1, 1, 2, -126, -127 });

            assertValueToBytes(true,
                new sbyte[] { 3, 1, 1, 2, -125, -127 });

            // tiny int:
            
            assertValueToBytes(0,
                new sbyte[] { 3, 1, 1, 2, 0, -127 });
            
            assertValueToBytes(1,
                new sbyte[] { 3, 1, 1, 2, 1, -127 });
            
            assertValueToBytes(-1,
                new sbyte[] { 3, 1, 1, 2, -1, -127 });

            // byte:
            
            assertValueToBytes(-100,
                new sbyte[] { 3, 1, 1, 2, -124, -100, -127 });

            // short:
            
            assertValueToBytes(10000,
                new sbyte[] { 3, 1, 1, 2, -123, 39, 16, -127 });
            
            assertValueToBytes(-10000,
                new sbyte[] { 3, 1, 1, 2, -123, -40, -16, -127 });

            // int:
            
            assertValueToBytes(1000000000,
                new sbyte[] { 3, 1, 1, 2, -122, 59, -102, -54, 0, -127 });
            
            assertValueToBytes(-1000000000,
                new sbyte[] { 3, 1, 1, 2, -122, -60, 101, 54, 0, -127 });

            // long:
            
            assertValueToBytes(1000000000000000000L,
                new sbyte[] { 3, 1, 1, 2, -121, 13, -32, -74, -77, -89, 100, 0, 0, -127 });
            
            assertValueToBytes(-1000000000000000000L,
                new sbyte[] { 3, 1, 1, 2, -121, -14, 31, 73, 76, 88, -100, 0, 0, -127 });

            // float:
            
            assertValueToBytes(12345f,
                new sbyte[] { 3, 1, 1, 2, -120, 70, 64, -28, 0, -127 });

            // double:
            
            assertValueToBytes(23456d,
                new sbyte[] { 3, 1, 1, 2, -119, 64, -42, -24, 0, 0, 0, 0, 0, -127 });

            // boolean[]:
            
            assertValueToBytes(new Boolean[] { true, false },
                new sbyte[] { 3, 1, 1, 2, -111, -125, 1, 2, -125, -126, -127, -127 });

            // byte[]:
            
            assertValueToBytes(new sbyte[] { 1, 2, 3 },
                new sbyte[] { 3, 1, 1, 2, -117, 3, 1, 2, 3, -127 });

            // byte[][]:
            
            assertValueToBytes(new sbyte[][] { new sbyte[] { 1, 2, 3 }, new sbyte[] { 2, 3, 4 } },
                new sbyte[] { 3, 1, 1, 2, -111, -124, 2, 2, -117, 3, 1, 2, 3, -117, 3, 2, 3, 4, -127, -127 });

            // short[]:
            
            assertValueToBytes(new short[] { 1, 2, 3 },
                new sbyte[] { 3, 1, 1, 2, -111, -123, 1, 3, 1, 2, 3, -127, -127 });

            // int[]:
            
            assertValueToBytes(new int[] { 1, 2, 3 },
                new sbyte[] { 3, 1, 1, 2, -111, -122, 1, 3, 1, 2, 3, -127, -127 });

            // long[]:
            
            assertValueToBytes(new long[] { 1, 2, 3 },
                new sbyte[] { 3, 1, 1, 2, -111, -121, 1, 3, 1, 2, 3, -127, -127 });

            // float[]:
            
            assertValueToBytes(new float[] { 1, 2, 3 },
                new sbyte[] { 3, 1, 1, 2, -111, -120, 1, 3, -120, 63, -128, 0, 0, -120, 64, 0, 0, 0, -120, 64, 64, 0, 0, -127, -127 });

            // double[]:
            
            assertValueToBytes(new double[] { 1, 2, 3 },
                new sbyte[] { 3, 1, 1, 2, -111, -119, 1, 3, -119, 63, -16, 0, 0, 0, 0, 0, 0, -119, 64, 0, 0, 0, 0, 0, 0, 0, -119, 64, 8, 0, 0, 0, 0, 0, 0, -127, -127 });

            // empty string:
            
            assertValueToBytes("",
                new sbyte[] { 3, 1, 1, 2, -110, -127 });

            // string:

            assertValueToBytes("abc",
                new sbyte[] { 3, 1, 1, 2, -109, 3, 97, 98, 99, -127 });

            // string[]:

            assertValueToBytes(new String[] { "a", "", "c" },
                new sbyte[] { 3, 1, 1, 2, -111, -109, 1, 3, -109, 1, 97, -110, -109, 1, 99, -127, -127 });

            // Date:
            
            assertValueToBytes(new DateTime(2008, 1, 2, 3, 4, 5, 6, DateTimeKind.Utc),
                new sbyte[] { 3, 1, 1, 2, -107, -122, 43, 57, 107, -52, 1, -122, 102, 0, 26, 64, -121, 0, 0, 1, 23, 56, 116, -88, -114, -127, -127 });

            // Date[]:
            
            assertValueToBytes(new DateTime[] { new DateTime(2008, 1, 2, 3, 4, 5, 6, DateTimeKind.Utc), new DateTime(2008, 2, 3, 4, 5, 6, 7, DateTimeKind.Utc) },
                new sbyte[] { 3, 1, 1, 2, -111, -107, -122, 43, 57, 107, -52, 1, 2, -107, -122, 43, 57, 107, -52, 1, -122, 102, 0, 26, 64, -121, 0, 0, 1, 23, 56, 116, -88, -114, -127, -107, -122, 43, 57, 107, -52, 1, -122, 102, 0, 26, 64, -121, 0, 0, 1, 23, -35, 120, 5, 87, -127, -127, -127 });
        }

        [Test]
        public void badtype()
        {
            Message msg = Bytes2msg(new sbyte[] { 3, 1, 0, -127 });
            Assert.AreEqual(1, msg.GetXType.Id);
        }

        [Test]
        [ExpectedException(typeof(ArgumentException))]
        public void badmsglen1()
        {
            Message msg = Bytes2msg(new sbyte[] { 3, 1, -1, -127 });
            Console.WriteLine("msg = " + msg);
        }

        [Test]
        [ExpectedException(typeof(ArgumentException))]
        public void badmsglen2()
        {
            Message msg = Bytes2msg(new sbyte[] { 3, 1, 99, -127 });
            Console.WriteLine("msg = " + msg);
        }

        [Test]
        public void badfield()
        {
            Message msg = Bytes2msg(new sbyte[] { 3, 1, 1, 2, 2, -127 }, Validator.Level.MISSING_OK);
            Console.WriteLine("msg = " + msg);
        }
    	
	    private void assertValueToBytes( Object value, sbyte[] expectedBytes ) 
	    {
            XType t = new XType(1, "a");
            Field f = new Field(2, "b");
            t.PutValidator(f, Validator_object.Get(0));

            Message msg = new Message(t, vf);
            msg.Add(f, value);

		    BinaryTaggedDataOutput btdo = new BinaryTaggedDataOutput( vf, "none:" );
		    FlexBuffer buf = new FlexBuffer();
            btdo.WriteMessage(msg, buf);

		    buf.SetIndex( 0 );
		    byte[] b = buf.GetAvailBytes();
            sbyte[] b1 = new sbyte[b.Length];
            Buffer.BlockCopy(b,0,b1,0,b.Length);
            Dump( b );
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

        private byte[] Msg2bytes(Message msg, bool? stringTypeAndField)
        {
            FlexBuffer buf = new FlexBuffer();

            URL u = new URL("none:");
            if (stringTypeAndField != null)
                u.AddTerm(BinaryTaggedDataOutput.STRING_TYPE_AND_FIELD, stringTypeAndField.ToString());

            BinaryTaggedDataOutput btdo = new BinaryTaggedDataOutput(vf, u.ToString());
            btdo.WriteMessage( msg, buf );
            buf.SetIndex( 0 );
            return buf.GetAvailBytes();
        }

        private Message Bytes2msg(byte[] buf, Validator.Level level)
        {
            BinaryTaggedDataInput btdi = new BinaryTaggedDataInput(vf, "none:");
            return btdi.ReadMessage(new FlexBuffer(buf));
        }

        private Message Bytes2msg(byte[] buf)
        {
            return Bytes2msg(buf, Validator.Level.FULL);
        }

        private Message Bytes2msg(sbyte[] buf, Validator.Level level)
        {
            return Bytes2msg((byte[])(Array)buf, level);
        }

        private Message Bytes2msg(sbyte[] buf)
        {
            return Bytes2msg(buf, Validator.Level.FULL);
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

            Test(x, v, null);
            Test(x, v, false);
            Test(x, v, true);
	    }


        private void Test(Object x, Validator v, bool? stringTypeAndField)
	    {
		    //Console.WriteLine( "-----------------------------------------" );

            mt_foo.ClearValidator( mf_x );
            mt_foo.PutValidator( mf_x, v );

		    Message msg = new Message( mt_foo, vf );
		    msg.Add( mf_x, x );
		    //Console.WriteLine( "msg = "+msg );

            byte[] bufx = Msg2bytes(msg, stringTypeAndField);
            Dump(bufx);
            Message msg2 = Bytes2msg( bufx );
    		
		    //Console.WriteLine( "msg2 = "+msg2 );
		    msg2.CheckType( mt_foo );
		    Assert.AreEqual( 1, msg2.Count );
		    Assert.IsTrue(msg.ContainsKey( mf_x ));
            Object y = msg2.Get(mf_x);
            AreEqual(x, y);
	    }

        public void Dump(byte[] buf)
        {
            Console.Write("buf = {");
            for (int i = 0; i < buf.Length; i++)
            {
                if (i > 0)
                    Console.Write(", ");
                else
                    Console.Write(" ");
                Console.Write((sbyte)buf[i]);
            }
            Console.WriteLine(" }");
        }

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

	        public MyValueFactory()
                : base ("none:", types, class2type)
	        {
		        // nothing to do.
	        }
        }
    }
}

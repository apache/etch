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
using Org.Apache.Etch.Bindings.Csharp.Util;
using NUnit.Framework;
using org.apache.etch.tests;
using org.apache.etch.tests.types.Test1;

namespace etch.tests
{
    [TestFixture]
    public class TestImplTest1
    {
     
    //    private Test1Server ts = null;
        private ImplTest1Server testServer = null;
        private ImplTest1Client testClient = null;

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestImpl" );
        }

        [SetUp]
        public void Setup()
        {
            RemoteTest1Client tc = null;
            RemoteTest1Server ts = null;
            testServer = new ImplTest1Server( tc );
            testClient = new ImplTest1Client( ts);
        }

        [Test]
        public void TestIncr()
        {
            try
            {
                int? i = 5;
                testServer.incr( i );
                testClient.incr( i );
            }
            catch ( Exception )
            { }
        }

        [Test] 
        public void TestNothing() 
        {
		
		    try
		    {			
			    testServer.nothing();		
			    testClient.nothing();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    		
		    }
	    }
	
	    [Test] 
        public void TestAdd() 
        {
		    try
		    {
                int? i = 5;
                int? j = 3;
                testServer.add( i, j );
                testClient.add( i, j );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void TestSub() 
        {
		    try
		    {
                int? i = 5;
                int? j = 3;
                testServer.sub( i, j );
                testClient.sub( i, j );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void TestSum() 
        {
		    try
		    {
                int[] x = { 1, 2, 3 };
			    testServer.sum( x );
			    testClient.sum( x );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }				
	    }
    	
	    [Test] 
        public void  TestFill() 
        {
		    try
		    {
                int? i = 5;
                int? j = 3;
			    testServer.fill( i, j );
                testClient.fill( i, j );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }				
	    }
    	
	    [Test] 
        public void  TestFillObject() 
        {
		    try
		    {
                int? i = 5;
			    Object j = null;
			    testServer.fillObject( i, j );
                testClient.fillObject( i, j );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }				
	    }
    	
    	
	    [Test] 
        public void  TestTrans() 
        {
		    try
		    {
                int? i = 5;
                E1 enumA = E1.A;
                testServer.trans( enumA, i );
                testClient.trans( enumA, i );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }				
	    }
    	
	    [Test] public void  TestDist() 
        {
		    try
		    {
                //int? i = 5;
			    S1 a = new S1();
			    S1 b = new S1();
                testServer.dist( a, b );
			    testClient.dist( a, b );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }			
	    }
    	
	    [Test] 
        public void  TestBlow() 
        {
		    try
		    {
                int? i = 5;
                testServer.blow( "", i );
			    testClient.blow( "", i );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }			
	    }
    	
	    [Test] 
        public void  TestExcp5() 
        {
		    try
		    {
                int? i = 5;
			    Object value = null;
			    testServer.throwExcp5( "", i, value );
			    testClient.throwExcp5( "", i, value );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }			
	    }
    	
	    [Test] 
        public void  TestExcp6() 
        {
		    try
		    {
			    int? i = 5;	
			    Object[] value = null;
			    testServer.throwExcp6( "", i, value );
			    testClient.throwExcp6( "", i, value );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }			
	    }
    	
	    [Test] 
        public void  TestBoolean() 
        {
		    try
		    {
                bool? b = null;
			    testServer.p_boolean( b );
			    testClient.p_boolean( b );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }		
	    }
    	
	    [Test] 
        public void  TestBooleanArray() 
        {
		    try
		    {
			    Boolean[] b = null;	
			    testServer.p_boolean_array( b );
			    testClient.p_boolean_array( b )	;
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestByte() 
        {
		    try
		    {
                SByte? a = null;
			    testServer.p_byte( a );
			    testClient.p_byte( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestByteArray() 
        {
		    try
		    {
			    sbyte[] a = null;
			    testServer.p_byte_array( a );
			    testClient.p_byte_array( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestShort() 
        {
		    try
		    {
                short? a  = null;
			    testServer.p_short( a );
			    testClient.p_short( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestShortArray() 
        {
		    try
		    {
                short[] a = null;
			    testServer.p_short_array( a );
			    testClient.p_short_array( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    		
		    }
	    }
    	
	    [Test] 
        public void  TestInt() 
        {
		    try
		    {
			    int? a  = null;
			    testServer.p_int( a );
			    testClient.p_int( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestIntArray() 
        {
		    try
		    {
			    int[] a  = null;
			    testServer.p_int_array( a );
			    testClient.p_int_array( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestLong() 
        {
		    try
		    {
                long? a  = null;
			    testServer.p_long( a );
			    testClient.p_long( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] public void  testLongArray() {
		    try
		    {
			    long[] a  = null;
			    testServer.p_long_array( a );
			    testClient.p_long_array( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestFloat() 
        {
		    try
		    {
			    float? a  = null;
			    testServer.p_float( a );
			    testClient.p_float( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestFloatArray() 
        {
		    try
		    {
			    float[] a  = null;
			    testServer.p_float_array( a );
			    testClient.p_float_array( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
    	
    	
	    [Test] 
        public void  TestDouble() 
        {
		    try
		    {
			    double? a  = null;
			    testServer.p_double( a );
			    testClient.p_double( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestDoubleArray() 
        {
		    try
		    {
			    double[] a  = null;
			    testServer.p_double_array( a );
			    testClient.p_double_array( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestString() 
        {
		    try
		    {
			    String a  = null;
			    testServer.p_string( a );
			    testClient.p_string( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestStringArray() 
        {
		    try
		    {
			    String[] a  = null;
			    testServer.p_string_array( a );
			    testClient.p_string_array( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestE1() 
        {
		    try
		    {
                E1 a  = default( E1 );
			    testServer.p_E1( a );
			    testClient.p_E1( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestE1Array() 
        {
		    try
		    {
			    E1[] a  = null;
			    testServer.p_E1_array( a );
			    testClient.p_E1_array( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestS1() 
        {
		    try
		    {
                S1 a  = default( S1 );
			    testServer.p_S1( a );
			    testClient.p_S1( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestS1Array() 
        {
		    try
		    {
                S1[] a  = null;
			    testServer.p_S1_array( a );
			    testClient.p_S1_array( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestS2() 
        {
		    try
		    {
                S2 a  = default( S2 );
			    testServer.p_S2( a );
			    testClient.p_S2( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestS2Array() 
        {
		    try
		    {
                S2[] a  = null;
			    testServer.p_S2_array( a );
			    testClient.p_S2_array( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestBlob() 
        {
		    try
		    {
			    StrIntHashMap a  = null;
			    testServer.p_Blob( a );
			    testClient.p_Blob( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestBlobArray() 
        {
		    try
		    {
			    StrIntHashMap[] a  = null;
			    testServer.p_Blob_array( a );
			    testClient.p_Blob_array( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestObject() 
        {
		    try
		    {
			    Object a  = null;
			    testServer.p_object( a );
			    testClient.p_object( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestObjectArray() 
        {
		    try
		    {
			    Object[] a  = null;
			    testServer.p_object_array( a );
			    testClient.p_object_array( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestObjectStruct() 
        {
		    try
		    {
                S3 a  = default( S3 );
			    testServer.p_object_struct( a );
			    testClient.p_object_struct( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestObjectStructArray() 
        {
		    try
		    {
                S4 a  = default( S4 );
			    testServer.p_object_struct_array( a );
			    testClient.p_object_struct_array( a );
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestAlwaysWork() 
        {
		    try
		    {
    			
			    testServer.alwaysWorks();
			    testClient.alwaysWorks();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestAlwaysFail() 
        {
		    try
		    {
    			
			    testServer.alwaysFails();
			    testClient.alwaysFails();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void TestIsTrue() 
        {
		    try
		    {
    			
			    testServer.isTrue();
			    testClient.isTrue();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestIsFalse() 
        {
		    try
		    {
    			
			    testServer.isFalse();
			    testClient.isFalse();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestM1() 
        {
		    try
		    {
    			
			    testServer.m1();
			    testClient.m1();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestCanM1() 
        {
		    try
		    {
    			
			    testServer.can_m1();
			    testClient.can_m1();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestM2() 
        {
		    try
		    {
    			
			    testServer.m2();
			    testClient.m2();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestCanM2() {
		    try
		    {
			    bool? b = null;
			    testServer.can_m2(b);
			    testClient.can_m2(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestM3() 
        {
		    try
		    {
    			
			    testServer.m3();
			    testClient.m3();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  testCanM3() 
        {
		    try
		    {
			    bool? b = null;
			    testServer.can_m3(b);
			    testClient.can_m3(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestM4() 
        {
		    try
		    {
    			
			    testServer.m4();
			    testClient.m4();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestCanM4() 
        {
		    try
		    {
			    bool? b = null;
			    testServer.can_m4(b);
			    testClient.can_m4(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestM5() 
        {
		    try
		    {
    			
			    testServer.m5();
			    testClient.m5();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  testCanM5() {
		    try
		    {
			    sbyte? b = null;
			    testServer.can_m5(b);
			    testClient.can_m5(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestM6() 
        {
		    try
		    {
    			
			    testServer.m6();
			    testClient.m6();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestCanM6() 
        {
		    try
		    {
			    short? b = null;
			    testServer.can_m6(b);
			    testClient.can_m6(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestM7() 
        {
		    try
		    {
    			
			    testServer.m7();
			    testClient.m7();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestCanM7() 
        {
		    try
		    {
			    int? b = null;
			    testServer.can_m7(b);
			    testClient.can_m7(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestM8() 
        {
		    try
		    {
    			
			    testServer.m8();
			    testClient.m8();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestCanM8() 
        {
		    try
		    {
			    long? b = null;
			    testServer.can_m8(b);
			    testClient.can_m8(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestM9() 
        {
		    try
		    {
    			
			    testServer.m6();
			    testClient.m6();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestCanM9() 
        {
		    try
		    {
			    float? b = null;
			    testServer.can_m9(b);
			    testClient.can_m9(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestM10() 
        {
		    try
		    {
    			
			    testServer.m10();
			    testClient.m10();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestCanM10() {
		    try
		    {
			    double? b = null;
			    testServer.can_m10(b);
			    testClient.can_m10(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestM11() 
        {
		    try
		    {
    			
			    testServer.m11();
			    testClient.m11();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestCanM11() 
        {
		    try
		    {
			    String b = null;
			    testServer.can_m11(b);
			    testClient.can_m11(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestM12() 
        {
		    try
		    {
    			
			    testServer.m12();
			    testClient.m12();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestCanM12() 
        {
		    try
		    {
			    int? b = null;
			    testServer.can_m12(b);
			    testClient.can_m12(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestM13() 
        {
		    try
		    {
			    int? b = null;
			    testServer.m13(b);
			    testClient.m13(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestCanM13() 
        {
		    try
		    {
			    int? b = null;
			    testServer.can_m13(b);
			    testClient.can_m13(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestM14() 
        {
		    try
		    {
                S1 b = default( S1 );
			    testServer.m14(b);
			    testClient.m14(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestCanM14() 
        {
		    try
		    {
			    int? b = null;
			    testServer.can_m14(b);
			    testClient.can_m14(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestM15() 
        {
		    try
		    {
    		
			    testServer.m15();
			    testClient.m15();
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    	
	    [Test] 
        public void  TestCanM15() 
        {
		    try
		    {
                E1 b = default( E1 );
			    testServer.can_m15(b);
			    testClient.can_m15(b);
		    }
		    catch ( Exception)
		    {
			    // TODO Auto-generated catch block
    			
		    }
	    }
    }
}

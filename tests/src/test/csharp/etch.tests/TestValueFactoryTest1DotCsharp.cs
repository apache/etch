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
using System.Collections.Generic;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using NUnit.Framework;
using org.apache.etch.tests;
using org.apache.etch.tests.types.Test1;

namespace etch.tests
{
    [TestFixture]
    public class TestValueFactoryTest1DotCsharp
    {
        private ValueFactoryTest1 vf = new ValueFactoryTest1("none:");

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestVF" );
        }

	    [Test]
	    public void test_E1()
	    {
		    // type //
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_E1,
                       
		            ValueFactoryTest1._mf_A,
		   ValueFactoryTest1._mf_B,
		   ValueFactoryTest1._mf_C );
	    }
    	
	    [Test]
	    public void test_E1_export()
	    {
            testEnumExport( E1.A,
			    ValueFactoryTest1._mt_org_apache_etch_tests_Test1_E1,
			    ValueFactoryTest1._mf_A );

            testEnumExport( E1.B,
			    ValueFactoryTest1._mt_org_apache_etch_tests_Test1_E1,
			    ValueFactoryTest1._mf_B );

            testEnumExport( E1.C,
			    ValueFactoryTest1._mt_org_apache_etch_tests_Test1_E1,
			    ValueFactoryTest1._mf_C );
	    }
    	
	    [Test]
	    public void test_E1_import()
	    {
            testEnumImport( E1.A,
			    ValueFactoryTest1._mt_org_apache_etch_tests_Test1_E1,
			    ValueFactoryTest1._mf_A );

            testEnumImport( E1.B,
			    ValueFactoryTest1._mt_org_apache_etch_tests_Test1_E1,
			    ValueFactoryTest1._mf_B );

            testEnumImport( E1.C,
			    ValueFactoryTest1._mt_org_apache_etch_tests_Test1_E1,
			    ValueFactoryTest1._mf_C );
	    }

	    [Test]
	    public void test_S1()
	    {
		    // type //

		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S1, 
    		
		    // fields //
    		
		 ValueFactoryTest1._mf_x,
		    ValueFactoryTest1._mf_y,
		    ValueFactoryTest1._mf_z );
	    }
    	
	    [Test]
	    public void test_S1_export()
	    {
            StructValue sv = vf.ExportCustomValue( new S1( 19, 23, 29 ) );
		    sv.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S1 );
		    Assert.AreEqual( 3, sv.Count );
		    Assert.AreEqual( 19, sv.Get( ValueFactoryTest1._mf_x ) );
		    Assert.AreEqual( 23, sv.Get( ValueFactoryTest1._mf_y ) );
		    Assert.AreEqual( 29, sv.Get( ValueFactoryTest1._mf_z ) );
	    }
    	
	    [Test]
	    public void test_S1_import()
	    {
            StructValue sv = new StructValue(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S1, vf);
		    sv.Add( ValueFactoryTest1._mf_x, 101 );
		    sv.Add( ValueFactoryTest1._mf_y, 103 );
		    sv.Add( ValueFactoryTest1._mf_z, 107 );
            S1 s = ( S1 ) vf.ImportCustomValue( sv );
		    Assert.AreEqual( 101, s.x );
		    Assert.AreEqual( 103, s.y );
		    Assert.AreEqual( 107, s.z );
	    }

	    [Test]
	    public void test_S2()
	    {
		    // type //

		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S2,
   
		    ValueFactoryTest1._mf_a,
		    ValueFactoryTest1._mf_b,
		    ValueFactoryTest1._mf_c );
	    }
    	
	    [Test]
	    public void test_S2_export()
	    {
            S1 a = new S1( 21, 22, 23 );
            S1 b = new S1( 31, 32, 33 );
            E1 c = E1.A;

            StructValue sv = vf.ExportCustomValue( new S2( a, b, c ) );
		    sv.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S2 );
		    Assert.AreEqual( 3, sv.Count );
    		
		    Assert.AreEqual( a, sv.Get( ValueFactoryTest1._mf_a ) );
		    Assert.AreEqual( b, sv.Get( ValueFactoryTest1._mf_b ) );
		    Assert.AreEqual( c, sv.Get( ValueFactoryTest1._mf_c ) );
	    }
    	
	    [Test]
	    public void test_S2_import()
	    {
            StructValue sv = new StructValue(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S2, vf);

            sv.Add( ValueFactoryTest1._mf_a, new S1( 21, 22, 23 ) );
            sv.Add( ValueFactoryTest1._mf_b, new S1( 31, 32, 33 ) );
            sv.Add( ValueFactoryTest1._mf_c, E1.A );

            S2 s = ( S2 ) vf.ImportCustomValue( sv );
		    Assert.AreEqual( 21, s.a.x );
		    Assert.AreEqual( 22, s.a.y );
		    Assert.AreEqual( 23, s.a.z );
		    Assert.AreEqual( 31, s.b.x );
		    Assert.AreEqual( 32, s.b.y );
		    Assert.AreEqual( 33, s.b.z );
            Assert.AreEqual( E1.A, s.c );
	    }
    	
        [Test]
        public void test_S3()
	    {
		    // type //

		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S3,
    		
		    // fields //
	//	    ValueFactoryTest1._mf_type1,
            ValueFactoryTest1._mf_tipe,
		    ValueFactoryTest1._mf_x );
	    }
    	
	    [Test]
	    public void test_S3_export()
	    {		
		    testS3Export("boolean", ConstsTest1.BOOL1);
		    testS3Export("byte", ConstsTest1.BYTE5);
		    testS3Export("short", ConstsTest1.SHORT5);
		    testS3Export("int", ConstsTest1.INT5);
		    testS3Export("long", ConstsTest1.LONG5);
		    testS3Export("float", ConstsTest1.FLOAT5);
		    testS3Export("double", ConstsTest1.DOUBLE5);
		    testS3Export("string", ConstsTest1.STRING3);
	    }
    	
	    [Test]
	    public void test_S3_import()
	    {
		    testS3Import("boolean", ConstsTest1.BOOL1);
		    testS3Import("byte", ConstsTest1.BYTE5);
		    testS3Import("short", ConstsTest1.SHORT5);
		    testS3Import("int", ConstsTest1.INT5);
		    testS3Import("long", ConstsTest1.LONG5);
		    testS3Import("float", ConstsTest1.FLOAT5);
		    testS3Import("double", ConstsTest1.DOUBLE5);
		    testS3Import("string", ConstsTest1.STRING3);
	    }
    	
	    [Test]
	    public void test_S4()
	    {
		    // type //

		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S4,
    		
		    // fields //
		     ValueFactoryTest1._mf_tipe,
		    ValueFactoryTest1._mf_x );
	    }
    	
	    [Test]
	    public void test_S4_export()
	    {	
		    Object[] boolObject = new Object[] {ConstsTest1.BOOL1, ConstsTest1.BOOL2};
		    Object[] byteObject = new Object[] {ConstsTest1.BYTE1, ConstsTest1.BYTE2, ConstsTest1.BYTE3, ConstsTest1.BYTE4, ConstsTest1.BYTE5};
		    Object[] shortObject = new Object[] {ConstsTest1.SHORT1, ConstsTest1.SHORT2, ConstsTest1.SHORT3, ConstsTest1.SHORT4, ConstsTest1.SHORT5};
		    Object[] intObject = new Object[]{ConstsTest1.INT1, ConstsTest1.INT2, ConstsTest1.INT3, ConstsTest1.INT4, ConstsTest1.INT5};
		    Object[] longObject = new Object[] {ConstsTest1.LONG1, ConstsTest1.LONG2, ConstsTest1.LONG3, ConstsTest1.LONG4, ConstsTest1.LONG5};
		    Object[] floatObject = new Object[]{ConstsTest1.FLOAT1, ConstsTest1.FLOAT2, ConstsTest1.FLOAT3, ConstsTest1.FLOAT4, ConstsTest1.FLOAT5};
		    Object[] doubleObject = new Object[] {ConstsTest1.DOUBLE1, ConstsTest1.DOUBLE2, ConstsTest1.DOUBLE3, ConstsTest1.DOUBLE4, ConstsTest1.DOUBLE5};
		    Object[] stringObject = new Object []{ConstsTest1.STRING1, ConstsTest1.STRING2, ConstsTest1.STRING3, ConstsTest1.STRING4, ConstsTest1.STRING5};

		    testS4Export("boolean", boolObject);
		    testS4Export("byte", byteObject);
		    testS4Export("short", shortObject);
		    testS4Export("int", intObject);
		    testS4Export("long", longObject);
		    testS4Export("float", floatObject);
		    testS4Export("double", doubleObject);
		    testS4Export("string", stringObject);
	    }
    	
	    [Test]
	    public void test_S4_import()
	    {
		    Object[] boolObject = new Object[] {ConstsTest1.BOOL1, ConstsTest1.BOOL2};
		    Object[] byteObject = new Object[] {ConstsTest1.BYTE1, ConstsTest1.BYTE2, ConstsTest1.BYTE3, ConstsTest1.BYTE4, ConstsTest1.BYTE5};
		    Object[] shortObject = new Object[] {ConstsTest1.SHORT1, ConstsTest1.SHORT2, ConstsTest1.SHORT3, ConstsTest1.SHORT4, ConstsTest1.SHORT5};
		    Object[] intObject = new Object[]{ConstsTest1.INT1, ConstsTest1.INT2, ConstsTest1.INT3, ConstsTest1.INT4, ConstsTest1.INT5};
		    Object[] longObject = new Object[] {ConstsTest1.LONG1, ConstsTest1.LONG2, ConstsTest1.LONG3, ConstsTest1.LONG4, ConstsTest1.LONG5};
		    Object[] floatObject = new Object[]{ConstsTest1.FLOAT1, ConstsTest1.FLOAT2, ConstsTest1.FLOAT3, ConstsTest1.FLOAT4, ConstsTest1.FLOAT5};
		    Object[] doubleObject = new Object[] {ConstsTest1.DOUBLE1, ConstsTest1.DOUBLE2, ConstsTest1.DOUBLE3, ConstsTest1.DOUBLE4, ConstsTest1.DOUBLE5};
		    Object[] stringObject = new Object []{ConstsTest1.STRING1, ConstsTest1.STRING2, ConstsTest1.STRING3, ConstsTest1.STRING4, ConstsTest1.STRING5};

		    testS4Import("boolean", boolObject);
		    testS4Import("byte", byteObject);
		    testS4Import("short", shortObject);
		    testS4Import("int", intObject);
		    testS4Import("long", longObject);
		    testS4Import("float", floatObject);
		    testS4Import("double", doubleObject);
		    testS4Import("string", stringObject);
	    }	

	    [Test]
	    public void test_excps()
	    {
		    // type //
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp1, ValueFactoryTest1._mf_msg, ValueFactoryTest1._mf_code ); 
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp2 );
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp3 );
            checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp4 );
            checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp5, ValueFactoryTest1._mf_msg,ValueFactoryTest1._mf_code, ValueFactoryTest1._mf_x );
            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp6, ValueFactoryTest1._mf_msg, ValueFactoryTest1._mf_code, ValueFactoryTest1._mf_x);
    		
		    // fields //
    		
		 //   checkField( ValueFactoryTest._mf_msg );
          //  checkField( ValueFactoryTest._mf_code );
          //  checkField( ValueFactoryTest._mf_x );
	    }
    	
	    [Test]
	    public void test_excps_export()
	    {
            Object[] boolObject = new Object[] { ConstsTest1.BOOL1, ConstsTest1.BOOL2 };
            Object[] byteObject = new Object[] { ConstsTest1.BYTE1, ConstsTest1.BYTE2, ConstsTest1.BYTE3, ConstsTest1.BYTE4, ConstsTest1.BYTE5 };
            Object[] shortObject = new Object[] { ConstsTest1.SHORT1, ConstsTest1.SHORT2, ConstsTest1.SHORT3, ConstsTest1.SHORT4, ConstsTest1.SHORT5 };
            Object[] intObject = new Object[] { ConstsTest1.INT1, ConstsTest1.INT2, ConstsTest1.INT3, ConstsTest1.INT4, ConstsTest1.INT5 };
            Object[] longObject = new Object[] { ConstsTest1.LONG1, ConstsTest1.LONG2, ConstsTest1.LONG3, ConstsTest1.LONG4, ConstsTest1.LONG5 };
            Object[] floatObject = new Object[] { ConstsTest1.FLOAT1, ConstsTest1.FLOAT2, ConstsTest1.FLOAT3, ConstsTest1.FLOAT4, ConstsTest1.FLOAT5 };
            Object[] doubleObject = new Object[] { ConstsTest1.DOUBLE1, ConstsTest1.DOUBLE2, ConstsTest1.DOUBLE3, ConstsTest1.DOUBLE4, ConstsTest1.DOUBLE5 };
            Object[] stringObject = new Object[] { ConstsTest1.STRING1, ConstsTest1.STRING2, ConstsTest1.STRING3, ConstsTest1.STRING4, ConstsTest1.STRING5 };

            String msg = "Exception";
            int code = 500;

		    StructValue sv = vf.ExportCustomValue( new Excp1( "abc", 23 ) );
		    sv.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp1 );
		    Assert.AreEqual( 2, sv.Count );
		    Assert.AreEqual( "abc", sv.Get( ValueFactoryTest1._mf_msg ) );
		    Assert.AreEqual( 23, sv.Get( ValueFactoryTest1._mf_code ) );

            sv = vf.ExportCustomValue( new Excp2() );
		    sv.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp2 );
		    Assert.AreEqual( 0, sv.Count );

            sv = vf.ExportCustomValue( new Excp3() );
		    sv.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp3 );
		    Assert.AreEqual( 0, sv.Count );

            sv = vf.ExportCustomValue( new Excp4() );
		    sv.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp4 );
		    Assert.AreEqual( 0, sv.Count );

            // Import exception with object as param
            testExcp5Export( msg, code, ConstsTest1.BOOL2 );
            testExcp5Export( msg, code, ConstsTest1.BYTE5 );
            testExcp5Export( msg, code, ConstsTest1.SHORT5 );
            testExcp5Export( msg, code, ConstsTest1.INT5 );
            testExcp5Export( msg, code, ConstsTest1.LONG5 );
            testExcp5Export( msg, code, ConstsTest1.FLOAT5 );
            testExcp5Export( msg, code, ConstsTest1.DOUBLE5 );
            testExcp5Export( msg, code, ConstsTest1.STRING3 );

            // Import exception with array of object as param
            testExcp6Export( msg, code, boolObject );
            testExcp6Export( msg, code, byteObject );
            testExcp6Export( msg, code, shortObject );
            testExcp6Export( msg, code, intObject );
            testExcp6Export( msg, code, longObject );
            testExcp6Export( msg, code, floatObject );
            testExcp6Export( msg, code, doubleObject );
            testExcp6Export( msg, code, stringObject );	
	    }
    	
	    [Test]
	    public void test_excps_import()
	    {

            Object[] boolObject = new Object[] { ConstsTest1.BOOL1, ConstsTest1.BOOL2 };
            Object[] byteObject = new Object[] { ConstsTest1.BYTE1, ConstsTest1.BYTE2, ConstsTest1.BYTE3, ConstsTest1.BYTE4, ConstsTest1.BYTE5 };
            Object[] shortObject = new Object[] { ConstsTest1.SHORT1, ConstsTest1.SHORT2, ConstsTest1.SHORT3, ConstsTest1.SHORT4, ConstsTest1.SHORT5 };
            Object[] intObject = new Object[] { ConstsTest1.INT1, ConstsTest1.INT2, ConstsTest1.INT3, ConstsTest1.INT4, ConstsTest1.INT5 };
            Object[] longObject = new Object[] { ConstsTest1.LONG1, ConstsTest1.LONG2, ConstsTest1.LONG3, ConstsTest1.LONG4, ConstsTest1.LONG5 };
            Object[] floatObject = new Object[] { ConstsTest1.FLOAT1, ConstsTest1.FLOAT2, ConstsTest1.FLOAT3, ConstsTest1.FLOAT4, ConstsTest1.FLOAT5 };
            Object[] doubleObject = new Object[] { ConstsTest1.DOUBLE1, ConstsTest1.DOUBLE2, ConstsTest1.DOUBLE3, ConstsTest1.DOUBLE4, ConstsTest1.DOUBLE5 };
            Object[] stringObject = new Object[] { ConstsTest1.STRING1, ConstsTest1.STRING2, ConstsTest1.STRING3, ConstsTest1.STRING4, ConstsTest1.STRING5 };

            String msg = "Exception";
            int code = 500;

            StructValue sv = new StructValue(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp1, vf);
		    sv.Add( ValueFactoryTest1._mf_msg, "def" );
		    sv.Add( ValueFactoryTest1._mf_code, 29 );
            Excp1 e1 = ( Excp1 ) vf.ImportCustomValue( sv );
		    Assert.AreEqual( "def", e1.msg );
		    Assert.AreEqual( 29, e1.code );
		    e1 = null;

            sv = new StructValue(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp2, vf);
            Excp2 e2 = ( Excp2 ) vf.ImportCustomValue( sv );
		    Assert.IsNotNull( e2 );
		    e2 = null;

            sv = new StructValue(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp3, vf);
            Excp3 e3 = ( Excp3 ) vf.ImportCustomValue( sv );
		    Assert.IsNotNull( e3 );
		    e3 = null;

            sv = new StructValue(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp4, vf);
            Excp4 e4 = ( Excp4 ) vf.ImportCustomValue( sv );
		    Assert.IsNotNull( e4 );
		    e4 = null;

            // Import exception with object as param
            testExcp5Import( msg, code, ConstsTest1.BOOL2 );
            testExcp5Import( msg, code, ConstsTest1.BYTE5 );
            testExcp5Import( msg, code, ConstsTest1.SHORT5 );
            testExcp5Import( msg, code, ConstsTest1.INT5 );
            testExcp5Import( msg, code, ConstsTest1.LONG5 );
            testExcp5Import( msg, code, ConstsTest1.FLOAT5 );
            testExcp5Import( msg, code, ConstsTest1.DOUBLE5 );
            testExcp5Import( msg, code, ConstsTest1.STRING3 );

            // Import exception with array of object as param
            testExcp6Import( msg, code, boolObject );
            testExcp6Import( msg, code, byteObject );
            testExcp6Import( msg, code, shortObject );
            testExcp6Import( msg, code, intObject );
            testExcp6Import( msg, code, longObject );
            testExcp6Import( msg, code, floatObject );
            testExcp6Import( msg, code, doubleObject );
            testExcp6Import( msg, code, stringObject );	
	    }
    	
	    [Test]
	    public void test_method_nothing()
	    {
            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_nothing, ValueFactoryTest1._mf__messageId);
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_nothing, 
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
			ValueFactoryTest1._mf_result );
		
	    }
    	
	    [Test]
	    public void test_method_incr()
	    {
            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_incr, ValueFactoryTest1._mf__messageId,
        ValueFactoryTest1._mf_x);


            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_incr, ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);
		    
	    }
    	
	    [Test]
	    public void test_method_sub()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sub, ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_x, ValueFactoryTest1._mf_y );


            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sub, ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo, ValueFactoryTest1._mf_result);
		
	    }
    	
	    [Test]
	    public void test_method_sum()
	    {
            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sum, ValueFactoryTest1._mf__messageId,
        ValueFactoryTest1._mf_x);


            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sum, ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);
		 
	    }
    	
	    [Test]
	    public void test_method_trans()
	    {
            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_trans, ValueFactoryTest1._mf__messageId,
        ValueFactoryTest1._mf_e,
        ValueFactoryTest1._mf_x);


            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans, ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);
		
	    }
    	
	    [Test]
	    public void test_method_dist()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_dist,
                ValueFactoryTest1._mf__messageId,
		    ValueFactoryTest1._mf_a,
		    ValueFactoryTest1._mf_b );

            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_dist, ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);
		
	    }
    	
	    [Test]
	    public void test_method_fill()
	    {
            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_fill, ValueFactoryTest1._mf__messageId,
		    ValueFactoryTest1._mf_n, ValueFactoryTest1._mf_x );

            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fill, ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);
		   
	    }
    	
        [Test]
	    public void test_method_fillObject()
	    {
            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_fillObject, ValueFactoryTest1._mf__messageId,
		    ValueFactoryTest1._mf_n, ValueFactoryTest1._mf_o );

            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fillObject, ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);
		   
	    }
    	

	    [Test]
	    public void test_method_blow()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_blow , ValueFactoryTest1._mf__messageId,
		    ValueFactoryTest1._mf_msg,
		    ValueFactoryTest1._mf_code );
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_blow, ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
		  
	    }
    	
	    [Test]
	    public void test_method_beets()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets, ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_e );   		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets, ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
		    
	    }
    	
        [Test]
	    public void test_method_throwExcp5()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_throwExcp5,
                ValueFactoryTest1._mf__messageId,
                ValueFactoryTest1._mf_msg,
                 ValueFactoryTest1._mf_code,
                 ValueFactoryTest1._mf_value);
		
            checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_throwExcp5,
                ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);

		
	    }
    	
	    [Test]
	    public void test_method_throwExcp6()
	    {
            checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_throwExcp5,
                ValueFactoryTest1._mf__messageId,
        ValueFactoryTest1._mf_msg,
        ValueFactoryTest1._mf_code,
        ValueFactoryTest1._mf_value);

		    
            checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_throwExcp6,
                ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);
	
	    }

	    [Test]
	    public void test_method_p_boolean()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_boolean,
                ValueFactoryTest1._mf__messageId,
        ValueFactoryTest1._mf_a);

		  
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean,
                ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);
		  
	    }
    	
	    [Test]
	    public void test_method_p_boolean_array()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_boolean_array,
                ValueFactoryTest1._mf__messageId,
        ValueFactoryTest1._mf_a);
	
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean_array,
                ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);
		
	    }
    	
	    [Test]
	    public void test_method_p_byte()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_byte,
                ValueFactoryTest1._mf__messageId,
        ValueFactoryTest1._mf_a);
	
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte,
                ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);

		
	    }
    	
	    [Test]
	    public void test_method_p_byte_array()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_byte_array,
                ValueFactoryTest1._mf__messageId,
        ValueFactoryTest1._mf_a);

		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte_array,
                ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);

		   
	    }
    	
	    [Test]
	    public void test_method_p_short()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_short,
                ValueFactoryTest1._mf__messageId,
        ValueFactoryTest1._mf_a);
		    
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short,
                ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);
		   
	    }
    	
	    [Test]
	    public void test_method_p_short_array()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_short_array,
               ValueFactoryTest1._mf__messageId,
        ValueFactoryTest1._mf_a);
	
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short_array,
            ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );


		
	    }
    	
	    [Test]
	    public void test_method_p_int()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_int,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int,
                ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);

	
	    }
    	
	    [Test]
	    public void test_method_p_int_array()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_int_array ,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );

    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int_array,
                ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);
	    }
    	
	    [Test]
	    public void test_method_p_long()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_long,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long,
                ValueFactoryTest1._mf__messageId,
            ValueFactoryTest1._mf__inReplyTo,
        ValueFactoryTest1._mf_result);


		
	    }
    	
	    [Test]
	    public void test_method_p_long_array()
	    {
	        checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_long_array,
	                  ValueFactoryTest1._mf__messageId,
	                  ValueFactoryTest1._mf_a);


	        checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long_array,
	                  ValueFactoryTest1._mf__messageId,
	                  ValueFactoryTest1._mf__inReplyTo,
	                  ValueFactoryTest1._mf_result);
	    }

        [Test]
	    public void test_method_p_float()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_float,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		   
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_float,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
		    
	    }
    	
	    [Test]
	    public void test_method_p_float_array()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_float_array,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );

    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_float_array,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );

	
	    }
    	
	    [Test]
	    public void test_method_p_double()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_double,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );

    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_double,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );

		   
	    }
    	
	    [Test]
	    public void test_method_p_double_array()
	    {
	        checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_double_array,
	                  ValueFactoryTest1._mf__messageId,
	                  ValueFactoryTest1._mf_a);
		   
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_double_array,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );

	
	    }
    	
	    [Test]
	    public void test_method_p_string()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_string,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );

		   
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );

		   
	    }

        [Test]
        public void test_method_p_string_array()
        {
            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_string_array,
                      ValueFactoryTest1._mf__messageId,
                      ValueFactoryTest1._mf_a);


            checkType(ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string_array,
                      ValueFactoryTest1._mf__messageId,
                      ValueFactoryTest1._mf__inReplyTo,
                      ValueFactoryTest1._mf_result);
        }

        [Test]
	    public void test_method_p_E1()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_E1,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );

		    
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_E1,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );

		    
	    }
    	
	    [Test]
	    public void test_method_p_E1_array()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_E1_array,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );


    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_E1_array,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	
	    }
    	
	    [Test]
	    public void test_method_p_S1()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S1,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S1,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );

	    }
    	
	    [Test]
	    public void test_method_p_S1_array()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S1_array,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );

		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S1_array,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	    
	    }
    	
	    [Test]
	    public void test_method_p_S2()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S2,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S2,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
		   
	    }
    	
	    [Test]
	    public void test_method_p_S2_array()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S2_array,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S2_array,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
		  
	    }
    	
	    [Test]
	    public void test_method_p_Blob()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_Blob,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_Blob,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
		  
	    }
    	
	    [Test]
	    public void test_method_p_Blob_array()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_Blob_array,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );

    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_Blob_array,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
		   
	    }

        [Test]
	    public void test_method_p_object()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_object,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		   
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_object,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
		   
	    }
    	
	    [Test]
	    public void test_method_p_object_array()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_object_array,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		   
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_object_array,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
		   
	    }
    	
	    [Test]
	    public void test_method_p_object_struct()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_object_struct,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		   
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_object_struct,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
		   
	    }

	    [Test]
	    public void test_method_p_object_struct_array()
	    {
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_object_struct_array,
                ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		    
    		
		    checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_object_struct_array,
                ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
		  
	    }

	    /////////////////////
	    // UTILITY METHODS //
	    /////////////////////
	    

        private void checkType( XType type, params Field[] fields )
	{
		 Assert.IsNotNull( type );
		Assert.AreSame( typeof(XType), type.GetType() );
		Assert.AreSame( type, vf.GetType( type.Id ) );
		
		List<Field> tfields = type.GetFields();

		if (fields != null)
		{
			Assert.AreEqual( fields.Length, tfields.Count );
			
			//for (Field f: fields)
            for (int i = 0; i < fields.Length;i++ )
            {
                Assert.IsNotNull(type.GetValidator(fields[i]));
                Assert.AreSame(fields[i], type.GetField(fields[i].Id));
                Assert.AreSame(fields[i], type.GetField(fields[i].Name));
            }
		}
		else
		{
			Assert.AreEqual( 0, tfields.Count );
		}
	}

	   
	    private void testEnumExport( E1 e, XType t, Field f )
	    {
		    StructValue sv = vf.ExportCustomValue( e );
		    sv.CheckType( t );
		    Assert.AreEqual( 1, sv.Count );
		    Assert.IsTrue( (Boolean) sv.Get( f ) );
	    }
    	
	    private void testEnumImport( E1 e, XType t, Field f )
	    {
            StructValue sv = new StructValue(t, vf);
		    sv.Add( f, true );
		    E1 a = (E1) vf.ImportCustomValue( sv );
		    Assert.AreEqual( e, a );
	    }

        private void testS3Export( String s, Object value )
        {
            StructValue sv = vf.ExportCustomValue( new S3( s, value ) );
            sv.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S3 );
            Assert.AreEqual( 2, sv.Count );
            Assert.AreEqual( s, sv[ ValueFactoryTest1._mf_tipe ] );
            Assert.AreEqual( value, sv[ ValueFactoryTest1._mf_x ] );
        }

        private void testS3Import( String s, Object value )
        {
            StructValue sv = new StructValue(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S3, vf);
            sv.Add(ValueFactoryTest1._mf_tipe, s);
            sv.Add( ValueFactoryTest1._mf_x, value );
            S3 myS3 = ( S3 ) vf.ImportCustomValue( sv );
            Assert.AreEqual( s, myS3.tipe );
            Assert.AreEqual( value, myS3.x );
        }

        private void testS4Export( String s, Object[] value )
        {
            StructValue sv = vf.ExportCustomValue( new S4( s, value ) );
            sv.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S4 );
            Assert.AreEqual( 2, sv.Count );
            Assert.AreEqual(s, sv[ValueFactoryTest1._mf_tipe]);
            Assert.AreEqual( value, sv[ ValueFactoryTest1._mf_x ] );
        }

        private void testS4Import( String s, Object[] value )
        {
            StructValue sv = new StructValue(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S4, vf);
            sv.Add(ValueFactoryTest1._mf_tipe, s);
            sv.Add( ValueFactoryTest1._mf_x, value );
            S4 myS4 = ( S4 ) vf.ImportCustomValue( sv );
            Assert.AreEqual( s, myS4.tipe );
            Assert.AreEqual( value, myS4.x );
        }

        private void testExcp5Export( String msg, int code, Object value )
        {
            StructValue sv = vf.ExportCustomValue( new Excp5( msg, code, value ) );
            sv.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp5 );
            Assert.AreEqual( 3, sv.Count );
            Assert.AreEqual( msg, sv[ValueFactoryTest1._mf_msg] );
            Assert.AreEqual( code, sv[ ValueFactoryTest1._mf_code] );
            Assert.AreEqual( value, sv[ ValueFactoryTest1._mf_x ] );
        }

        private void testExcp5Import( String msg, int code, Object value )
        {
            StructValue sv = new StructValue(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp5, vf);
            sv.Add( ValueFactoryTest1._mf_msg, msg );
            sv.Add( ValueFactoryTest1._mf_code, code );
            sv.Add( ValueFactoryTest1._mf_x, value );
            Excp5 e = ( Excp5 ) vf.ImportCustomValue( sv );
            Assert.AreEqual( msg, e.msg );
            Assert.AreEqual( code, e.code );
            Assert.AreEqual( value, e.x );
        }

        private void testExcp6Export( String msg, int code, Object[] value )
        {
            StructValue sv = vf.ExportCustomValue( new Excp6( msg, code, value ) );
            sv.CheckType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp6 );
            Assert.AreEqual( 3, sv.Count );
            Assert.AreEqual( msg, sv[ ValueFactoryTest1._mf_msg ] );
            Assert.AreEqual( code, sv[ ValueFactoryTest1._mf_code]  );
            Assert.AreEqual( value, sv[ ValueFactoryTest1._mf_x ] );
        }

        private void testExcp6Import( String msg, int code, Object[] value )
        {
            StructValue sv = new StructValue(ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp6, vf);
            sv.Add( ValueFactoryTest1._mf_msg, msg );
            sv.Add( ValueFactoryTest1._mf_code, code );
            sv.Add( ValueFactoryTest1._mf_x, value );
            Excp6 e = ( Excp6 ) vf.ImportCustomValue( sv );
            Assert.AreEqual( msg, e.msg );
            Assert.AreEqual( code, e.code );
            Assert.AreEqual( value, e.x );
        }
    }
}

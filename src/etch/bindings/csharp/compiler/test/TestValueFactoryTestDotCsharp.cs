using System;
using System.Collections.Generic;
using System.Text;

using NUnit.Framework;
using Etch.Msg;
using Etch.Support;
using Etch.Transport;
using etch.bindings.csharp.compiler.test;

namespace Etch.Compiler.Test
{
    [TestFixture]
    public class TestValueFactoryTestDotCsharp
    {
        private ValueFactoryTest vf = new ValueFactoryTest();
	
	    [Test]
	    public void test_E1()
	    {
		    // type //
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_E1 );
    		
		    // fields //
    		
		    checkField( ValueFactoryTest._mf_A );
		    checkField( ValueFactoryTest._mf_B );
		    checkField( ValueFactoryTest._mf_C );
	    }
    	
	    [Test]
	    public void test_E1_export()
	    {
            testEnumExport( etch.bindings.csharp.compiler.test.ConstsTest.E1.A,
			    ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_E1,
			    ValueFactoryTest._mf_A );

            testEnumExport( etch.bindings.csharp.compiler.test.ConstsTest.E1.B,
			    ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_E1,
			    ValueFactoryTest._mf_B );

            testEnumExport( etch.bindings.csharp.compiler.test.ConstsTest.E1.C,
			    ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_E1,
			    ValueFactoryTest._mf_C );
	    }
    	
	    [Test]
	    public void test_E1_import()
	    {
            testEnumImport( etch.bindings.csharp.compiler.test.ConstsTest.E1.A,
			    ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_E1,
			    ValueFactoryTest._mf_A );

            testEnumImport( etch.bindings.csharp.compiler.test.ConstsTest.E1.B,
			    ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_E1,
			    ValueFactoryTest._mf_B );

            testEnumImport( etch.bindings.csharp.compiler.test.ConstsTest.E1.C,
			    ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_E1,
			    ValueFactoryTest._mf_C );
	    }

	    [Test]
	    public void test_S1()
	    {
		    // type //

		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_S1 );
    		
		    // fields //
    		
		    checkField( ValueFactoryTest._mf_x );
		    checkField( ValueFactoryTest._mf_y );
		    checkField( ValueFactoryTest._mf_z );
	    }
    	
	    [Test]
	    public void test_S1_export()
	    {
            StructValue sv = vf.ExportCustomValue( new etch.bindings.csharp.compiler.test.ConstsTest.S1( 19, 23, 29 ) );
		    sv.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_S1 );
		    Assert.AreEqual( 3, sv.Count );
		    Assert.AreEqual( 19, sv.Get( ValueFactoryTest._mf_x ) );
		    Assert.AreEqual( 23, sv.Get( ValueFactoryTest._mf_y ) );
		    Assert.AreEqual( 29, sv.Get( ValueFactoryTest._mf_z ) );
	    }
    	
	    [Test]
	    public void test_S1_import()
	    {
		    StructValue sv = new StructValue( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_S1 );
		    sv.Add( ValueFactoryTest._mf_x, 101 );
		    sv.Add( ValueFactoryTest._mf_y, 103 );
		    sv.Add( ValueFactoryTest._mf_z, 107 );
            etch.bindings.csharp.compiler.test.ConstsTest.S1 s = ( etch.bindings.csharp.compiler.test.ConstsTest.S1 ) vf.ImportCustomValue( sv );
		    Assert.AreEqual( 101, s.x );
		    Assert.AreEqual( 103, s.y );
		    Assert.AreEqual( 107, s.z );
	    }

	    [Test]
	    public void test_S2()
	    {
		    // type //

		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_S2 );
    		
		    // fields //
    		
		    checkField( ValueFactoryTest._mf_a );
		    checkField( ValueFactoryTest._mf_b );
		    checkField( ValueFactoryTest._mf_c );
	    }
    	
	    [Test]
	    public void test_S2_export()
	    {
            etch.bindings.csharp.compiler.test.ConstsTest.S1 a = new etch.bindings.csharp.compiler.test.ConstsTest.S1( 21, 22, 23 );
            etch.bindings.csharp.compiler.test.ConstsTest.S1 b = new etch.bindings.csharp.compiler.test.ConstsTest.S1( 31, 32, 33 );
            etch.bindings.csharp.compiler.test.ConstsTest.E1 c = etch.bindings.csharp.compiler.test.ConstsTest.E1.A;

            StructValue sv = vf.ExportCustomValue( new etch.bindings.csharp.compiler.test.ConstsTest.S2( a, b, c ) );
		    sv.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_S2 );
		    Assert.AreEqual( 3, sv.Count );
    		
		    Assert.AreEqual( a, sv.Get( ValueFactoryTest._mf_a ) );
		    Assert.AreEqual( b, sv.Get( ValueFactoryTest._mf_b ) );
		    Assert.AreEqual( c, sv.Get( ValueFactoryTest._mf_c ) );
	    }
    	
	    [Test]
	    public void test_S2_import()
	    {
		    StructValue sv = new StructValue( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_S2 );

            sv.Add( ValueFactoryTest._mf_a, new etch.bindings.csharp.compiler.test.ConstsTest.S1( 21, 22, 23 ) );
            sv.Add( ValueFactoryTest._mf_b, new etch.bindings.csharp.compiler.test.ConstsTest.S1( 31, 32, 33 ) );
            sv.Add( ValueFactoryTest._mf_c, etch.bindings.csharp.compiler.test.ConstsTest.E1.A );

            etch.bindings.csharp.compiler.test.ConstsTest.S2 s = ( etch.bindings.csharp.compiler.test.ConstsTest.S2 ) vf.ImportCustomValue( sv );
		    Assert.AreEqual( 21, s.a.x );
		    Assert.AreEqual( 22, s.a.y );
		    Assert.AreEqual( 23, s.a.z );
		    Assert.AreEqual( 31, s.b.x );
		    Assert.AreEqual( 32, s.b.y );
		    Assert.AreEqual( 33, s.b.z );
            Assert.AreEqual( etch.bindings.csharp.compiler.test.ConstsTest.E1.A, s.c );
	    }
    	
        [Test]
        public void test_S3()
	    {
		    // type //

		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_S3 );
    		
		    // fields //
		    checkField( ValueFactoryTest._mf_type );
		    checkField( ValueFactoryTest._mf_x );
	    }
    	
	    [Test]
	    public void test_S3_export()
	    {		
		    testS3Export("boolean", ConstsTest.BOOL1);
		    testS3Export("byte", ConstsTest.BYTE5);
		    testS3Export("short", ConstsTest.SHORT5);
		    testS3Export("int", ConstsTest.INT5);
		    testS3Export("long", ConstsTest.LONG5);
		    testS3Export("float", ConstsTest.FLOAT5);
		    testS3Export("double", ConstsTest.DOUBLE5);
		    testS3Export("string", ConstsTest.STRING3);
	    }
    	
	    [Test]
	    public void test_S3_import()
	    {
		    testS3Import("boolean", ConstsTest.BOOL1);
		    testS3Import("byte", ConstsTest.BYTE5);
		    testS3Import("short", ConstsTest.SHORT5);
		    testS3Import("int", ConstsTest.INT5);
		    testS3Import("long", ConstsTest.LONG5);
		    testS3Import("float", ConstsTest.FLOAT5);
		    testS3Import("double", ConstsTest.DOUBLE5);
		    testS3Import("string", ConstsTest.STRING3);
	    }
    	
	    [Test]
	    public void test_S4()
	    {
		    // type //

		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_S4 );
    		
		    // fields //
		    checkField( ValueFactoryTest._mf_type );
		    checkField( ValueFactoryTest._mf_x );
	    }
    	
	    [Test]
	    public void test_S4_export()
	    {	
		    Object[] boolObject = new Object[] {ConstsTest.BOOL1, ConstsTest.BOOL2};
		    Object[] byteObject = new Object[] {ConstsTest.BYTE1, ConstsTest.BYTE2, ConstsTest.BYTE3, ConstsTest.BYTE4, ConstsTest.BYTE5};
		    Object[] shortObject = new Object[] {ConstsTest.SHORT1, ConstsTest.SHORT2, ConstsTest.SHORT3, ConstsTest.SHORT4, ConstsTest.SHORT5};
		    Object[] intObject = new Object[]{ConstsTest.INT1, ConstsTest.INT2, ConstsTest.INT3, ConstsTest.INT4, ConstsTest.INT5};
		    Object[] longObject = new Object[] {ConstsTest.LONG1, ConstsTest.LONG2, ConstsTest.LONG3, ConstsTest.LONG4, ConstsTest.LONG5};
		    Object[] floatObject = new Object[]{ConstsTest.FLOAT1, ConstsTest.FLOAT2, ConstsTest.FLOAT3, ConstsTest.FLOAT4, ConstsTest.FLOAT5};
		    Object[] doubleObject = new Object[] {ConstsTest.DOUBLE1, ConstsTest.DOUBLE2, ConstsTest.DOUBLE3, ConstsTest.DOUBLE4, ConstsTest.DOUBLE5};
		    Object[] stringObject = new Object []{ConstsTest.STRING1, ConstsTest.STRING2, ConstsTest.STRING3, ConstsTest.STRING4, ConstsTest.STRING5};

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
		    Object[] boolObject = new Object[] {ConstsTest.BOOL1, ConstsTest.BOOL2};
		    Object[] byteObject = new Object[] {ConstsTest.BYTE1, ConstsTest.BYTE2, ConstsTest.BYTE3, ConstsTest.BYTE4, ConstsTest.BYTE5};
		    Object[] shortObject = new Object[] {ConstsTest.SHORT1, ConstsTest.SHORT2, ConstsTest.SHORT3, ConstsTest.SHORT4, ConstsTest.SHORT5};
		    Object[] intObject = new Object[]{ConstsTest.INT1, ConstsTest.INT2, ConstsTest.INT3, ConstsTest.INT4, ConstsTest.INT5};
		    Object[] longObject = new Object[] {ConstsTest.LONG1, ConstsTest.LONG2, ConstsTest.LONG3, ConstsTest.LONG4, ConstsTest.LONG5};
		    Object[] floatObject = new Object[]{ConstsTest.FLOAT1, ConstsTest.FLOAT2, ConstsTest.FLOAT3, ConstsTest.FLOAT4, ConstsTest.FLOAT5};
		    Object[] doubleObject = new Object[] {ConstsTest.DOUBLE1, ConstsTest.DOUBLE2, ConstsTest.DOUBLE3, ConstsTest.DOUBLE4, ConstsTest.DOUBLE5};
		    Object[] stringObject = new Object []{ConstsTest.STRING1, ConstsTest.STRING2, ConstsTest.STRING3, ConstsTest.STRING4, ConstsTest.STRING5};

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
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp1 );
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp2 );
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp3 );
            checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp4 );
            checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp5 );
            checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp6 );
    		
		    // fields //
    		
		    checkField( ValueFactoryTest._mf_msg );
            checkField( ValueFactoryTest._mf_code );
            checkField( ValueFactoryTest._mf_x );
	    }
    	
	    [Test]
	    public void test_excps_export()
	    {
            Object[] boolObject = new Object[] { ConstsTest.BOOL1, ConstsTest.BOOL2 };
            Object[] byteObject = new Object[] { ConstsTest.BYTE1, ConstsTest.BYTE2, ConstsTest.BYTE3, ConstsTest.BYTE4, ConstsTest.BYTE5 };
            Object[] shortObject = new Object[] { ConstsTest.SHORT1, ConstsTest.SHORT2, ConstsTest.SHORT3, ConstsTest.SHORT4, ConstsTest.SHORT5 };
            Object[] intObject = new Object[] { ConstsTest.INT1, ConstsTest.INT2, ConstsTest.INT3, ConstsTest.INT4, ConstsTest.INT5 };
            Object[] longObject = new Object[] { ConstsTest.LONG1, ConstsTest.LONG2, ConstsTest.LONG3, ConstsTest.LONG4, ConstsTest.LONG5 };
            Object[] floatObject = new Object[] { ConstsTest.FLOAT1, ConstsTest.FLOAT2, ConstsTest.FLOAT3, ConstsTest.FLOAT4, ConstsTest.FLOAT5 };
            Object[] doubleObject = new Object[] { ConstsTest.DOUBLE1, ConstsTest.DOUBLE2, ConstsTest.DOUBLE3, ConstsTest.DOUBLE4, ConstsTest.DOUBLE5 };
            Object[] stringObject = new Object[] { ConstsTest.STRING1, ConstsTest.STRING2, ConstsTest.STRING3, ConstsTest.STRING4, ConstsTest.STRING5 };

            String msg = "Exception";
            int code = 500;

		    StructValue sv = vf.ExportCustomValue( new etch.bindings.csharp.compiler.test.ConstsTest.Excp1( "abc", 23 ) );
		    sv.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp1 );
		    Assert.AreEqual( 2, sv.Count );
		    Assert.AreEqual( "abc", sv.Get( ValueFactoryTest._mf_msg ) );
		    Assert.AreEqual( 23, sv.Get( ValueFactoryTest._mf_code ) );

            sv = vf.ExportCustomValue( new etch.bindings.csharp.compiler.test.ConstsTest.Excp2() );
		    sv.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp2 );
		    Assert.AreEqual( 0, sv.Count );

            sv = vf.ExportCustomValue( new etch.bindings.csharp.compiler.test.ConstsTest.Excp3() );
		    sv.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp3 );
		    Assert.AreEqual( 0, sv.Count );

            sv = vf.ExportCustomValue( new etch.bindings.csharp.compiler.test.ConstsTest.Excp4() );
		    sv.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp4 );
		    Assert.AreEqual( 0, sv.Count );

            // Import exception with object as param
            testExcp5Export( msg, code, ConstsTest.BOOL2 );
            testExcp5Export( msg, code, ConstsTest.BYTE5 );
            testExcp5Export( msg, code, ConstsTest.SHORT5 );
            testExcp5Export( msg, code, ConstsTest.INT5 );
            testExcp5Export( msg, code, ConstsTest.LONG5 );
            testExcp5Export( msg, code, ConstsTest.FLOAT5 );
            testExcp5Export( msg, code, ConstsTest.DOUBLE5 );
            testExcp5Export( msg, code, ConstsTest.STRING3 );

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

            Object[] boolObject = new Object[] { ConstsTest.BOOL1, ConstsTest.BOOL2 };
            Object[] byteObject = new Object[] { ConstsTest.BYTE1, ConstsTest.BYTE2, ConstsTest.BYTE3, ConstsTest.BYTE4, ConstsTest.BYTE5 };
            Object[] shortObject = new Object[] { ConstsTest.SHORT1, ConstsTest.SHORT2, ConstsTest.SHORT3, ConstsTest.SHORT4, ConstsTest.SHORT5 };
            Object[] intObject = new Object[] { ConstsTest.INT1, ConstsTest.INT2, ConstsTest.INT3, ConstsTest.INT4, ConstsTest.INT5 };
            Object[] longObject = new Object[] { ConstsTest.LONG1, ConstsTest.LONG2, ConstsTest.LONG3, ConstsTest.LONG4, ConstsTest.LONG5 };
            Object[] floatObject = new Object[] { ConstsTest.FLOAT1, ConstsTest.FLOAT2, ConstsTest.FLOAT3, ConstsTest.FLOAT4, ConstsTest.FLOAT5 };
            Object[] doubleObject = new Object[] { ConstsTest.DOUBLE1, ConstsTest.DOUBLE2, ConstsTest.DOUBLE3, ConstsTest.DOUBLE4, ConstsTest.DOUBLE5 };
            Object[] stringObject = new Object[] { ConstsTest.STRING1, ConstsTest.STRING2, ConstsTest.STRING3, ConstsTest.STRING4, ConstsTest.STRING5 };

            String msg = "Exception";
            int code = 500;

		    StructValue sv = new StructValue( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp1 );
		    sv.Add( ValueFactoryTest._mf_msg, "def" );
		    sv.Add( ValueFactoryTest._mf_code, 29 );
            etch.bindings.csharp.compiler.test.ConstsTest.Excp1 e1 = ( etch.bindings.csharp.compiler.test.ConstsTest.Excp1 ) vf.ImportCustomValue( sv );
		    Assert.AreEqual( "def", e1.msg );
		    Assert.AreEqual( 29, e1.code );
		    e1 = null;
    		
		    sv = new StructValue( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp2 );
            etch.bindings.csharp.compiler.test.ConstsTest.Excp2 e2 = ( etch.bindings.csharp.compiler.test.ConstsTest.Excp2 ) vf.ImportCustomValue( sv );
		    Assert.IsNotNull( e2 );
		    e2 = null;
    		
		    sv = new StructValue( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp3 );
            etch.bindings.csharp.compiler.test.ConstsTest.Excp3 e3 = ( etch.bindings.csharp.compiler.test.ConstsTest.Excp3 ) vf.ImportCustomValue( sv );
		    Assert.IsNotNull( e3 );
		    e3 = null;
    		
		    sv = new StructValue( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp4 );
            etch.bindings.csharp.compiler.test.ConstsTest.Excp4 e4 = ( etch.bindings.csharp.compiler.test.ConstsTest.Excp4 ) vf.ImportCustomValue( sv );
		    Assert.IsNotNull( e4 );
		    e4 = null;

            // Import exception with object as param
            testExcp5Import( msg, code, ConstsTest.BOOL2 );
            testExcp5Import( msg, code, ConstsTest.BYTE5 );
            testExcp5Import( msg, code, ConstsTest.SHORT5 );
            testExcp5Import( msg, code, ConstsTest.INT5 );
            testExcp5Import( msg, code, ConstsTest.LONG5 );
            testExcp5Import( msg, code, ConstsTest.FLOAT5 );
            testExcp5Import( msg, code, ConstsTest.DOUBLE5 );
            testExcp5Import( msg, code, ConstsTest.STRING3 );

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
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_nothing );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_nothing );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_incr()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_incr );
		    checkField( ValueFactoryTest._mf_x );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_incr );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_sub()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_sub );
		    checkField( ValueFactoryTest._mf_x );
		    checkField( ValueFactoryTest._mf_y );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_sub );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_sum()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_sum );
		    checkField( ValueFactoryTest._mf_x );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_sum );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_trans()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_trans );
		    checkField( ValueFactoryTest._mf_e );
		    checkField( ValueFactoryTest._mf_x );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_trans );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_dist()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_dist );
		    checkField( ValueFactoryTest._mf_a );
		    checkField( ValueFactoryTest._mf_b );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_dist );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_fill()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_fill );
		    checkField( ValueFactoryTest._mf_n );
		    checkField( ValueFactoryTest._mf_x );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_fill );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
        [Test]
	    public void test_method_fillObject()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_fillObject );
		    checkField( ValueFactoryTest._mf_n );
		    checkField( ValueFactoryTest._mf_o );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_fillObject );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	

	    [Test]
	    public void test_method_blow()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_blow );
		    checkField( ValueFactoryTest._mf_msg );
		    checkField( ValueFactoryTest._mf_code );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_blow );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_beets()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_beets );
		    checkField( ValueFactoryTest._mf_e );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_beets );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
        [Test]
	    public void test_method_throwExcp5()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_throwExcp5 );
		    checkField( ValueFactoryTest._mf_msg );
		    checkField( ValueFactoryTest._mf_code );
		    checkField( ValueFactoryTest._mf_value );

            checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_throwExcp5 );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_throwExcp6()
	    {
            checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_throwExcp5 );
		    checkField( ValueFactoryTest._mf_msg );
		    checkField( ValueFactoryTest._mf_code );
		    checkField( ValueFactoryTest._mf_value );

            checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_throwExcp6 );
		    checkField( ValueFactoryTest._mf_result );
	    }

	    [Test]
	    public void test_method_p_boolean()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_boolean );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_boolean );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_boolean_array()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_boolean_array );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_boolean_array );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_byte()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_byte );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_byte );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_byte_array()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_byte_array );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_byte_array );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_short()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_short );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_short );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_short_array()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_short_array );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_short_array );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_int()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_int );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_int );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_int_array()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_int_array );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_int_array );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_long()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_long );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_long );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_long_array()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_long_array );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_long_array );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_float()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_float );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_float );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_float_array()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_float_array );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_float_array );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_double()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_double );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_double );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_double_array()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_double_array );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_double_array );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_string()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_string );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_string );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_string_array()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_string_array );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_string_array );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_E1()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_E1 );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_E1 );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_E1_array()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_E1_array );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_E1_array );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_S1()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_S1 );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_S1 );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_S1_array()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_S1_array );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_S1_array );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_S2()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_S2 );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_S2 );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_S2_array()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_S2_array );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_S2_array );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_Blob()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_Blob );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_Blob );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_Blob_array()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_Blob_array );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_Blob_array );
		    checkField( ValueFactoryTest._mf_result );
	    }

        [Test]
	    public void test_method_p_object()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_object );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_object );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_object_array()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_object_array );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_object_array );
		    checkField( ValueFactoryTest._mf_result );
	    }
    	
	    [Test]
	    public void test_method_p_object_struct()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_object_struct );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_object_struct );
		    checkField( ValueFactoryTest._mf_result );
	    }

	    [Test]
	    public void test_method_p_object_struct_array()
	    {
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_p_object_struct_array );
		    checkField( ValueFactoryTest._mf_a );
    		
		    checkType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test__result_p_object_struct_array );
		    checkField( ValueFactoryTest._mf_result );
	    }

	    /////////////////////
	    // UTILITY METHODS //
	    /////////////////////

	    private void checkType( XType t )
	    {
		    Assert.IsNotNull( t );
		    Assert.AreEqual( typeof(XType), t.GetType() );
		    Assert.AreEqual( t, vf.GetType( t.Id ) );
	    }
    	
	    private void checkField( Field f )
	    {
		    Assert.IsNotNull( f );
            Assert.AreEqual( typeof( Field ), f.GetType() );
		    Assert.AreEqual( f, vf.GetField( f.Id ) );
	    }

	    private void testEnumExport( etch.bindings.csharp.compiler.test.ConstsTest.E1 e, XType t, Field f )
	    {
		    StructValue sv = vf.ExportCustomValue( e );
		    sv.CheckType( t );
		    Assert.AreEqual( 1, sv.Count );
		    Assert.IsTrue( (Boolean) sv.Get( f ) );
	    }
    	
	    private void testEnumImport( etch.bindings.csharp.compiler.test.ConstsTest.E1 e, XType t, Field f )
	    {
		    StructValue sv = new StructValue( t );
		    sv.Add( f, true );
		    etch.bindings.csharp.compiler.test.ConstsTest.E1 a = (etch.bindings.csharp.compiler.test.ConstsTest.E1) vf.ImportCustomValue( sv );
		    Assert.AreEqual( e, a );
	    }

        private void testS3Export( String s, Object value )
        {
            StructValue sv = vf.ExportCustomValue( new etch.bindings.csharp.compiler.test.ConstsTest.S3( s, value ) );
            sv.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_S3 );
            Assert.AreEqual( 2, sv.Count );
            Assert.AreEqual( s, sv[ ValueFactoryTest._mf_type ] );
            Assert.AreEqual( value, sv[ ValueFactoryTest._mf_x ] );
        }

        private void testS3Import( String s, Object value )
        {
            StructValue sv = new StructValue( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_S3 );
            sv.Add( ValueFactoryTest._mf_type, s );
            sv.Add( ValueFactoryTest._mf_x, value );
            etch.bindings.csharp.compiler.test.ConstsTest.S3 myS3 = ( etch.bindings.csharp.compiler.test.ConstsTest.S3 ) vf.ImportCustomValue( sv );
            Assert.AreEqual( s, myS3.type );
            Assert.AreEqual( value, myS3.x );
        }

        private void testS4Export( String s, Object[] value )
        {
            StructValue sv = vf.ExportCustomValue( new etch.bindings.csharp.compiler.test.ConstsTest.S4( s, value ) );
            sv.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_S4 );
            Assert.AreEqual( 2, sv.Count );
            Assert.AreEqual( s, sv[ ValueFactoryTest._mf_type] );
            Assert.AreEqual( value, sv[ ValueFactoryTest._mf_x ] );
        }

        private void testS4Import( String s, Object[] value )
        {
            StructValue sv = new StructValue( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_S4 );
            sv.Add( ValueFactoryTest._mf_type, s );
            sv.Add( ValueFactoryTest._mf_x, value );
            etch.bindings.csharp.compiler.test.ConstsTest.S4 myS4 = ( etch.bindings.csharp.compiler.test.ConstsTest.S4 ) vf.ImportCustomValue( sv );
            Assert.AreEqual( s, myS4.type );
            Assert.AreEqual( value, myS4.x );
        }

        private void testExcp5Export( String msg, int code, Object value )
        {
            StructValue sv = vf.ExportCustomValue( new etch.bindings.csharp.compiler.test.ConstsTest.Excp5( msg, code, value ) );
            sv.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp5 );
            Assert.AreEqual( 3, sv.Count );
            Assert.AreEqual( msg, sv[ValueFactoryTest._mf_msg] );
            Assert.AreEqual( code, sv[ ValueFactoryTest._mf_code] );
            Assert.AreEqual( value, sv[ ValueFactoryTest._mf_x ] );
        }

        private void testExcp5Import( String msg, int code, Object value )
        {
            StructValue sv = new StructValue( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp5 );
            sv.Add( ValueFactoryTest._mf_msg, msg );
            sv.Add( ValueFactoryTest._mf_code, code );
            sv.Add( ValueFactoryTest._mf_x, value );
            etch.bindings.csharp.compiler.test.ConstsTest.Excp5 e = ( etch.bindings.csharp.compiler.test.ConstsTest.Excp5 ) vf.ImportCustomValue( sv );
            Assert.AreEqual( msg, e.msg );
            Assert.AreEqual( code, e.code );
            Assert.AreEqual( value, e.x );
        }

        private void testExcp6Export( String msg, int code, Object[] value )
        {
            StructValue sv = vf.ExportCustomValue( new etch.bindings.csharp.compiler.test.ConstsTest.Excp6( msg, code, value ) );
            sv.CheckType( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp6 );
            Assert.AreEqual( 3, sv.Count );
            Assert.AreEqual( msg, sv[ ValueFactoryTest._mf_msg ] );
            Assert.AreEqual( code, sv[ ValueFactoryTest._mf_code]  );
            Assert.AreEqual( value, sv[ ValueFactoryTest._mf_x ] );
        }

        private void testExcp6Import( String msg, int code, Object[] value )
        {
            StructValue sv = new StructValue( ValueFactoryTest._mt_etch_bindings_csharp_compiler_test_Test_Excp6 );
            sv.Add( ValueFactoryTest._mf_msg, msg );
            sv.Add( ValueFactoryTest._mf_code, code );
            sv.Add( ValueFactoryTest._mf_x, value );
            etch.bindings.csharp.compiler.test.ConstsTest.Excp6 e = ( etch.bindings.csharp.compiler.test.ConstsTest.Excp6 ) vf.ImportCustomValue( sv );
            Assert.AreEqual( msg, e.msg );
            Assert.AreEqual( code, e.code );
            Assert.AreEqual( value, e.x );
        }
    }
}

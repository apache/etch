/* $Id$
 *
 * Created by wert on Jun 8, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.compiler.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;
import etch.bindings.java.compiler.test.Test.E1;
import etch.bindings.java.compiler.test.Test.Excp1;
import etch.bindings.java.compiler.test.Test.Excp2;
import etch.bindings.java.compiler.test.Test.Excp3;
import etch.bindings.java.compiler.test.Test.Excp4;
import etch.bindings.java.compiler.test.Test.Excp5;
import etch.bindings.java.compiler.test.Test.Excp6;
import etch.bindings.java.compiler.test.Test.S1;
import etch.bindings.java.compiler.test.Test.S2;
import etch.bindings.java.compiler.test.Test.S3;
import etch.bindings.java.compiler.test.Test.S4;
import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;

public class TestValueFactoryTestDotJava
{
	private ValueFactoryTest vf = new ValueFactoryTest();
	
	@org.junit.Test
	public void test_E1()
	{
		// type //
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_E1 );
		
		// fields //
		
		checkField( ValueFactoryTest._mf_A );
		checkField( ValueFactoryTest._mf_B );
		checkField( ValueFactoryTest._mf_C );
	}
	
	@org.junit.Test
	public void test_E1_export()
	{
		testEnumExport( E1.A,
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_E1,
			ValueFactoryTest._mf_A );
		
		testEnumExport( E1.B,
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_E1,
			ValueFactoryTest._mf_B );
		
		testEnumExport( E1.C,
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_E1,
			ValueFactoryTest._mf_C );
	}
	
	@org.junit.Test
	public void test_E1_import()
	{
		testEnumImport( E1.A,
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_E1,
			ValueFactoryTest._mf_A );
		
		testEnumImport( E1.B,
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_E1,
			ValueFactoryTest._mf_B );
		
		testEnumImport( E1.C,
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_E1,
			ValueFactoryTest._mf_C );
	}

	@org.junit.Test
	public void test_S1()
	{
		// type //

		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_S1 );
		
		// fields //
		
		checkField( ValueFactoryTest._mf_x );
		checkField( ValueFactoryTest._mf_y );
		checkField( ValueFactoryTest._mf_z );
	}
	
	@org.junit.Test
	public void test_S1_export()
	{
		StructValue sv = vf.exportCustomValue( new S1( 19, 23, 29 ) );
		sv.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_S1 );
		assertEquals( 3, sv.size() );
		assertEquals( 19, sv.get( ValueFactoryTest._mf_x ) );
		assertEquals( 23, sv.get( ValueFactoryTest._mf_y ) );
		assertEquals( 29, sv.get( ValueFactoryTest._mf_z ) );
	}
	
	@org.junit.Test
	public void test_S1_import()
	{
		StructValue sv = new StructValue( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_S1 );
		sv.put( ValueFactoryTest._mf_x, 101 );
		sv.put( ValueFactoryTest._mf_y, 103 );
		sv.put( ValueFactoryTest._mf_z, 107 );
		S1 s = (S1) vf.importCustomValue( sv );
		assertEquals( 101, s.x );
		assertEquals( 103, s.y );
		assertEquals( 107, s.z );
	}

	@org.junit.Test
	public void test_S2()
	{
		// type //

		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_S2 );
		
		// fields //
		
		checkField( ValueFactoryTest._mf_a );
		checkField( ValueFactoryTest._mf_b );
		checkField( ValueFactoryTest._mf_c );
	}
	
	@org.junit.Test
	public void test_S2_export()
	{
		S1 a = new S1( 21, 22, 23 );
		S1 b = new S1( 31, 32, 33 );
		E1 c = E1.A;
		
		StructValue sv = vf.exportCustomValue( new S2( a, b, c ) );
		sv.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_S2 );
		assertEquals( 3, sv.size() );
		
		assertSame( a, sv.get( ValueFactoryTest._mf_a ) );
		assertSame( b, sv.get( ValueFactoryTest._mf_b ) );
		assertSame( c, sv.get( ValueFactoryTest._mf_c ) );
	}
	
	@org.junit.Test
	public void test_S2_import()
	{
		StructValue sv = new StructValue( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_S2 );
		
		sv.put( ValueFactoryTest._mf_a, new S1( 21, 22, 23 ) );
		sv.put( ValueFactoryTest._mf_b, new S1( 31, 32, 33 ) );
		sv.put( ValueFactoryTest._mf_c, E1.A );
		
		S2 s = (S2) vf.importCustomValue( sv );
		assertEquals( 21, s.a.x );
		assertEquals( 22, s.a.y );
		assertEquals( 23, s.a.z );
		assertEquals( 31, s.b.x );
		assertEquals( 32, s.b.y );
		assertEquals( 33, s.b.z );
		assertEquals( E1.A, s.c );
	}
	
	@org.junit.Test
	public void test_S3()
	{
		// type //

		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_S3 );
		
		// fields //
		checkField( ValueFactoryTest._mf_type );
		checkField( ValueFactoryTest._mf_x );
	}
	
	@org.junit.Test
	public void test_S3_export()
	{		
		testS3Export("boolean", Test.BOOL1);
		testS3Export("byte", Test.BYTE5);
		testS3Export("short", Test.SHORT5);
		testS3Export("int", Test.INT5);
		testS3Export("long", Test.LONG5);
		testS3Export("float", Test.FLOAT5);
		testS3Export("double", Test.DOUBLE5);
		testS3Export("string", Test.STRING3);
	}
	
	@org.junit.Test
	public void test_S3_import()
	{
		testS3Import("boolean", Test.BOOL1);
		testS3Import("byte", Test.BYTE5);
		testS3Import("short", Test.SHORT5);
		testS3Import("int", Test.INT5);
		testS3Import("long", Test.LONG5);
		testS3Import("float", Test.FLOAT5);
		testS3Import("double", Test.DOUBLE5);
		testS3Import("string", Test.STRING3);
	}
	
	@org.junit.Test
	public void test_S4()
	{
		// type //

		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_S4 );
		
		// fields //
		checkField( ValueFactoryTest._mf_type );
		checkField( ValueFactoryTest._mf_x );
	}
	
	@org.junit.Test
	public void test_S4_export()
	{	
		Object[] boolObject = new Object[] {Test.BOOL1, Test.BOOL2};
		Object[] byteObject = new Object[] {Test.BYTE1, Test.BYTE2, Test.BYTE3, Test.BYTE4, Test.BYTE5};
		Object[] shortObject = new Object[] {Test.SHORT1, Test.SHORT2, Test.SHORT3, Test.SHORT4, Test.SHORT5};
		Object[] intObject = new Object[]{Test.INT1, Test.INT2, Test.INT3, Test.INT4, Test.INT5};
		Object[] longObject = new Object[] {Test.LONG1, Test.LONG2, Test.LONG3, Test.LONG4, Test.LONG5};
		Object[] floatObject = new Object[]{Test.FLOAT1, Test.FLOAT2, Test.FLOAT3, Test.FLOAT4, Test.FLOAT5};
		Object[] doubleObject = new Object[] {Test.DOUBLE1, Test.DOUBLE2, Test.DOUBLE3, Test.DOUBLE4, Test.DOUBLE5};
		Object[] stringObject = new Object []{Test.STRING1, Test.STRING2, Test.STRING3, Test.STRING4, Test.STRING5};

		testS4Export("boolean", boolObject);
		testS4Export("byte", byteObject);
		testS4Export("short", shortObject);
		testS4Export("int", intObject);
		testS4Export("long", longObject);
		testS4Export("float", floatObject);
		testS4Export("double", doubleObject);
		testS4Export("string", stringObject);
	}
	
	@org.junit.Test
	public void test_S4_import()
	{
		Object[] boolObject = new Object[] {Test.BOOL1, Test.BOOL2};
		Object[] byteObject = new Object[] {Test.BYTE1, Test.BYTE2, Test.BYTE3, Test.BYTE4, Test.BYTE5};
		Object[] shortObject = new Object[] {Test.SHORT1, Test.SHORT2, Test.SHORT3, Test.SHORT4, Test.SHORT5};
		Object[] intObject = new Object[]{Test.INT1, Test.INT2, Test.INT3, Test.INT4, Test.INT5};
		Object[] longObject = new Object[] {Test.LONG1, Test.LONG2, Test.LONG3, Test.LONG4, Test.LONG5};
		Object[] floatObject = new Object[]{Test.FLOAT1, Test.FLOAT2, Test.FLOAT3, Test.FLOAT4, Test.FLOAT5};
		Object[] doubleObject = new Object[] {Test.DOUBLE1, Test.DOUBLE2, Test.DOUBLE3, Test.DOUBLE4, Test.DOUBLE5};
		Object[] stringObject = new Object []{Test.STRING1, Test.STRING2, Test.STRING3, Test.STRING4, Test.STRING5};

		testS4Import("boolean", boolObject);
		testS4Import("byte", byteObject);
		testS4Import("short", shortObject);
		testS4Import("int", intObject);
		testS4Import("long", longObject);
		testS4Import("float", floatObject);
		testS4Import("double", doubleObject);
		testS4Import("string", stringObject);
	}	

	@org.junit.Test
	public void test_excps()
	{
		// type //
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp1 );
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp2 );
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp3 );
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp4 );
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp5 );
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp6 );
		
		// fields //
		
		checkField( ValueFactoryTest._mf_msg );
		checkField( ValueFactoryTest._mf_code );
		checkField( ValueFactoryTest._mf_x );	
	}
	
	@org.junit.Test
	public void test_excps_export()
	{
		Object[] boolObject = new Object[] {Test.BOOL1, Test.BOOL2};
		Object[] byteObject = new Object[] {Test.BYTE1, Test.BYTE2, Test.BYTE3, Test.BYTE4, Test.BYTE5};
		Object[] shortObject = new Object[] {Test.SHORT1, Test.SHORT2, Test.SHORT3, Test.SHORT4, Test.SHORT5};
		Object[] intObject = new Object[]{Test.INT1, Test.INT2, Test.INT3, Test.INT4, Test.INT5};
		Object[] longObject = new Object[] {Test.LONG1, Test.LONG2, Test.LONG3, Test.LONG4, Test.LONG5};
		Object[] floatObject = new Object[]{Test.FLOAT1, Test.FLOAT2, Test.FLOAT3, Test.FLOAT4, Test.FLOAT5};
		Object[] doubleObject = new Object[] {Test.DOUBLE1, Test.DOUBLE2, Test.DOUBLE3, Test.DOUBLE4, Test.DOUBLE5};
		Object[] stringObject = new Object []{Test.STRING1, Test.STRING2, Test.STRING3, Test.STRING4, Test.STRING5};

		String msg = "Exception";
		int code = 500;
		
		StructValue sv = vf.exportCustomValue( new Excp1( "abc", 23 ) );
		sv.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp1 );
		assertEquals( 2, sv.size() );
		assertEquals( "abc", sv.get( ValueFactoryTest._mf_msg ) );
		assertEquals( 23, sv.get( ValueFactoryTest._mf_code ) );
		
		sv = vf.exportCustomValue( new Excp2() );
		sv.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp2 );
		assertEquals( 0, sv.size() );
		
		sv = vf.exportCustomValue( new Excp3() );
		sv.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp3 );
		assertEquals( 0, sv.size() );
		
		sv = vf.exportCustomValue( new Excp4() );
		sv.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp4 );
		assertEquals( 0, sv.size() );
		
		// Import exception with object as param
		testExcp5Export(msg, code, Test.BOOL2);
		testExcp5Export(msg, code, Test.BYTE5);
		testExcp5Export(msg, code, Test.SHORT5);
		testExcp5Export(msg, code, Test.INT5);
		testExcp5Export(msg, code, Test.LONG5);
		testExcp5Export(msg, code, Test.FLOAT5);
		testExcp5Export(msg, code, Test.DOUBLE5);
		testExcp5Export(msg, code, Test.STRING3);	
		
		// Import exception with array of object as param
		testExcp6Export(msg, code, boolObject);
		testExcp6Export(msg, code, byteObject);
		testExcp6Export(msg, code, shortObject);
		testExcp6Export(msg, code, intObject);
		testExcp6Export(msg, code, longObject);
		testExcp6Export(msg, code, floatObject);
		testExcp6Export(msg, code, doubleObject);
		testExcp6Export(msg, code, stringObject);	
	}
	
	@org.junit.Test
	public void test_excps_import()
	{
		Object[] boolObject = new Object[] {Test.BOOL1, Test.BOOL2};
		Object[] byteObject = new Object[] {Test.BYTE1, Test.BYTE2, Test.BYTE3, Test.BYTE4, Test.BYTE5};
		Object[] shortObject = new Object[] {Test.SHORT1, Test.SHORT2, Test.SHORT3, Test.SHORT4, Test.SHORT5};
		Object[] intObject = new Object[]{Test.INT1, Test.INT2, Test.INT3, Test.INT4, Test.INT5};
		Object[] longObject = new Object[] {Test.LONG1, Test.LONG2, Test.LONG3, Test.LONG4, Test.LONG5};
		Object[] floatObject = new Object[]{Test.FLOAT1, Test.FLOAT2, Test.FLOAT3, Test.FLOAT4, Test.FLOAT5};
		Object[] doubleObject = new Object[] {Test.DOUBLE1, Test.DOUBLE2, Test.DOUBLE3, Test.DOUBLE4, Test.DOUBLE5};
		Object[] stringObject = new Object []{Test.STRING1, Test.STRING2, Test.STRING3, Test.STRING4, Test.STRING5};

		String msg = "Exception";
		int code = 500;
		
		StructValue sv = new StructValue( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp1 );
		sv.put( ValueFactoryTest._mf_msg, "def" );
		sv.put( ValueFactoryTest._mf_code, 29 );
		Excp1 e1 = (Excp1) vf.importCustomValue( sv );
		assertEquals( "def", e1.msg );
		assertEquals( 29, e1.code );
		e1 = null;
		
		sv = new StructValue( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp2 );
		Excp2 e2 = (Excp2) vf.importCustomValue( sv );
		assertNotNull( e2 );
		e2 = null;
		
		sv = new StructValue( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp3 );
		Excp3 e3 = (Excp3) vf.importCustomValue( sv );
		assertNotNull( e3 );
		e3 = null;
		
		sv = new StructValue( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp4 );
		Excp4 e4 = (Excp4) vf.importCustomValue( sv );
		assertNotNull( e4 );
		e4 = null;
		
		// Import exception with object as param
		testExcp5Import(msg, code, Test.BOOL2);
		testExcp5Import(msg, code, Test.BYTE5);
		testExcp5Import(msg, code, Test.SHORT5);
		testExcp5Import(msg, code, Test.INT5);
		testExcp5Import(msg, code, Test.LONG5);
		testExcp5Import(msg, code, Test.FLOAT5);
		testExcp5Import(msg, code, Test.DOUBLE5);
		testExcp5Import(msg, code, Test.STRING3);	
		
		// Import exception with array of object as param
		testExcp6Import(msg, code, boolObject);
		testExcp6Import(msg, code, byteObject);
		testExcp6Import(msg, code, shortObject);
		testExcp6Import(msg, code, intObject);
		testExcp6Import(msg, code, longObject);
		testExcp6Import(msg, code, floatObject);
		testExcp6Import(msg, code, doubleObject);
		testExcp6Import(msg, code, stringObject);	
	}
	
	@org.junit.Test
	public void test_method_nothing()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_nothing );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_nothing );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_incr()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_incr );
		checkField( ValueFactoryTest._mf_x );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_incr );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_sub()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_sub );
		checkField( ValueFactoryTest._mf_x );
		checkField( ValueFactoryTest._mf_y );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_sub );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_sum()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_sum );
		checkField( ValueFactoryTest._mf_x );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_sum );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_trans()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_trans );
		checkField( ValueFactoryTest._mf_e );
		checkField( ValueFactoryTest._mf_x );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_trans );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_dist()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_dist );
		checkField( ValueFactoryTest._mf_a );
		checkField( ValueFactoryTest._mf_b );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_dist );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_fill()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_fill );
		checkField( ValueFactoryTest._mf_n );
		checkField( ValueFactoryTest._mf_x );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_fill );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_fillObject()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_fillObject );
		checkField( ValueFactoryTest._mf_n );
		checkField( ValueFactoryTest._mf_o );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_fillObject );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_blow()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_blow );
		checkField( ValueFactoryTest._mf_msg );
		checkField( ValueFactoryTest._mf_code );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_blow );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_beets()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_beets );
		checkField( ValueFactoryTest._mf_e );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_beets );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_throwExcp5()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_throwExcp5 );
		checkField( ValueFactoryTest._mf_msg );
		checkField( ValueFactoryTest._mf_code );
		checkField( ValueFactoryTest._mf_value );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_throwExcp5 );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_throwExcp6()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_throwExcp5 );
		checkField( ValueFactoryTest._mf_msg );
		checkField( ValueFactoryTest._mf_code );
		checkField( ValueFactoryTest._mf_value );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_throwExcp6 );
		checkField( ValueFactoryTest._mf_result );
	}

	@org.junit.Test
	public void test_method_p_boolean()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_boolean );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_boolean );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_boolean_array()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_boolean_array );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_boolean_array );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_byte()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_byte );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_byte );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_byte_array()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_byte_array );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_byte_array );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_short()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_short );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_short );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_short_array()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_short_array );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_short_array );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_int()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_int );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_int );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_int_array()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_int_array );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_int_array );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_long()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_long );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_long );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_long_array()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_long_array );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_long_array );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_float()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_float );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_float );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_float_array()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_float_array );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_float_array );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_double()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_double );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_double );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_double_array()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_double_array );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_double_array );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_string()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_string );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_string );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_string_array()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_string_array );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_string_array );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_E1()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_E1 );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_E1 );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_E1_array()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_E1_array );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_E1_array );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_S1()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_S1 );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_S1 );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_S1_array()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_S1_array );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_S1_array );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_S2()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_S2 );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_S2 );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_S2_array()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_S2_array );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_S2_array );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_Blob()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_Blob );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_Blob );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_Blob_array()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_Blob_array );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_Blob_array );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_object()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_object );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_object );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_object_array()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_object_array );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_object_array );
		checkField( ValueFactoryTest._mf_result );
	}
	
	@org.junit.Test
	public void test_method_p_object_struct()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_object_struct );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_object_struct );
		checkField( ValueFactoryTest._mf_result );
	}

	@org.junit.Test
	public void test_method_p_object_struct_array()
	{
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_p_object_struct_array );
		checkField( ValueFactoryTest._mf_a );
		
		checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_p_object_struct_array );
		checkField( ValueFactoryTest._mf_result );
	}
	
	/////////////////////
	// UTILITY METHODS //
	/////////////////////
	private void checkType( Type t )
	{
		assertNotNull( t );
		assertSame( Type.class, t.getClass() );
		assertSame( t, vf.getType( t.getId() ) );
	}
	
	private void checkField( Field f )
	{
		assertNotNull( f );
		assertSame( Field.class, f.getClass() );
		assertSame( f, vf.getField( f.getId() ) );
	}

	private void testEnumExport( E1 e, Type t, Field f )
	{
		StructValue sv = vf.exportCustomValue( e );
		sv.checkType( t );
		assertEquals( 1, sv.size() );
		assertTrue( (Boolean) sv.get( f ) );
	}
	
	private void testEnumImport( E1 e, Type t, Field f )
	{
		StructValue sv = new StructValue( t );
		sv.put(  f, true );
		E1 a = (E1) vf.importCustomValue( sv );
		assertSame( e, a );
	}
	
	private void testS3Export(String s, Object value)
	{
		StructValue sv = vf.exportCustomValue( new S3( s,value ) );
		sv.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_S3 );
		assertEquals( 2, sv.size() );
		assertEquals( s, sv.get( ValueFactoryTest._mf_type ) );
		assertEquals( value, sv.get( ValueFactoryTest._mf_x ) );
	}
	
	private void testS3Import(String s, Object value)
	{
		StructValue sv = new StructValue( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_S3 );
		sv.put( ValueFactoryTest._mf_type, s );
		sv.put( ValueFactoryTest._mf_x, value );
		S3 myS3 = (S3) vf.importCustomValue( sv );
		assertEquals( s, myS3.type );
		assertEquals( value, myS3.x );
	}	
	
	private void testS4Export(String s, Object[] value)
	{
		StructValue sv = vf.exportCustomValue( new S4( s,value ) );
		sv.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_S4 );
		assertEquals( 2, sv.size() );
		assertEquals( s, sv.get( ValueFactoryTest._mf_type ) );
		assertEquals( value, sv.get( ValueFactoryTest._mf_x ) );
	}
	
	private void testS4Import(String s, Object[] value)
	{
		StructValue sv = new StructValue( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_S4 );
		sv.put( ValueFactoryTest._mf_type, s );
		sv.put( ValueFactoryTest._mf_x, value );
		S4 myS4 = (S4) vf.importCustomValue( sv );
		assertEquals( s, myS4.type );
		assertEquals( value, myS4.x );
	}
	
	private void testExcp5Export(String msg, int code, Object value)
	{
		StructValue sv = vf.exportCustomValue( new Excp5( msg, code, value ) );
		sv.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp5 );
		assertEquals( 3, sv.size() );
		assertEquals( msg, sv.get( ValueFactoryTest._mf_msg ) );
		assertEquals( code, sv.get( ValueFactoryTest._mf_code ) );
		assertEquals( value, sv.get( ValueFactoryTest._mf_x ) );		
	}
	
	private void testExcp5Import(String msg, int code, Object value)
	{
		StructValue sv = new StructValue( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp5 );
		sv.put( ValueFactoryTest._mf_msg, msg );
		sv.put( ValueFactoryTest._mf_code, code );
		sv.put( ValueFactoryTest._mf_x, value );		
		Excp5 e = (Excp5) vf.importCustomValue( sv );
		assertEquals( msg, e.msg );
		assertEquals( code, e.code );
		assertEquals( value, e.x );		
	}	
	
	private void testExcp6Export(String msg, int code, Object[] value)
	{
		StructValue sv = vf.exportCustomValue( new Excp6( msg, code, value ) );
		sv.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp6 );
		assertEquals( 3, sv.size() );
		assertEquals( msg, sv.get( ValueFactoryTest._mf_msg ) );
		assertEquals( code, sv.get( ValueFactoryTest._mf_code ) );
		assertEquals( value, sv.get( ValueFactoryTest._mf_x ) );		
	}
	
	private void testExcp6Import(String msg, int code, Object[] value)
	{
		StructValue sv = new StructValue( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_Excp6 );
		sv.put( ValueFactoryTest._mf_msg, msg );
		sv.put( ValueFactoryTest._mf_code, code );
		sv.put( ValueFactoryTest._mf_x, value );		
		Excp6 e = (Excp6) vf.importCustomValue( sv );
		assertEquals( msg, e.msg );
		assertEquals( code, e.code );
		assertEquals( value, e.x );		
	}
}

/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.tests;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import java.util.Set;

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.tests.Test1.E1;
import org.apache.etch.tests.Test1.Excp1;
import org.apache.etch.tests.Test1.Excp2;
import org.apache.etch.tests.Test1.Excp3;
import org.apache.etch.tests.Test1.Excp4;
import org.apache.etch.tests.Test1.Excp5;
import org.apache.etch.tests.Test1.Excp6;
import org.apache.etch.tests.Test1.S1;
import org.apache.etch.tests.Test1.S2;
import org.apache.etch.tests.Test1.S3;
import org.apache.etch.tests.Test1.S4;


/** Test of ValueFactoryTest1 */
public class TestValueFactoryTest1DotJava
{
	private ValueFactoryTest1 vf = new ValueFactoryTest1( "tcp:" );
	
	/** */
	@org.junit.Test
	public void test_E1()
	{
		// type //
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_E1,
		
		// fields //
		
		ValueFactoryTest1._mf_A,
		ValueFactoryTest1._mf_B,
		ValueFactoryTest1._mf_C );
	}
	
	/** */
	@org.junit.Test
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
	
	/** */
	@org.junit.Test
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

	/** */
	@org.junit.Test
	public void test_S1()
	{
		// type //

		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S1,
		
		// fields //
		
		ValueFactoryTest1._mf_x,
		ValueFactoryTest1._mf_y,
		ValueFactoryTest1._mf_z );
	}
	
	/** */
	@org.junit.Test
	public void test_S1_export()
	{
		StructValue sv = vf.exportCustomValue( new S1( 19, 23, 29 ) );
		sv.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S1 );
		assertEquals( 3, sv.size() );
		assertEquals( 19, sv.get( ValueFactoryTest1._mf_x ) );
		assertEquals( 23, sv.get( ValueFactoryTest1._mf_y ) );
		assertEquals( 29, sv.get( ValueFactoryTest1._mf_z ) );
	}
	
	/** */
	@org.junit.Test
	public void test_S1_import()
	{
		StructValue sv = new StructValue( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S1, vf );
		sv.put( ValueFactoryTest1._mf_x, 101 );
		sv.put( ValueFactoryTest1._mf_y, 103 );
		sv.put( ValueFactoryTest1._mf_z, 107 );
		S1 s = (S1) vf.importCustomValue( sv );
		assertEquals( 101, s.x );
		assertEquals( 103, s.y );
		assertEquals( 107, s.z );
	}

	/** */
	@org.junit.Test
	public void test_S2()
	{
		// type //

		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S2,
		
		// fields //
		
		ValueFactoryTest1._mf_a,
		ValueFactoryTest1._mf_b,
		ValueFactoryTest1._mf_c );
	}
	
	/** */
	@org.junit.Test
	public void test_S2_export()
	{
		S1 a = new S1( 21, 22, 23 );
		S1 b = new S1( 31, 32, 33 );
		E1 c = E1.A;
		
		StructValue sv = vf.exportCustomValue( new S2( a, b, c ) );
		sv.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S2 );
		assertEquals( 3, sv.size() );
		
		assertSame( a, sv.get( ValueFactoryTest1._mf_a ) );
		assertSame( b, sv.get( ValueFactoryTest1._mf_b ) );
		assertSame( c, sv.get( ValueFactoryTest1._mf_c ) );
	}
	
	/** */
	@org.junit.Test
	public void test_S2_import()
	{
		StructValue sv = new StructValue( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S2, vf );
		
		sv.put( ValueFactoryTest1._mf_a, new S1( 21, 22, 23 ) );
		sv.put( ValueFactoryTest1._mf_b, new S1( 31, 32, 33 ) );
		sv.put( ValueFactoryTest1._mf_c, E1.A );
		
		S2 s = (S2) vf.importCustomValue( sv );
		assertEquals( 21, s.a.x );
		assertEquals( 22, s.a.y );
		assertEquals( 23, s.a.z );
		assertEquals( 31, s.b.x );
		assertEquals( 32, s.b.y );
		assertEquals( 33, s.b.z );
		assertEquals( E1.A, s.c );
	}
	
	/** */
	@org.junit.Test
	public void test_S3()
	{
		// type //

		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S3,
		
		// fields //
		
		ValueFactoryTest1._mf_tipe,
		ValueFactoryTest1._mf_x );
	}
	
	/** */
	@org.junit.Test
	public void test_S3_export()
	{		
		testS3Export("boolean", Test1.BOOL1);
		testS3Export("byte", Test1.BYTE5);
		testS3Export("short", Test1.SHORT5);
		testS3Export("int", Test1.INT5);
		testS3Export("long", Test1.LONG5);
		testS3Export("float", Test1.FLOAT5);
		testS3Export("double", Test1.DOUBLE5);
		testS3Export("string", Test1.STRING3);
	}
	
	/** */
	@org.junit.Test
	public void test_S3_import()
	{
		testS3Import("boolean", Test1.BOOL1);
		testS3Import("byte", Test1.BYTE5);
		testS3Import("short", Test1.SHORT5);
		testS3Import("int", Test1.INT5);
		testS3Import("long", Test1.LONG5);
		testS3Import("float", Test1.FLOAT5);
		testS3Import("double", Test1.DOUBLE5);
		testS3Import("string", Test1.STRING3);
	}
	
	/** */
	@org.junit.Test
	public void test_S4()
	{
		// type //

		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S4,
		
		// fields //
		ValueFactoryTest1._mf_tipe,
		ValueFactoryTest1._mf_x );
	}
	
	/** */
	@org.junit.Test
	public void test_S4_export()
	{	
		Object[] boolObject = new Object[] {Test1.BOOL1, Test1.BOOL2};
		Object[] byteObject = new Object[] {Test1.BYTE1, Test1.BYTE2, Test1.BYTE3, Test1.BYTE4, Test1.BYTE5};
		Object[] shortObject = new Object[] {Test1.SHORT1, Test1.SHORT2, Test1.SHORT3, Test1.SHORT4, Test1.SHORT5};
		Object[] intObject = new Object[]{Test1.INT1, Test1.INT2, Test1.INT3, Test1.INT4, Test1.INT5};
		Object[] longObject = new Object[] {Test1.LONG1, Test1.LONG2, Test1.LONG3, Test1.LONG4, Test1.LONG5};
		Object[] floatObject = new Object[]{Test1.FLOAT1, Test1.FLOAT2, Test1.FLOAT3, Test1.FLOAT4, Test1.FLOAT5};
		Object[] doubleObject = new Object[] {Test1.DOUBLE1, Test1.DOUBLE2, Test1.DOUBLE3, Test1.DOUBLE4, Test1.DOUBLE5};
		Object[] stringObject = new Object []{Test1.STRING1, Test1.STRING2, Test1.STRING3, Test1.STRING4, Test1.STRING5};

		testS4Export("boolean", boolObject);
		testS4Export("byte", byteObject);
		testS4Export("short", shortObject);
		testS4Export("int", intObject);
		testS4Export("long", longObject);
		testS4Export("float", floatObject);
		testS4Export("double", doubleObject);
		testS4Export("string", stringObject);
	}
	
	/** */
	@org.junit.Test
	public void test_S4_import()
	{
		Object[] boolObject = new Object[] {Test1.BOOL1, Test1.BOOL2};
		Object[] byteObject = new Object[] {Test1.BYTE1, Test1.BYTE2, Test1.BYTE3, Test1.BYTE4, Test1.BYTE5};
		Object[] shortObject = new Object[] {Test1.SHORT1, Test1.SHORT2, Test1.SHORT3, Test1.SHORT4, Test1.SHORT5};
		Object[] intObject = new Object[]{Test1.INT1, Test1.INT2, Test1.INT3, Test1.INT4, Test1.INT5};
		Object[] longObject = new Object[] {Test1.LONG1, Test1.LONG2, Test1.LONG3, Test1.LONG4, Test1.LONG5};
		Object[] floatObject = new Object[]{Test1.FLOAT1, Test1.FLOAT2, Test1.FLOAT3, Test1.FLOAT4, Test1.FLOAT5};
		Object[] doubleObject = new Object[] {Test1.DOUBLE1, Test1.DOUBLE2, Test1.DOUBLE3, Test1.DOUBLE4, Test1.DOUBLE5};
		Object[] stringObject = new Object []{Test1.STRING1, Test1.STRING2, Test1.STRING3, Test1.STRING4, Test1.STRING5};

		testS4Import("boolean", boolObject);
		testS4Import("byte", byteObject);
		testS4Import("short", shortObject);
		testS4Import("int", intObject);
		testS4Import("long", longObject);
		testS4Import("float", floatObject);
		testS4Import("double", doubleObject);
		testS4Import("string", stringObject);
	}	
	
	/** */
	@org.junit.Test
	public void test_excps()
	{
		// type //
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp1,
			ValueFactoryTest1._mf_msg,
			ValueFactoryTest1._mf_code );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp2 );
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp3 );
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp4 );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp5,
			ValueFactoryTest1._mf_msg,
			ValueFactoryTest1._mf_code,
			ValueFactoryTest1._mf_x );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp6,
			ValueFactoryTest1._mf_msg,
			ValueFactoryTest1._mf_code,
			ValueFactoryTest1._mf_x );
		
		// fields //
		
//		ValueFactoryTest1._mf_msg );
//		ValueFactoryTest1._mf_code );
//		ValueFactoryTest1._mf_x );	
	}
	
	/** */
	@org.junit.Test
	public void test_excps_export()
	{
		Object[] boolObject = new Object[] {Test1.BOOL1, Test1.BOOL2};
		Object[] byteObject = new Object[] {Test1.BYTE1, Test1.BYTE2, Test1.BYTE3, Test1.BYTE4, Test1.BYTE5};
		Object[] shortObject = new Object[] {Test1.SHORT1, Test1.SHORT2, Test1.SHORT3, Test1.SHORT4, Test1.SHORT5};
		Object[] intObject = new Object[]{Test1.INT1, Test1.INT2, Test1.INT3, Test1.INT4, Test1.INT5};
		Object[] longObject = new Object[] {Test1.LONG1, Test1.LONG2, Test1.LONG3, Test1.LONG4, Test1.LONG5};
		Object[] floatObject = new Object[]{Test1.FLOAT1, Test1.FLOAT2, Test1.FLOAT3, Test1.FLOAT4, Test1.FLOAT5};
		Object[] doubleObject = new Object[] {Test1.DOUBLE1, Test1.DOUBLE2, Test1.DOUBLE3, Test1.DOUBLE4, Test1.DOUBLE5};
		Object[] stringObject = new Object []{Test1.STRING1, Test1.STRING2, Test1.STRING3, Test1.STRING4, Test1.STRING5};

		String msg = "Exception";
		int code = 500;
		
		StructValue sv = vf.exportCustomValue( new Excp1( "abc", 23 ) );
		sv.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp1 );
		assertEquals( 2, sv.size() );
		assertEquals( "abc", sv.get( ValueFactoryTest1._mf_msg ) );
		assertEquals( 23, sv.get( ValueFactoryTest1._mf_code ) );
		
		sv = vf.exportCustomValue( new Excp2() );
		sv.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp2 );
		assertEquals( 0, sv.size() );
		
		sv = vf.exportCustomValue( new Excp3() );
		sv.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp3 );
		assertEquals( 0, sv.size() );
		
		sv = vf.exportCustomValue( new Excp4() );
		sv.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp4 );
		assertEquals( 0, sv.size() );
		
		// Import exception with object as param
		testExcp5Export(msg, code, Test1.BOOL2);
		testExcp5Export(msg, code, Test1.BYTE5);
		testExcp5Export(msg, code, Test1.SHORT5);
		testExcp5Export(msg, code, Test1.INT5);
		testExcp5Export(msg, code, Test1.LONG5);
		testExcp5Export(msg, code, Test1.FLOAT5);
		testExcp5Export(msg, code, Test1.DOUBLE5);
		testExcp5Export(msg, code, Test1.STRING3);	
		
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
	
	/** */
	@org.junit.Test
	public void test_excps_import()
	{
		Object[] boolObject = new Object[] {Test1.BOOL1, Test1.BOOL2};
		Object[] byteObject = new Object[] {Test1.BYTE1, Test1.BYTE2, Test1.BYTE3, Test1.BYTE4, Test1.BYTE5};
		Object[] shortObject = new Object[] {Test1.SHORT1, Test1.SHORT2, Test1.SHORT3, Test1.SHORT4, Test1.SHORT5};
		Object[] intObject = new Object[]{Test1.INT1, Test1.INT2, Test1.INT3, Test1.INT4, Test1.INT5};
		Object[] longObject = new Object[] {Test1.LONG1, Test1.LONG2, Test1.LONG3, Test1.LONG4, Test1.LONG5};
		Object[] floatObject = new Object[]{Test1.FLOAT1, Test1.FLOAT2, Test1.FLOAT3, Test1.FLOAT4, Test1.FLOAT5};
		Object[] doubleObject = new Object[] {Test1.DOUBLE1, Test1.DOUBLE2, Test1.DOUBLE3, Test1.DOUBLE4, Test1.DOUBLE5};
		Object[] stringObject = new Object []{Test1.STRING1, Test1.STRING2, Test1.STRING3, Test1.STRING4, Test1.STRING5};

		String msg = "Exception";
		int code = 500;
		
		StructValue sv = new StructValue( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp1, vf );
		sv.put( ValueFactoryTest1._mf_msg, "def" );
		sv.put( ValueFactoryTest1._mf_code, 29 );
		Excp1 e1 = (Excp1) vf.importCustomValue( sv );
		assertEquals( "def", e1.msg );
		assertEquals( 29, e1.code );
		e1 = null;
		
		sv = new StructValue( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp2, vf );
		Excp2 e2 = (Excp2) vf.importCustomValue( sv );
		assertNotNull( e2 );
		e2 = null;
		
		sv = new StructValue( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp3, vf );
		Excp3 e3 = (Excp3) vf.importCustomValue( sv );
		assertNotNull( e3 );
		e3 = null;
		
		sv = new StructValue( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp4, vf );
		Excp4 e4 = (Excp4) vf.importCustomValue( sv );
		assertNotNull( e4 );
		e4 = null;
		
		// Import exception with object as param
		testExcp5Import(msg, code, Test1.BOOL2);
		testExcp5Import(msg, code, Test1.BYTE5);
		testExcp5Import(msg, code, Test1.SHORT5);
		testExcp5Import(msg, code, Test1.INT5);
		testExcp5Import(msg, code, Test1.LONG5);
		testExcp5Import(msg, code, Test1.FLOAT5);
		testExcp5Import(msg, code, Test1.DOUBLE5);
		testExcp5Import(msg, code, Test1.STRING3);	
		
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
	
	/** */
	@org.junit.Test
	public void test_method_nothing()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_nothing,
			ValueFactoryTest1._mf__messageId );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_nothing,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
			ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_incr()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_incr,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_x );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_incr,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_sub()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sub,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_x,
		ValueFactoryTest1._mf_y );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sub,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_sum()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sum,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_x );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sum,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_trans()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_trans,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_e,
		ValueFactoryTest1._mf_x );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_dist()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_dist,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a,
		ValueFactoryTest1._mf_b );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_dist,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_fill()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_fill,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_n,
		ValueFactoryTest1._mf_x );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fill,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_fillObject()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_fillObject,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_n,
		ValueFactoryTest1._mf_o );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fillObject,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_blow()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_blow,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_msg,
		ValueFactoryTest1._mf_code );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_blow,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_beets()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_e );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_throwExcp5()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_throwExcp5,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_msg,
		ValueFactoryTest1._mf_code,
		ValueFactoryTest1._mf_value );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_throwExcp5,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_throwExcp6()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_throwExcp5,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_msg,
		ValueFactoryTest1._mf_code,
		ValueFactoryTest1._mf_value );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_throwExcp6,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}

	/** */
	@org.junit.Test
	public void test_method_p_boolean()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_boolean,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_p_boolean_array()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_boolean_array,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean_array,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_p_byte()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_byte,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_p_byte_array()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_byte_array,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte_array,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_p_short()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_short,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_p_short_array()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_short_array,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short_array,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_p_int()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_int,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_p_int_array()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_int_array,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int_array,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_p_long()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_long,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
	public void test_method_p_long_array()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_long_array,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long_array,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
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
	
	/** */
	@org.junit.Test
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
	
	/** */
	@org.junit.Test
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
	
	/** */
	@org.junit.Test
	public void test_method_p_double_array()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_double_array,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_double_array,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
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
	
	/** */
	@org.junit.Test
	public void test_method_p_string_array()
	{
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_string_array,
			ValueFactoryTest1._mf__messageId,
		ValueFactoryTest1._mf_a );
		
		checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string_array,
			ValueFactoryTest1._mf__messageId,
			ValueFactoryTest1._mf__inReplyTo,
		ValueFactoryTest1._mf_result );
	}
	
	/** */
	@org.junit.Test
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
	
	/** */
	@org.junit.Test
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
	
	/** */
	@org.junit.Test
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
	
	/** */
	@org.junit.Test
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
	
	/** */
	@org.junit.Test
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
	
	/** */
	@org.junit.Test
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
	
	/** */
	@org.junit.Test
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
	
	/** */
	@org.junit.Test
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
	
	/** */
	@org.junit.Test
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
	
	/** */
	@org.junit.Test
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
	
	/** */
	@org.junit.Test
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

	/** */
	@org.junit.Test
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
	
	private void checkType( Type type, Field... fields )
	{
		assertNotNull( type );
		assertSame( Type.class, type.getClass() );
		assertSame( type, vf.getType( type.getId() ) );
		
		Set<Field> tfields = type.getFields();

		if (fields != null)
		{
			assertEquals( fields.length, tfields.size() );
			
			for (Field f: fields)
			{
				assertNotNull( type.getValidator( f ) );
				assertSame( f, type.getField( f.getId() ) );
				assertSame( f, type.getField( f.getName() ) );
			}
		}
		else
		{
			assertEquals( 0, tfields.size() );
		}
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
		StructValue sv = new StructValue( t, vf );
		sv.put(  f, true );
		E1 a = (E1) vf.importCustomValue( sv );
		assertSame( e, a );
	}
	
	private void testS3Export(String s, Object value)
	{
		StructValue sv = vf.exportCustomValue( new S3( s,value ) );
		sv.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S3 );
		assertEquals( 2, sv.size() );
		assertEquals( s, sv.get( ValueFactoryTest1._mf_tipe ) );
		assertEquals( value, sv.get( ValueFactoryTest1._mf_x ) );
	}
	
	private void testS3Import(String s, Object value)
	{
		StructValue sv = new StructValue( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S3, vf );
		sv.put( ValueFactoryTest1._mf_tipe, s );
		sv.put( ValueFactoryTest1._mf_x, value );
		S3 myS3 = (S3) vf.importCustomValue( sv );
		assertEquals( s, myS3.tipe );
		assertEquals( value, myS3.x );
	}	
	
	private void testS4Export(String s, Object[] value)
	{
		StructValue sv = vf.exportCustomValue( new S4( s,value ) );
		sv.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S4 );
		assertEquals( 2, sv.size() );
		assertEquals( s, sv.get( ValueFactoryTest1._mf_tipe ) );
		assertEquals( value, sv.get( ValueFactoryTest1._mf_x ) );
	}
	
	private void testS4Import(String s, Object[] value)
	{
		StructValue sv = new StructValue( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_S4, vf );
		sv.put( ValueFactoryTest1._mf_tipe, s );
		sv.put( ValueFactoryTest1._mf_x, value );
		S4 myS4 = (S4) vf.importCustomValue( sv );
		assertEquals( s, myS4.tipe );
		assertArrayEquals( value, myS4.x );
	}
	
	private void testExcp5Export(String msg, int code, Object value)
	{
		StructValue sv = vf.exportCustomValue( new Excp5( msg, code, value ) );
		sv.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp5 );
		assertEquals( 3, sv.size() );
		assertEquals( msg, sv.get( ValueFactoryTest1._mf_msg ) );
		assertEquals( code, sv.get( ValueFactoryTest1._mf_code ) );
		assertEquals( value, sv.get( ValueFactoryTest1._mf_x ) );		
	}
	
	private void testExcp5Import(String msg, int code, Object value)
	{
		StructValue sv = new StructValue( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp5, vf );
		sv.put( ValueFactoryTest1._mf_msg, msg );
		sv.put( ValueFactoryTest1._mf_code, code );
		sv.put( ValueFactoryTest1._mf_x, value );		
		Excp5 e = (Excp5) vf.importCustomValue( sv );
		assertEquals( msg, e.msg );
		assertEquals( code, e.code );
		assertEquals( value, e.x );		
	}	
	
	private void testExcp6Export(String msg, int code, Object[] value)
	{
		StructValue sv = vf.exportCustomValue( new Excp6( msg, code, value ) );
		sv.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp6 );
		assertEquals( 3, sv.size() );
		assertEquals( msg, sv.get( ValueFactoryTest1._mf_msg ) );
		assertEquals( code, sv.get( ValueFactoryTest1._mf_code ) );
		assertEquals( value, sv.get( ValueFactoryTest1._mf_x ) );		
	}
	
	private void testExcp6Import(String msg, int code, Object[] value)
	{
		StructValue sv = new StructValue( ValueFactoryTest1._mt_org_apache_etch_tests_Test1_Excp6, vf );
		sv.put( ValueFactoryTest1._mf_msg, msg );
		sv.put( ValueFactoryTest1._mf_code, code );
		sv.put( ValueFactoryTest1._mf_x, value );		
		Excp6 e = (Excp6) vf.importCustomValue( sv );
		assertEquals( msg, e.msg );
		assertEquals( code, e.code );
		assertArrayEquals( value, e.x );		
	}
}

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

import org.apache.etch.bindings.java.util.StrIntHashMap;
import org.apache.etch.tests.Test1.E1;
import org.apache.etch.tests.Test1.S1;
import org.apache.etch.tests.Test1.S2;
import org.apache.etch.tests.Test1.S3;
import org.apache.etch.tests.Test1.S4;
import org.junit.Before;
import org.junit.Test;


/**
 * Simple tests to ensure that ImplTestClient and ImplTestServer are generated
 * correctly
 *
 * @author gsandhir
 */
public class TestImplTest1
{
	private ImplTest1Server testServer = null;
	private ImplTest1Client testClient = null;
	
	/** */
	@Before public void setup() {
		RemoteTest1Client tc = null;
		RemoteTest1Server ts = null;
		testServer = new ImplTest1Server(tc);
		testClient = new ImplTest1Client(ts);
	}
	
	/** */
	@Test public void testIncr() {
		
		try
		{
			Integer i = new Integer(5);		
			testServer.incr(i);			
			testClient.incr( i );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
		
		}
	}
	
	/** */
	@Test public void testNothing() {
		
		try
		{			
			testServer.nothing();		
			testClient.nothing();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
		
		}
	}
	
	/** */
	@Test public void testAdd() {
		try
		{
			Integer i = new Integer(5);
			Integer j = new Integer(3);
			testServer.add( i,j);
			testClient.add( i,j);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void testSub() {
		try
		{
			Integer i = new Integer(5);
			Integer j = new Integer(3);
			testServer.sub( i,j);
			testClient.sub( i,j);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testSum() {
		try
		{
			int[] x = {1,2,3};
			testServer.sum( x );
			testClient.sum( x );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}				
	}
	
	/** */
	@Test public void  testFill() {
		try
		{
			Integer i = new Integer(5);
			Integer j = new Integer(3);
			testServer.fill( i, j );
			testClient.fill( i,j );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}				
	}
	
	/** */
	@Test public void  testFillObject() {
		try
		{
			Integer i = new Integer(5);
			Object j = null;
			testServer.fillObject( i, j );
			testClient.fillObject( i,j );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}				
	}
	
	
	/** */
	@Test public void  testTrans() {
		try
		{
			Integer i = new Integer(5);
			E1 enumA = E1.A;
			testServer.trans( enumA,i  );
			testClient.trans( enumA,i  );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}				
	}
	
	/** */
	@Test public void  testDist() {
		try
		{
			S1 a = new S1();
			S1 b = new S1();
			testServer.dist( a, b );
			testClient.dist( a, b );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}			
	}
	
	/** @throws Exception */
	@Test public void  testBlow() throws Exception {
		try
		{
			Integer i = new Integer(5);		
			testServer.blow( "", i);
			testClient.blow( "", i );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}			
	}
	
	/** @throws Exception */
	@Test public void  testExcp5() throws Exception {
		try
		{
			Integer i = new Integer(5);	
			Object value = null;
			testServer.throwExcp5( "", i, value );
			testClient.throwExcp5( "", i, value );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}			
	}
	
	/** @throws Exception */
	@Test public void  testExcp6() throws Exception {
		try
		{
			Integer i = new Integer(5);	
			Object value[] = null;
			testServer.throwExcp6( "", i, value );
			testClient.throwExcp6( "", i, value );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}			
	}
	
	/** */
	@Test public void  testBoolean() {
		try
		{
			Boolean b = null;	
			testServer.p_boolean( b );
			testClient.p_boolean( b );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}		
	}
	
	/** */
	@Test public void  testBooleanArray() {
		try
		{
			boolean b[] = null;	
			testServer.p_boolean_array( b );
			testClient.p_boolean_array( b )	;
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testByte() {
		try
		{
			Byte a = null;
			testServer.p_byte( a );
			testClient.p_byte( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testByteArray() {
		try
		{
			byte a[] = null;
			testServer.p_byte_array( a );
			testClient.p_byte_array( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testShort() {
		try
		{
			Short a  = null;
			testServer.p_short( a );
			testClient.p_short( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testShortArray() {
		try
		{
			short a[] = null;
			testServer.p_short_array( a );
			testClient.p_short_array( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
		
		}
	}
	
	/** */
	@Test public void  testInt() {
		try
		{
			Integer a  = null;
			testServer.p_int( a );
			testClient.p_int( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testIntArray() {
		try
		{
			int a[]  = null;
			testServer.p_int_array( a );
			testClient.p_int_array( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testLong() {
		try
		{
			Long a  = null;
			testServer.p_long( a );
			testClient.p_long( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testLongArray() {
		try
		{
			long a[]  = null;
			testServer.p_long_array( a );
			testClient.p_long_array( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testFloat() {
		try
		{
			Float a  = null;
			testServer.p_float( a );
			testClient.p_float( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testFloatArray() {
		try
		{
			float a[]  = null;
			testServer.p_float_array( a );
			testClient.p_float_array( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	
	
	/** */
	@Test public void  testDouble() {
		try
		{
			Double a  = null;
			testServer.p_double( a );
			testClient.p_double( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testDoubleArray() {
		try
		{
			double a[]  = null;
			testServer.p_double_array( a );
			testClient.p_double_array( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testString() {
		try
		{
			String a  = null;
			testServer.p_string( a );
			testClient.p_string( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testStringArray() {
		try
		{
			String a[]  = null;
			testServer.p_string_array( a );
			testClient.p_string_array( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testE1() {
		try
		{
			E1 a  = null;
			testServer.p_E1( a );
			testClient.p_E1( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testE1Array() {
		try
		{
			E1 a[]  = null;
			testServer.p_E1_array( a );
			testClient.p_E1_array( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testS1() {
		try
		{
			S1 a  = null;
			testServer.p_S1( a );
			testClient.p_S1( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testS1Array() {
		try
		{
			S1 a[]  = null;
			testServer.p_S1_array( a );
			testClient.p_S1_array( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testS2() {
		try
		{
			S2 a  = null;
			testServer.p_S2( a );
			testClient.p_S2( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testS2Array() {
		try
		{
			S2 a[]  = null;
			testServer.p_S2_array( a );
			testClient.p_S2_array( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testBlob() {
		try
		{
			StrIntHashMap a  = null;
			testServer.p_Blob( a );
			testClient.p_Blob( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testBlobArray() {
		try
		{
			StrIntHashMap[] a  = null;
			testServer.p_Blob_array( a );
			testClient.p_Blob_array( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testObject() {
		try
		{
			Object a  = null;
			testServer.p_object( a );
			testClient.p_object( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testObjectArray() {
		try
		{
			Object[] a  = null;
			testServer.p_object_array( a );
			testClient.p_object_array( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testObjectStruct() {
		try
		{
			S3 a  = null;
			testServer.p_object_struct( a );
			testClient.p_object_struct( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testObjectStructArray() {
		try
		{
			S4 a  = null;
			testServer.p_object_struct_array( a );
			testClient.p_object_struct_array( a );
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testAlwaysWork() {
		try
		{
			
			testServer.alwaysWorks();
			testClient.alwaysWorks();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testAlwaysFail() {
		try
		{
			
			testServer.alwaysFails();
			testClient.alwaysFails();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testIsTrue() {
		try
		{
			
			testServer.isTrue();
			testClient.isTrue();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testIsFalse() {
		try
		{
			
			testServer.isFalse();
			testClient.isFalse();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testM1() {
		try
		{
			
			testServer.m1();
			testClient.m1();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testCanM1() {
		try
		{
			
			testServer.can_m1();
			testClient.can_m1();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testM2() {
		try
		{
			
			testServer.m2();
			testClient.m2();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testCanM2() {
		try
		{
			Boolean b = null;
			testServer.can_m2(b);
			testClient.can_m2(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testM3() {
		try
		{
			
			testServer.m3();
			testClient.m3();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testCanM3() {
		try
		{
			Boolean b = null;
			testServer.can_m3(b);
			testClient.can_m3(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testM4() {
		try
		{
			
			testServer.m4();
			testClient.m4();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testCanM4() {
		try
		{
			Boolean b = null;
			testServer.can_m4(b);
			testClient.can_m4(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testM5() {
		try
		{
			
			testServer.m5();
			testClient.m5();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testCanM5() {
		try
		{
			Byte b = null;
			testServer.can_m5(b);
			testClient.can_m5(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testM6() {
		try
		{
			
			testServer.m6();
			testClient.m6();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testCanM6() {
		try
		{
			Short b = null;
			testServer.can_m6(b);
			testClient.can_m6(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testM7() {
		try
		{
			
			testServer.m7();
			testClient.m7();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testCanM7() {
		try
		{
			Integer b = null;
			testServer.can_m7(b);
			testClient.can_m7(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testM8() {
		try
		{
			
			testServer.m8();
			testClient.m8();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testCanM8() {
		try
		{
			Long b = null;
			testServer.can_m8(b);
			testClient.can_m8(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testM9() {
		try
		{
			
			testServer.m6();
			testClient.m6();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testCanM9() {
		try
		{
			Float b = null;
			testServer.can_m9(b);
			testClient.can_m9(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testM10() {
		try
		{
			
			testServer.m10();
			testClient.m10();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testCanM10() {
		try
		{
			Double b = null;
			testServer.can_m10(b);
			testClient.can_m10(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testM11() {
		try
		{
			
			testServer.m11();
			testClient.m11();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testCanM11() {
		try
		{
			String b = null;
			testServer.can_m11(b);
			testClient.can_m11(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testM12() {
		try
		{
			
			testServer.m12();
			testClient.m12();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testCanM12() {
		try
		{
			Integer b = null;
			testServer.can_m12(b);
			testClient.can_m12(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testM13() {
		try
		{
			Integer b = null;
			testServer.m13(b);
			testClient.m13(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testCanM13() {
		try
		{
			Integer b = null;
			testServer.can_m13(b);
			testClient.can_m13(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testM14() {
		try
		{
			S1 b = null;
			testServer.m14(b);
			testClient.m14(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testCanM14() {
		try
		{
			Integer b = null;
			testServer.can_m14(b);
			testClient.can_m14(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testM15() {
		try
		{
		
			testServer.m15();
			testClient.m15();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	/** */
	@Test public void  testCanM15() {
		try
		{
			E1 b = null;
			testServer.can_m15(b);
			testClient.can_m15(b);
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
			
		}
	}
	
	
	
	
	
}

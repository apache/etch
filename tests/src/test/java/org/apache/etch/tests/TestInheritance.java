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

import static org.junit.Assert.assertEquals;

import org.apache.etch.tests.Inheritance.E1;
import org.apache.etch.tests.Inheritance.E2;
import org.apache.etch.tests.Inheritance.E3;
import org.apache.etch.tests.Inheritance.S1;
import org.apache.etch.tests.Inheritance.S2;
import org.apache.etch.tests.Inheritance.S3;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;


/**
 * Main implementation for InheritanceClient.
 */
public class TestInheritance
{
	/** @throws Exception */
	@BeforeClass
	public static void setUp() throws Exception
	{
		MainInheritanceListener.main( new String[] { "tcp://127.0.0.1:0" } );
		
		String uri = MainInheritanceListener.localUri;

		server = InheritanceHelper.newServer( uri, null,
			new InheritanceHelper.InheritanceClientFactory()
			{
				public InheritanceClient newInheritanceClient(
					RemoteInheritanceServer server ) throws Exception
				{
					return new ImplInheritanceClient( server );
				}
			} );
		
		server._startAndWaitUp( 4000 );
	}
	
	private static RemoteInheritanceServer server;
	
	/** @throws Exception */
	@Test
	public void types() throws Exception
	{
		assertEquals( true, S1.class.isAssignableFrom( S1.class ) );
		assertEquals( true, S1.class.isAssignableFrom( S2.class ) );
		assertEquals( true, S1.class.isAssignableFrom( S3.class ) );
		
		assertEquals( false, S2.class.isAssignableFrom( S1.class ) );
		assertEquals( true, S2.class.isAssignableFrom( S2.class ) );
		assertEquals( true, S2.class.isAssignableFrom( S3.class ) );
		
		assertEquals( false, S3.class.isAssignableFrom( S1.class ) );
		assertEquals( false, S3.class.isAssignableFrom( S2.class ) );
		assertEquals( true, S3.class.isAssignableFrom( S3.class ) );
		
		assertEquals( true, E1.class.isAssignableFrom( E1.class ) );
		assertEquals( true, E1.class.isAssignableFrom( E2.class ) );
		assertEquals( true, E1.class.isAssignableFrom( E3.class ) );
		
		assertEquals( true, Exception.class.isAssignableFrom( E1.class ) );
		assertEquals( true, Exception.class.isAssignableFrom( E2.class ) );
		assertEquals( true, Exception.class.isAssignableFrom( E3.class ) );
		
		assertEquals( false, E2.class.isAssignableFrom( E1.class ) );
		assertEquals( true, E2.class.isAssignableFrom( E2.class ) );
		assertEquals( true, E2.class.isAssignableFrom( E3.class ) );
		
		assertEquals( false, E3.class.isAssignableFrom( E1.class ) );
		assertEquals( false, E3.class.isAssignableFrom( E2.class ) );
		assertEquals( true, E3.class.isAssignableFrom( E3.class ) );
	}
	
	/** @throws Exception */
	@Test
	public void construct_s1() throws Exception
	{
		check_s1( new S1(), null, null );
		
		check_s1( new S1( null, null ), null, null );
		
		check_s1( new S1( 1, 2 ), 1, 2 );
	}
	
	/** @throws Exception */
	@Test
	public void construct_s2() throws Exception
	{
		check_s2( new S2(), null, null, null, null );
		
		check_s2( new S2( null, null, null, null ), null, null, null, null );
		
		check_s2( new S2( 1, 2, 3, 4 ), 1, 2, 3, 4 );
	}
	
	/** @throws Exception */
	@Test
	public void construct_s3() throws Exception
	{
		check_s3( new S3(), null, null, null, null, null, null );
		
		check_s3( new S3( null, null, null, null, null, null ),
			null, null, null, null, null, null );
		
		check_s3( new S3( 1, 2, 3, 4, 5, 6 ), 1, 2, 3, 4, 5, 6 );
	}
	
	/** @throws Exception */
	@Test
	public void construct_e1() throws Exception
	{
		check_e1( new E1(), null, null );
		
		check_e1( new E1( null, null ), null, null );
		
		check_e1( new E1( 1, 2 ), 1, 2 );
	}
	
	/** @throws Exception */
	@Test
	public void construct_e2() throws Exception
	{
		check_e2( new E2(), null, null, null, null );
		
		check_e2( new E2( null, null, null, null ), null, null, null, null );
		
		check_e2( new E2( 1, 2, 3, 4 ), 1, 2, 3, 4 );
	}
	
	/** @throws Exception */
	@Test
	public void construct_e3() throws Exception
	{
		check_e3( new E3(), null, null, null, null, null, null );
		
		check_e3( new E3( null, null, null, null, null, null ),
			null, null, null, null, null, null );
		
		check_e3( new E3( 1, 2, 3, 4, 5, 6 ), 1, 2, 3, 4, 5, 6 );
	}
	
	/** @throws Exception */
	@Test
	public void assign_s1() throws Exception
	{
		S1 v = new S1( 1, 2 ); check_s1( v, 1, 2 );
		
		v.a = null;            check_s1( v, null, 2 );
		v.a = 3;               check_s1( v, 3, 2 );
		v.setA( null );        check_s1( v, null, 2 );
		v.setA( 4 );           check_s1( v, 4, 2 );
		
		v.b = null;            check_s1( v, 4, null );
		v.b = 5;               check_s1( v, 4, 5 );
		v.setB( null );        check_s1( v, 4, null );
		v.setB( 6 );           check_s1( v, 4, 6 );
	}
	
	/** @throws Exception */
	@Test
	public void assign_s2() throws Exception
	{
		S2 v = new S2( 1, 2, 3, 4 ); check_s2( v, 1, 2, 3, 4 );
		
		v.a = null;                  check_s2( v, null, 2, 3, 4 );
		v.a = 5;                     check_s2( v, 5, 2, 3, 4 );
		v.setA( null );              check_s2( v, null, 2, 3, 4 );
		v.setA( 6 );                 check_s2( v, 6, 2, 3, 4 );
		
		v.b = null;                  check_s2( v, 6, null, 3, 4 );
		v.b = 7;                     check_s2( v, 6, 7, 3, 4 );
		v.setB( null );              check_s2( v, 6, null, 3, 4 );
		v.setB( 8 );                 check_s2( v, 6, 8, 3, 4 );
		
		v.c = null;                  check_s2( v, 6, 8, null, 4 );
		v.c = 9;                     check_s2( v, 6, 8, 9, 4 );
		v.setC( null );              check_s2( v, 6, 8, null, 4 );
		v.setC( 10 );                check_s2( v, 6, 8, 10, 4 );
		
		v.d = null;                  check_s2( v, 6, 8, 10, null );
		v.d = 11;                    check_s2( v, 6, 8, 10, 11 );
		v.setD( null );              check_s2( v, 6, 8, 10, null );
		v.setD( 12 );                check_s2( v, 6, 8, 10, 12 );
	}
	
	/** @throws Exception */
	@Test
	public void assign_s3() throws Exception
	{
		S3 v = new S3( 1, 2, 3, 4, 5, 6 ); check_s3( v, 1, 2, 3, 4, 5, 6 );
		
		v.a = null;                        check_s3( v, null, 2, 3, 4, 5, 6 );
		v.a = 7;                           check_s3( v, 7, 2, 3, 4, 5, 6 );
		v.setA( null );                    check_s3( v, null, 2, 3, 4, 5, 6 );
		v.setA( 8 );                       check_s3( v, 8, 2, 3, 4, 5, 6 );
		
		v.b = null;                        check_s3( v, 8, null, 3, 4, 5, 6 );
		v.b = 9;                           check_s3( v, 8, 9, 3, 4, 5, 6 );
		v.setB( null );                    check_s3( v, 8, null, 3, 4, 5, 6 );
		v.setB( 10 );                      check_s3( v, 8, 10, 3, 4, 5, 6 );
		
		v.c = null;                        check_s3( v, 8, 10, null, 4, 5, 6 );
		v.c = 11;                          check_s3( v, 8, 10, 11, 4, 5, 6 );
		v.setC( null );                    check_s3( v, 8, 10, null, 4, 5, 6 );
		v.setC( 12 );                      check_s3( v, 8, 10, 12, 4, 5, 6 );
		
		v.d = null;                        check_s3( v, 8, 10, 12, null, 5, 6 );
		v.d = 13;                          check_s3( v, 8, 10, 12, 13, 5, 6 );
		v.setD( null );                    check_s3( v, 8, 10, 12, null, 5, 6 );
		v.setD( 14 );                      check_s3( v, 8, 10, 12, 14, 5, 6 );
		
		v.e = null;                        check_s3( v, 8, 10, 12, 14, null, 6 );
		v.e = 15;                          check_s3( v, 8, 10, 12, 14, 15, 6 );
		v.setE( null );                    check_s3( v, 8, 10, 12, 14, null, 6 );
		v.setE( 16 );                      check_s3( v, 8, 10, 12, 14, 16, 6 );
		
		v.f = null;                        check_s3( v, 8, 10, 12, 14, 16, null );
		v.f = 17;                          check_s3( v, 8, 10, 12, 14, 16, 17 );
		v.setF( null );                    check_s3( v, 8, 10, 12, 14, 16, null );
		v.setF( 18 );                      check_s3( v, 8, 10, 12, 14, 16, 18 );
	}
	
	/** @throws Exception */
	@Test
	public void tostring() throws Exception
	{
		assertEquals(
			"S1(a=1; b=2)",
			new S1( 1, 2 ).toString() );
		
		assertEquals(
			"S2(S1(a=1; b=2); c=3; d=4)",
			new S2( 1, 2, 3, 4 ).toString() );
		
		assertEquals(
			"S3(S2(S1(a=1; b=2); c=3; d=4); e=5; f=6)",
			new S3( 1, 2, 3, 4, 5, 6 ).toString() );
		
		assertEquals(
			"org.apache.etch.tests.Inheritance$E1: a=1; b=2",
			new E1( 1, 2 ).toString() );
		
		assertEquals(
			"org.apache.etch.tests.Inheritance$E2: a=1; b=2; c=3; d=4",
			new E2( 1, 2, 3, 4 ).toString() );
		
		assertEquals(
			"org.apache.etch.tests.Inheritance$E3: a=1; b=2; c=3; d=4; e=5; f=6",
			new E3( 1, 2, 3, 4, 5, 6 ).toString() );
	}

	private void check_s1( S1 v, Integer a, Integer b )
	{
		assertEquals( a, v.a );
		assertEquals( a, v.getA() );
		
		assertEquals( b, v.b );
		assertEquals( b, v.getB() );
	}

	private void check_s2( S2 v, Integer a, Integer b, Integer c, Integer d )
	{
		check_s1( v, a, b );
		
		assertEquals( c, v.c );
		assertEquals( c, v.getC() );
		
		assertEquals( d, v.d );
		assertEquals( d, v.getD() );
	}

	private void check_s3( S3 v, Integer a, Integer b, Integer c, Integer d,
		Integer e, Integer f )
	{
		check_s2( v, a, b, c, d );
		
		assertEquals( e, v.e );
		assertEquals( e, v.getE() );
		
		assertEquals( f, v.f );
		assertEquals( f, v.getF() );
	}

	private void check_e1( E1 v, Integer a, Integer b )
	{
		assertEquals( a, v.a );
		assertEquals( a, v.getA() );
		
		assertEquals( b, v.b );
		assertEquals( b, v.getB() );
	}

	private void check_e2( E2 v, Integer a, Integer b, Integer c, Integer d )
	{
		check_e1( v, a, b );
		
		assertEquals( c, v.c );
		assertEquals( c, v.getC() );
		
		assertEquals( d, v.d );
		assertEquals( d, v.getD() );
	}

	private void check_e3( E3 v, Integer a, Integer b, Integer c, Integer d,
		Integer e, Integer f )
	{
		check_e2( v, a, b, c, d );
		
		assertEquals( e, v.e );
		assertEquals( e, v.getE() );
		
		assertEquals( f, v.f );
		assertEquals( f, v.getF() );
	}

	/** @throws Exception */
	@Test
	public void test_s1() throws Exception
	{
		do_s1( new S1( 1, 2 ), new S1Compare() );
		do_s1( new S1( 9, 8 ), new S1Compare() );
	}

	/** @throws Exception */
	@Test
	public void test_s2() throws Exception
	{
		do_s2( new S2( 1, 2, 3, 4 ), new S2Compare() );
		do_s2( new S2( 9, 8, 7, 6 ), new S2Compare() );
	}

	/** @throws Exception */
	@Test
	public void test_s3() throws Exception
	{
		do_s3( new S3( 1, 2, 3, 4, 5, 6 ), new S3Compare() );
		do_s3( new S3( 9, 8, 7, 6, 5, 4 ), new S3Compare() );
	}
	
	private interface Compare<T>
	{
		/**
		 * @param a
		 * @param b
		 */
		public void compare( T a, T b );
	}
	
	/**
	 * Class to compare one S1 to another.
	 */
	static class S1Compare implements Compare<S1>
	{
		public void compare( S1 a, S1 b )
		{
			assertEquals( S1.class, a.getClass() );
			assertEquals( S1.class, b.getClass() );
			assertEquals( a.a, b.a );
			assertEquals( a.b, b.b );
		}
	}
	
	/**
	 * Class to compare one S2 to another.
	 */
	static class S2Compare implements Compare<S2>
	{
		public void compare( S2 a, S2 b )
		{
			assertEquals( S2.class, a.getClass() );
			assertEquals( S2.class, b.getClass() );
			assertEquals( a.a, b.a );
			assertEquals( a.b, b.b );
			assertEquals( a.c, b.c );
			assertEquals( a.d, b.d );
		}
	}

	/**
	 * Class to compare one S3 to another.
	 */
	static class S3Compare implements Compare<S3>
	{
		public void compare( S3 a, S3 b )
		{
			assertEquals( S3.class, a.getClass() );
			assertEquals( S3.class, b.getClass() );
			assertEquals( a.a, b.a );
			assertEquals( a.b, b.b );
			assertEquals( a.c, b.c );
			assertEquals( a.d, b.d );
			assertEquals( a.e, b.e );
			assertEquals( a.f, b.f );
		}
	}
	
	private static class AObjectCompare implements Compare<Object[]>
	{
		/**
		 * @param cmp
		 */
		public AObjectCompare( Compare<Object> cmp )
		{
			this.cmp = cmp;
		}
		
		private final Compare<Object> cmp;

		@SuppressWarnings("unchecked")
		public void compare( Object[] a, Object[] b )
		{
			assertEquals( a.length, b.length );
			for (int i = 0; i < a.length; i++)
				cmp.compare( a[i], b[i] );
		}
	}
	
	private static class AS1Compare implements Compare<S1[]>
	{
		/**
		 * @param cmp
		 */
		public AS1Compare( Compare<S1> cmp )
		{
			this.cmp = cmp;
		}
		
		private final Compare<S1> cmp;

		@SuppressWarnings("unchecked")
		public void compare( S1[] a, S1[] b )
		{
			assertEquals( a.length, b.length );
			for (int i = 0; i < a.length; i++)
				cmp.compare( a[i], b[i] );
		}
	}
	
	private static class AS2Compare implements Compare<S2[]>
	{
		/**
		 * @param cmp
		 */
		public AS2Compare( Compare<S2> cmp )
		{
			this.cmp = cmp;
		}
		
		private final Compare<S2> cmp;

		@SuppressWarnings("unchecked")
		public void compare( S2[] a, S2[] b )
		{
			assertEquals( a.length, b.length );
			for (int i = 0; i < a.length; i++)
				cmp.compare( a[i], b[i] );
		}
	}
	
	private static class AS3Compare implements Compare<S3[]>
	{
		/**
		 * @param cmp
		 */
		public AS3Compare( Compare<S3> cmp )
		{
			this.cmp = cmp;
		}
		
		private final Compare<S3> cmp;

		@SuppressWarnings("unchecked")
		public void compare( S3[] a, S3[] b )
		{
			assertEquals( a.length, b.length );
			for (int i = 0; i < a.length; i++)
				cmp.compare( a[i], b[i] );
		}
	}

	/////////////
	// HELPERS //
	/////////////
	
	@SuppressWarnings("unchecked")
	private void do_obj( Object v, Compare cmp )
	{
		// test an object both by itself and as an array.
		
		cmp.compare( v, server.f1( v ) );
		
		Object[] a = { v };
		Compare acmp = new AObjectCompare( cmp );
		
		acmp.compare( a, server.f5( a ) );
	}

	@SuppressWarnings("unchecked")
	private void do_s1( S1 v, Compare cmp )
	{
		// test an S1 both by itself and as an array.
		// also test each of those as an object.
		
		cmp.compare( v, server.f2( v ) );
		do_obj( v, cmp );
		
		S1[] a = { v };
		Compare acmp = new AS1Compare( cmp );

		acmp.compare( a, server.f6( a ) );
		do_obj( a, acmp );
	}

	@SuppressWarnings("unchecked")
	private void do_s2( S2 v, Compare cmp )
	{
		// test an S2 both by itself and as an array.
		// also test the S2 as an S1, and the S2 array
		// as an object.
		
		cmp.compare( v, server.f3( v ) );
		do_s1( v, cmp );
		
		S2[] a = { v };
		Compare acmp = new AS2Compare( cmp );

		acmp.compare( a, server.f7( a ) );
		do_obj( a, acmp );
	}

	@SuppressWarnings("unchecked")
	private void do_s3( S3 v, Compare cmp )
	{
		// test an S3 both by itself and as an array.
		// also test the S3 as an S2, and the S3 array
		// as an object.
		
		cmp.compare( v, server.f4( v ) );
		do_s2( v, cmp );
		
		S3[] a = { v };
		Compare acmp = new AS3Compare( cmp );

		acmp.compare( a, server.f8( a ) );
		do_obj( a, acmp );
	}

	/** @throws Exception */
	@AfterClass
	public static void fini() throws Exception
	{
		if (server != null)
			server._stopAndWaitDown( 4000 );
	}
}

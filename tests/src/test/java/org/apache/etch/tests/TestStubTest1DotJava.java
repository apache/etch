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
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.support.DeliveryService;
import org.apache.etch.bindings.java.support.Mailbox;
import org.apache.etch.bindings.java.support.Pool;
import org.apache.etch.bindings.java.support._Etch_AuthException;
import org.apache.etch.bindings.java.transport.SessionMessage;
import org.apache.etch.util.core.Who;


/** Test of StubTest1 */
public class TestStubTest1DotJava 
{
	/** @throws Exception */
	@org.junit.Test
	public void method_nothing() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_nothing,
			vf );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_nothing );
		assertEquals( 0, src.xreply.size() );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_incr1() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_incr, vf );
		msg.put( ValueFactoryTest1._mf_x, 3 );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_incr );
		assertEquals( 1, src.xreply.size() );
		assertEquals( 4, src.xreply.get( ValueFactoryTest1._mf_result ) );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_incr2() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_incr, vf );
		// msg.put( ValueFactoryTest1._mf_x, 3 ); -- cause an exception
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_incr );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest1._mf_result );
		assertTrue( o instanceof RuntimeException ); // because incr is not
		// declared to throw
		// anything
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_sub() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sub, vf );
		msg.put( ValueFactoryTest1._mf_x, 7 );
		msg.put( ValueFactoryTest1._mf_y, 3 );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sub );
		assertEquals( 1, src.xreply.size() );
		assertEquals( 4, src.xreply.get( ValueFactoryTest1._mf_result ) );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_sum() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sum, vf );
		msg.put( ValueFactoryTest1._mf_x, new int[]
		{
			1, 2, 3, 7, 11
		} );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sum );
		assertEquals( 1, src.xreply.size() );
		assertEquals( 24, src.xreply.get( ValueFactoryTest1._mf_result ) );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_trans1() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_trans,
			vf );
		msg.put( ValueFactoryTest1._mf_e, Test1.E1.A );
		msg.put( ValueFactoryTest1._mf_x, 5 );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans );
		assertEquals( 1, src.xreply.size() );
		assertEquals( 2, src.xreply.get( ValueFactoryTest1._mf_result ) );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_trans2() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_trans,
			vf );
		msg.put( ValueFactoryTest1._mf_e, Test1.E1.B );
		msg.put( ValueFactoryTest1._mf_x, 5 );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans );
		assertEquals( 1, src.xreply.size() );
		assertEquals( 10, src.xreply.get( ValueFactoryTest1._mf_result ) );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_trans3() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_trans,
			vf );
		msg.put( ValueFactoryTest1._mf_e, Test1.E1.C );
		msg.put( ValueFactoryTest1._mf_x, 5 );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans );
		assertEquals( 1, src.xreply.size() );
		assertEquals( 12, src.xreply.get( ValueFactoryTest1._mf_result ) );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_dist1() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_dist, vf );
		msg.put( ValueFactoryTest1._mf_a, new Test1.S1( 1, 1, 1 ) );
		msg.put( ValueFactoryTest1._mf_b, new Test1.S1( 0, 0, 0 ) );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_dist );
		assertEquals( 1, src.xreply.size() );
		assertEquals( Math.sqrt( 3 ), src.xreply
			.get( ValueFactoryTest1._mf_result ) );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_dist2() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_dist, vf );
		msg.put( ValueFactoryTest1._mf_a, new Test1.S1( 1, 2, 3 ) );
		msg.put( ValueFactoryTest1._mf_b, new Test1.S1( 6, 5, 4 ) );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_dist );
		assertEquals( 1, src.xreply.size() );
		assertEquals( Math.sqrt( 35 ), src.xreply
			.get( ValueFactoryTest1._mf_result ) );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_fill() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_fill, vf );
		msg.put( ValueFactoryTest1._mf_n, 4 );
		msg.put( ValueFactoryTest1._mf_x, 3 );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fill );
		assertEquals( 1, src.xreply.size() );
		int[] x = (int[]) src.xreply.get( ValueFactoryTest1._mf_result );
		assertNotNull( x );
		assertEquals( 4, x.length );
		for (int y : x)
			assertEquals( 3, y );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_blow() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_blow, vf );
		msg.put( ValueFactoryTest1._mf_msg, "foo" );
		msg.put( ValueFactoryTest1._mf_code, 23 );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_blow );
		assertEquals( 1, src.xreply.size() );
		Test1.Excp1 e = (Test1.Excp1) src.xreply
			.get( ValueFactoryTest1._mf_result );
		assertNotNull( e );
		assertEquals( "foo", e.msg );
		assertEquals( 23, e.code );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_beets1() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets,
			vf );
		msg.put( ValueFactoryTest1._mf_e, Test1.E1.A );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest1._mf_result );
		assertEquals( 5, o );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_beets2() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets,
			vf );
		msg.put( ValueFactoryTest1._mf_e, Test1.E1.B );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest1._mf_result );
		assertEquals( Test1.Excp3.class, o.getClass() );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_beets3() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets,
			vf );
		msg.put( ValueFactoryTest1._mf_e, Test1.E1.C );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest1._mf_result );
		assertEquals( Test1.Excp4.class, o.getClass() );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_beets4() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets,
			vf );
		msg.put( ValueFactoryTest1._mf_e, null );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets );
		assertEquals( 0, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest1._mf_result );
		assertNull( o );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_isTrue() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_isTrue,
			vf );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_isTrue );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest1._mf_result );
		assertEquals( true, o );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_isFalse() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_isFalse,
			vf );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_isFalse );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest1._mf_result );
		assertEquals( false, o );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_alwaysWorks() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_alwaysWorks,
			vf );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_alwaysWorks );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest1._mf_result );
		assertEquals( 23, o );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_alwaysFails() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_alwaysFails,
			vf );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_alwaysFails );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest1._mf_result );
		assertTrue( o instanceof _Etch_AuthException );
		_Etch_AuthException e = (_Etch_AuthException) o;
		assertEquals( "stub could not authorize alwaysFails using isFalse",
			e.msg );
	}
	 
	/** @throws Exception */
	@org.junit.Test
	public void method_fillObjectInt() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_fillObject,
			vf );
		msg.put( ValueFactoryTest1._mf_n, 3 );
		msg.put( ValueFactoryTest1._mf_o, 9 );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fillObject );
		assertEquals( 1, src.xreply.size() );
		Object[] o = (Object[]) src.xreply.get( ValueFactoryTest1._mf_result );
		assertNotNull( o );
		assertEquals( 3, o.length );
		for (Object n : o)
			assertEquals( 9, n );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_fillObjectStr() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_fillObject,
			vf );
		msg.put( ValueFactoryTest1._mf_n, 3 );
		msg.put( ValueFactoryTest1._mf_o, "raw" );
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fillObject );
		assertEquals( 1, src.xreply.size() );
		Object[] o = (Object[]) src.xreply.get( ValueFactoryTest1._mf_result );
		assertNotNull( o );
		assertEquals( 3, o.length );
		for (Object n : o)
			assertEquals( "raw", n );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_throwExcp5() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_throwExcp5,
			vf );
		msg.put(ValueFactoryTest1._mf_msg,"raw");
		msg.put(ValueFactoryTest1._mf_code,9);
		msg.put(ValueFactoryTest1._mf_value,"Exception with object as param");
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_throwExcp5 );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest1._mf_result );
		assertTrue( o instanceof Test1.Excp5 );
		Test1.Excp5 e = (Test1.Excp5) o;
		assertEquals( e.msg, "raw" );
		assertEquals( e.code, 9 );
		assertEquals( e.x, "Exception with object as param" );
		
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_throwExcp6() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_throwExcp6,
			vf );
		msg.put(ValueFactoryTest1._mf_msg,"raw");
		msg.put(ValueFactoryTest1._mf_code,99);
		msg.put( ValueFactoryTest1._mf_value, method_fillObjectHelper(5,"Exception with array of object as param"));
		stub.sessionMessage( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_throwExcp6 );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest1._mf_result );
		assertTrue( o instanceof Test1.Excp6 );
		Test1.Excp6 e = (Test1.Excp6) o;
		assertEquals( e.msg, "raw" );
		assertEquals( e.code, 99 );
		Object[] objArr = e.x ;
		assertNotNull( objArr );
		assertEquals( 5, objArr.length );
		for (Object n : objArr)
			assertEquals( "Exception with array of object as param", n );
		
	}
	
	// Helper method to fill an object array of size sz with Object Value
	
	private Object[] method_fillObjectHelper (int sz, Object value)
	{
		Object[] myObject = test.fillObject( sz, value );
		
		for ( int i = 0; i < sz; i++ ) 
		{
			myObject[i] = value;
		}	
		return myObject;
	}

	private final FakeTest1 test = new FakeTest1();

	private final ValueFactoryTest1 vf = new ValueFactoryTest1( "tcp:" );

	private final Pool pool = new Pool()
	{
		public void run( PoolRunnable runnable )
		{
			try
			{
				runnable.run();
			}
			catch ( Exception e )
			{
				throw new RuntimeException( e );
			}
		}
	};

	private final MyMessageSource src = new MyMessageSource();
	private final StubTest1<Test1> stub = new StubTest1<Test1>( src, test, pool, pool );
	
	/** a fake MessageSource for testing */
	public static class MyMessageSource implements DeliveryService
	{
		/** */
		public Message xreply;

		public void transportMessage( Who recipient, Message msg ) throws Exception
		{
			assertNull( recipient );
			assertNull( xreply );
			xreply = msg;
		}

		public Mailbox begincall( Message msg ) throws Exception
		{
			// ignore
			return null;
		}

		public Object endcall( Mailbox mb, Type responseType ) throws Exception
		{
			// ignore
			return null;
		}

		public boolean sessionMessage( Who sender, Message msg )
			throws Exception
		{
			// ignore
			return false;
		}

		public void sessionControl( Object control, Object value )
			throws Exception
		{
			// ignore
			
		}

		public void sessionNotify( Object event ) throws Exception
		{
			// ignore
			
		}

		public Object sessionQuery( Object query ) throws Exception
		{
			// ignore
			return null;
		}

		public SessionMessage getSession()
		{
			// ignore
			return null;
		}

		public void setSession( SessionMessage session )
		{
			// ignore
			
		}

		public void transportControl( Object control, Object value )
			throws Exception
		{
			// ignore
			
		}

		public void transportNotify( Object event ) throws Exception
		{
			// ignore
			
		}

		public Object transportQuery( Object query ) throws Exception
		{
			// ignore
			return null;
		}
	}
}

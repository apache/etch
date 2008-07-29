/*
 * $Id$
 * 
 * Created by wert on Jun 8, 2007.
 * 
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.compiler.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;
import metreos.core.Who;
import etch.bindings.java.msg.Message;
import etch.bindings.java.support.MessageSource;
import etch.bindings.java.support.Pool;
import etch.bindings.java.support._Etch_AuthException;

public class TestStubTestDotJava 
{
	@org.junit.Test
	public void method_nothing() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_nothing,
			vf );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_nothing );
		assertEquals( 0, src.xreply.size() );
	}

	@org.junit.Test
	public void method_incr1() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_incr, vf );
		msg.put( ValueFactoryTest._mf_x, 3 );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_incr );
		assertEquals( 1, src.xreply.size() );
		assertEquals( 4, src.xreply.get( ValueFactoryTest._mf_result ) );
	}

	@org.junit.Test
	public void method_incr2() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_incr, vf );
		// msg.put( ValueFactoryTest._mf_x, 3 ); -- cause an exception
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_incr );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest._mf_result );
		assertTrue( o instanceof RuntimeException ); // because incr is not
		// declared to throw
		// anything
	}

	@org.junit.Test
	public void method_sub() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_sub, vf );
		msg.put( ValueFactoryTest._mf_x, 7 );
		msg.put( ValueFactoryTest._mf_y, 3 );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_sub );
		assertEquals( 1, src.xreply.size() );
		assertEquals( 4, src.xreply.get( ValueFactoryTest._mf_result ) );
	}

	@org.junit.Test
	public void method_sum() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_sum, vf );
		msg.put( ValueFactoryTest._mf_x, new int[]
		{
			1, 2, 3, 7, 11
		} );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_sum );
		assertEquals( 1, src.xreply.size() );
		assertEquals( 24, src.xreply.get( ValueFactoryTest._mf_result ) );
	}

	@org.junit.Test
	public void method_trans1() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_trans,
			vf );
		msg.put( ValueFactoryTest._mf_e, Test.E1.A );
		msg.put( ValueFactoryTest._mf_x, 5 );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_trans );
		assertEquals( 1, src.xreply.size() );
		assertEquals( 2, src.xreply.get( ValueFactoryTest._mf_result ) );
	}

	@org.junit.Test
	public void method_trans2() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_trans,
			vf );
		msg.put( ValueFactoryTest._mf_e, Test.E1.B );
		msg.put( ValueFactoryTest._mf_x, 5 );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_trans );
		assertEquals( 1, src.xreply.size() );
		assertEquals( 10, src.xreply.get( ValueFactoryTest._mf_result ) );
	}

	@org.junit.Test
	public void method_trans3() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_trans,
			vf );
		msg.put( ValueFactoryTest._mf_e, Test.E1.C );
		msg.put( ValueFactoryTest._mf_x, 5 );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_trans );
		assertEquals( 1, src.xreply.size() );
		assertEquals( 12, src.xreply.get( ValueFactoryTest._mf_result ) );
	}

	@org.junit.Test
	public void method_dist1() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_dist, vf );
		msg.put( ValueFactoryTest._mf_a, new Test.S1( 1, 1, 1 ) );
		msg.put( ValueFactoryTest._mf_b, new Test.S1( 0, 0, 0 ) );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_dist );
		assertEquals( 1, src.xreply.size() );
		assertEquals( Math.sqrt( 3 ), src.xreply
			.get( ValueFactoryTest._mf_result ) );
	}

	@org.junit.Test
	public void method_dist2() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_dist, vf );
		msg.put( ValueFactoryTest._mf_a, new Test.S1( 1, 2, 3 ) );
		msg.put( ValueFactoryTest._mf_b, new Test.S1( 6, 5, 4 ) );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_dist );
		assertEquals( 1, src.xreply.size() );
		assertEquals( Math.sqrt( 35 ), src.xreply
			.get( ValueFactoryTest._mf_result ) );
	}

	@org.junit.Test
	public void method_fill() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_fill, vf );
		msg.put( ValueFactoryTest._mf_n, 4 );
		msg.put( ValueFactoryTest._mf_x, 3 );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_fill );
		assertEquals( 1, src.xreply.size() );
		int[] x = (int[]) src.xreply.get( ValueFactoryTest._mf_result );
		assertNotNull( x );
		assertEquals( 4, x.length );
		for (int y : x)
			assertEquals( 3, y );
	}

	@org.junit.Test
	public void method_blow() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_blow, vf );
		msg.put( ValueFactoryTest._mf_msg, "foo" );
		msg.put( ValueFactoryTest._mf_code, 23 );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_blow );
		assertEquals( 1, src.xreply.size() );
		Test.Excp1 e = (Test.Excp1) src.xreply
			.get( ValueFactoryTest._mf_result );
		assertNotNull( e );
		assertEquals( "foo", e.msg );
		assertEquals( 23, e.code );
	}

	@org.junit.Test
	public void method_beets1() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_beets,
			vf );
		msg.put( ValueFactoryTest._mf_e, Test.E1.A );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_beets );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest._mf_result );
		assertEquals( 5, o );
	}

	@org.junit.Test
	public void method_beets2() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_beets,
			vf );
		msg.put( ValueFactoryTest._mf_e, Test.E1.B );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_beets );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest._mf_result );
		assertEquals( Test.Excp3.class, o.getClass() );
	}

	@org.junit.Test
	public void method_beets3() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_beets,
			vf );
		msg.put( ValueFactoryTest._mf_e, Test.E1.C );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_beets );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest._mf_result );
		assertEquals( Test.Excp4.class, o.getClass() );
	}

	@org.junit.Test
	public void method_beets4() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_beets,
			vf );
		msg.put( ValueFactoryTest._mf_e, null );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_beets );
		assertEquals( 0, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest._mf_result );
		assertNull( o );
	}

	@org.junit.Test
	public void method_isTrue() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_isTrue,
			vf );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_isTrue );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest._mf_result );
		assertEquals( true, o );
	}

	@org.junit.Test
	public void method_isFalse() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_isFalse,
			vf );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_isFalse );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest._mf_result );
		assertEquals( false, o );
	}

	@org.junit.Test
	public void method_alwaysWorks() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_alwaysWorks,
			vf );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_alwaysWorks );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest._mf_result );
		assertEquals( 23, o );
	}

	@org.junit.Test
	public void method_alwaysFails() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_alwaysFails,
			vf );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_alwaysFails );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest._mf_result );
		assertTrue( o instanceof _Etch_AuthException );
		_Etch_AuthException e = (_Etch_AuthException) o;
		assertEquals( "stub could not authorize alwaysFails using isFalse",
			e.msg );
	}
	 
	@org.junit.Test
	public void method_fillObjectInt() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_fillObject,
			vf );
		msg.put( ValueFactoryTest._mf_n, 3 );
		msg.put( ValueFactoryTest._mf_o, 9 );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_fillObject );
		assertEquals( 1, src.xreply.size() );
		Object[] o = (Object[]) src.xreply.get( ValueFactoryTest._mf_result );
		assertNotNull( o );
		assertEquals( 3, o.length );
		for (Object n : o)
			assertEquals( 9, n );
	}

	@org.junit.Test
	public void method_fillObjectStr() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_fillObject,
			vf );
		msg.put( ValueFactoryTest._mf_n, 3 );
		msg.put( ValueFactoryTest._mf_o, "raw" );
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_fillObject );
		assertEquals( 1, src.xreply.size() );
		Object[] o = (Object[]) src.xreply.get( ValueFactoryTest._mf_result );
		assertNotNull( o );
		assertEquals( 3, o.length );
		for (Object n : o)
			assertEquals( "raw", n );
	}
	
	@org.junit.Test
	public void method_throwExcp5() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_throwExcp5,
			vf );
		msg.put(ValueFactoryTest._mf_msg,"raw");
		msg.put(ValueFactoryTest._mf_code,9);
		msg.put(ValueFactoryTest._mf_value,"Exception with object as param");
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_throwExcp5 );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest._mf_result );
		assertTrue( o instanceof Test.Excp5 );
		Test.Excp5 e = (Test.Excp5) o;
		assertEquals( e.msg, "raw" );
		assertEquals( e.code, 9 );
		assertEquals( e.x, "Exception with object as param" );
		
	}

	@org.junit.Test
	public void method_throwExcp6() throws Exception
	{
		Message msg = new Message(
			ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test_throwExcp6,
			vf );
		msg.put(ValueFactoryTest._mf_msg,"raw");
		msg.put(ValueFactoryTest._mf_code,99);
		msg.put( ValueFactoryTest._mf_value, method_fillObjectHelper(5,"Exception with array of object as param"));
		stub.message( null, msg );
		// check the result.
		src.xreply
			.checkType( ValueFactoryTest._mt_etch_bindings_java_compiler_test_Test__result_throwExcp6 );
		assertEquals( 1, src.xreply.size() );
		Object o = src.xreply.get( ValueFactoryTest._mf_result );
		assertTrue( o instanceof Test.Excp6 );
		Test.Excp6 e = (Test.Excp6) o;
		assertEquals( e.msg, "raw" );
		assertEquals( e.code, 99 );
		Object[] objArr = (Object[]) e.x ;
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

	private final FakeTest test = new FakeTest();

	private final ValueFactoryTest vf = new ValueFactoryTest();

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
	private final StubTest stub = new StubTest( src, test, pool, pool );
	
	public static class MyMessageSource implements MessageSource
	{
		public Message xreply;

		public void messagex( Who recipient, Message msg ) throws Exception
		{
			assertNull( recipient );
			assertNull( xreply );
			xreply = msg;
		}
	}
}

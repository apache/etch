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

package org.apache.etch.bindings.java.support;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import org.apache.etch.bindings.java.msg.AsyncMode;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.transport.SessionMessage;
import org.apache.etch.util.core.Who;
import org.junit.Before;
import org.junit.Test;


/** Test StubBase */
public class TestStubBase
{
	private final ValueFactory vf = new DummyValueFactory();
	private final MyDeliveryService transport = new MyDeliveryService();
	private final MyObject session = new MyObject();
	private final MyPool queued = new MyPool();
	private final MyPool free = new MyPool();
	private final MyStub stub = new MyStub( transport, session, queued, free );
	private final Who who = new Who() { /* */ };
	private final Type howdy = new Type( "howdy" );
	private final Type nogood = new Type( "nogood" );
	
	/** */
	@Before
	public void setup()
	{
		howdy.setStubHelper(
			new StubHelper<MyObject>()
			{
				@Override
				public void run( DeliveryService svc, MyObject obj, Who sender,
					Message msg ) throws Exception
				{
					obj.howdy( svc, sender, msg );
				}
			} );
	}
	
	/** @throws Exception */
	@Test
	public void construction() throws Exception
	{
		assertSame( stub, transport.getSession() );
	}
	
	/** @throws Exception */
	@Test
	public void sessionMessage1() throws Exception
	{
		howdy.setAsyncMode( AsyncMode.NONE );
		
		Message msg = new Message( howdy, vf );
		boolean msg_handled = stub.sessionMessage( who, msg );
		assertTrue( msg_handled );
		
		assertSame( What.HOWDY, session.what );
		assertSame( transport, session.svc );
		assertSame( who, session.sender );
		assertSame( msg, session.msg );
		assertFalse( queued.run );
		assertFalse( free.run );
	}
	
	/** @throws Exception */
	@Test
	public void sessionMessage2() throws Exception
	{
		howdy.setAsyncMode( AsyncMode.QUEUED );
		
		Message msg = new Message( howdy, vf );
		boolean msg_handled = stub.sessionMessage( who, msg );
		assertTrue( msg_handled );
		
		assertSame( What.HOWDY, session.what );
		assertSame( transport, session.svc );
		assertSame( who, session.sender );
		assertSame( msg, session.msg );
		assertTrue( queued.run );
		assertFalse( free.run );
	}
	
	/** @throws Exception */
	@Test
	public void sessionMessage3() throws Exception
	{
		howdy.setAsyncMode( AsyncMode.FREE );
		
		Message msg = new Message( howdy, vf );
		boolean msg_handled = stub.sessionMessage( who, msg );
		assertTrue( msg_handled );
		
		assertSame( What.HOWDY, session.what );
		assertSame( transport, session.svc );
		assertSame( who, session.sender );
		assertSame( msg, session.msg );
		assertFalse( queued.run );
		assertTrue( free.run );
	}
	
	/** @throws Exception */
	@Test
	public void sessionMessage4() throws Exception
	{
		howdy.setAsyncMode( AsyncMode.NONE );
		
		Message msg = new Message( nogood, vf );
		boolean msg_handled = stub.sessionMessage( who, msg );
		assertFalse( msg_handled );
		
		assertNull( session.what );
		assertNull( session.svc );
		assertNull( session.sender );
		assertNull( session.msg );
		assertFalse( queued.run );
		assertFalse( free.run );
	}

	/** @throws Exception */
	@Test
	public void sessionQuery() throws Exception
	{
		Object QUERY = "foo";
		Object RESULT = "bar";
		
		assertNull( session.what );
		assertNull( session.query );
		session.query_result = RESULT;
		
		Object result = stub.sessionQuery( QUERY );

		assertSame( What.SESSION_QUERY, session.what );
		assertSame( QUERY, session.query );
		assertSame( RESULT, result );
	}

	/** @throws Exception */
	@Test
	public void sessionControl() throws Exception
	{
		Object CONTROL = "foo";
		Object VALUE = "bar";
		
		assertNull( session.what );
		assertNull( session.control );
		assertNull( session.value );
		
		stub.sessionControl( CONTROL, VALUE );

		assertSame( What.SESSION_CONTROL, session.what );
		assertSame( CONTROL, session.control );
		assertSame( VALUE, session.value );
	}

	/** @throws Exception */
	@Test
	public void sessionNotify() throws Exception
	{
		Object EVENT = "foo";
		
		assertNull( session.what );
		assertNull( session.event );
		
		stub.sessionNotify( EVENT );

		assertSame( What.SESSION_NOTIFY, session.what );
		assertSame( EVENT, session.event );
	}
	
	/** */
	enum What
	{
		/** */ TRANSPORT_MESSAGE,
		/** */ TRANSPORT_QUERY,
		/** */ TRANSPORT_CONTROL,
		/** */ TRANSPORT_NOTIFY,
		/** */ SESSION_QUERY,
		/** */ SESSION_CONTROL,
		/** */ SESSION_NOTIFY,
		/** */ HOWDY
	}
	
	/**
	 * MyDeliveryService (fake for testing).
	 */
	@SuppressWarnings("hiding")
	static class MyDeliveryService implements DeliveryService
	{
		/** */ public What what;
		/** */ public Message msg;
		/** */ public Who recipient;
		/** */ public Object query;
		/** */ public boolean query_result;
		/** */ public Object control;
		/** */ public Object value;
		/** */ public Object event;
		
		public Mailbox begincall( Message msg ) throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public Object endcall( Mailbox mb, Type responseType ) throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public boolean sessionMessage( Who sender, Message msg )
			throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public void sessionControl( Object control, Object value )
			throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public void sessionNotify( Object event ) throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public Object sessionQuery( Object query ) throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public void transportMessage( Who recipient, Message msg )
			throws Exception
		{
			what = What.TRANSPORT_MESSAGE;
			this.recipient = recipient;
			this.msg = msg;
		}

		public SessionMessage getSession()
		{
			return session;
		}

		public void setSession( SessionMessage session )
		{
			this.session = session;
		}
		
		private SessionMessage session;

		public Object transportQuery( Object query ) throws Exception
		{
			what = What.TRANSPORT_QUERY;
			this.query = query;
			return query_result;
		}

		public void transportControl( Object control, Object value )
			throws Exception
		{
			what = What.TRANSPORT_CONTROL;
			this.control = control;
			this.value = value;
		}

		public void transportNotify( Object event ) throws Exception
		{
			what = What.TRANSPORT_NOTIFY;
			this.event = event;
		}
	}
	
	/**
	 * MyStub (fake for testing).
	 */
	static class MyStub extends StubBase<MyObject>
	{
		/**
		 * @param svc
		 * @param obj
		 * @param queued
		 * @param free
		 */
		public MyStub( DeliveryService svc, MyObject obj, Pool queued, Pool free )
		{
			super( svc, obj, queued, free );
		}
	}
	
	/**
	 * MyObject (fake for testing).
	 */
	@SuppressWarnings("hiding")
	static class MyObject implements ObjSession
	{
		/** */ public What what;
		/** */ public Object query;
		/** */ public Object query_result;
		/** */ public Object control;
		/** */ public Object value;
		/** */ public Object event;
		/** */ public DeliveryService svc;
		/** */ public Who sender;
		/** */ public Message msg;
		
		public Object _sessionQuery( Object query ) throws Exception
		{
			what = What.SESSION_QUERY;
			this.query = query;
			return query_result;
		}
		
		/**
		 * @param svc
		 * @param sender
		 * @param msg
		 */
		public void howdy( DeliveryService svc, Who sender, Message msg )
		{
			what = What.HOWDY;
			this.svc = svc;
			this.sender = sender;
			this.msg = msg;
		}

		public void _sessionControl( Object control, Object value )
			throws Exception
		{
			what = What.SESSION_CONTROL;
			this.control = control;
			this.value = value;
		}

		public void _sessionNotify( Object event ) throws Exception
		{
			what = What.SESSION_NOTIFY;
			this.event = event;
		}
	}
	
	/** */
	static class MyPool implements Pool
	{
		/** */ public boolean run;
		
		public void run( PoolRunnable runnable ) throws Exception
		{
			run = true;
			runnable.run();
		}
	}
}

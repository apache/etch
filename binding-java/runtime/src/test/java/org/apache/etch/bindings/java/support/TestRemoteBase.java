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

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.TypeMap;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.transport.SessionMessage;
import org.apache.etch.util.core.Who;
import org.apache.etch.util.core.io.Transport;
import org.apache.etch.util.core.io.Transport.WaitDown;
import org.apache.etch.util.core.io.Transport.WaitUp;
import org.junit.Test;


/**
 * Test of RemoteBase.
 */
public class TestRemoteBase
{
	private final MyDeliveryService transport = new MyDeliveryService();
	private final ValueFactory vf = new MyDefaultValueFactory( "tcp:" );
	private final RemoteBase remote = new RemoteBase( transport, vf );
	private final Type mt = new Type( "foo" );
	private final Type rmt = new Type( "bar" );
	private final Mailbox mb = new MyMailbox();
	
	/**
	 * Constructs TestRemoteBase.
	 */
	public TestRemoteBase()
	{
//		assertSame( transport.getSession(), remote );
		
//		mt.putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
//		mt.setResult( rmt );
		
//		rmt.setResponseField( DefaultValueFactory._mf_result );
//		rmt.putValidator( DefaultValueFactory._mf_result, etch.bindings.java.support.Validator_int.get( 0 ) );
//		rmt.putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
//		rmt.putValidator( DefaultValueFactory._mf__inReplyTo, Validator_long.get( 0 ) );
	}

	/** @throws Exception */
	@Test
	public void newMessage() throws Exception
	{
		Message msg = remote._newMessage( mt );
		msg.checkType( mt );
		assertSame( vf, msg.vf() );
	}

	/** @throws Exception */
	@Test
	public void send() throws Exception
	{
		Message msg = remote._newMessage( mt );
		
		remote._send( msg );
		
		assertSame( What.TRANSPORT_MESSAGE, transport.what );
		assertNull( transport.recipient );
		assertSame( msg, transport.msg );
	}

	/** @throws Exception */
	@Test
	public void begincall() throws Exception
	{
		Message msg = remote._newMessage( mt );
		
		transport.mb = mb;
		
		Mailbox rmb = remote._begincall( msg );
		
		assertSame( What.BEGIN_CALL, transport.what );
		assertSame( msg, transport.msg );
		assertSame( mb, rmb );
	}
	
	/** @throws Exception */
	@Test
	public void endcall() throws Exception
	{
		Object obj = new Object();
		
		transport.result = obj;
		
		Object robj = remote._endcall( mb, rmt );
		
		assertSame( What.END_CALL, transport.what );
		assertSame( mb, transport.mb );
		assertSame( rmt, transport.responseType );
		assertSame( obj, robj );
	}

	/** @throws Exception */
	@Test
	public void transportQuery() throws Exception
	{
		Object QUERY = "foo";
		Object RESULT = "bar";
		
		assertNull( transport.what );
		assertNull( transport.query );
		transport.query_result = RESULT;
		
		Object result = remote._transportQuery( QUERY );

		assertSame( What.TRANSPORT_QUERY, transport.what );
		assertSame( QUERY, transport.query );
		assertSame( RESULT, result );
	}

	/** @throws Exception */
	@Test
	public void transportControl() throws Exception
	{
		Object CONTROL = "foo";
		Object VALUE = "bar";
		
		assertNull( transport.what );
		assertNull( transport.control );
		assertNull( transport.value );
		
		remote._transportControl( CONTROL, VALUE );

		assertSame( What.TRANSPORT_CONTROL, transport.what );
		assertSame( CONTROL, transport.control );
		assertSame( VALUE, transport.value );
	}

	/** @throws Exception */
	@Test
	public void transportNotify() throws Exception
	{
		Object EVENT = "foo";
		
		assertNull( transport.what );
		assertNull( transport.event );
		
		remote._transportNotify( EVENT );

		assertSame( What.TRANSPORT_NOTIFY, transport.what );
		assertSame( EVENT, transport.event );
	}
	
	/** @throws Exception */
	@Test
	public void start() throws Exception
	{
		assertNull( transport.what );
		assertNull( transport.query );
		assertNull( transport.control );
		assertNull( transport.value );
		assertNull( transport.event );
		
		remote._start();
		
		assertSame( What.TRANSPORT_CONTROL, transport.what );
		assertNull( transport.query );
		assertSame( Transport.START, transport.control );
		assertNull( transport.value );
		assertNull( transport.event );
	}
	
	/** @throws Exception */
	@Test
	public void waitUp() throws Exception
	{
		assertNull( transport.what );
		assertNull( transport.query );
		assertNull( transport.control );
		assertNull( transport.value );
		assertNull( transport.event );
		
		remote._waitUp( 1234 );
		
		assertSame( What.TRANSPORT_QUERY, transport.what );
		assertNotNull( transport.query );
		assertNull( transport.control );
		assertNull( transport.value );
		assertNull( transport.event );
		
		assertSame( WaitUp.class, transport.query.getClass() );
		assertEquals( 1234, ((WaitUp) transport.query).maxDelay );
	}
	
	/** @throws Exception */
	@Test
	public void startAndWaitUp() throws Exception
	{
		assertNull( transport.what );
		assertNull( transport.query );
		assertNull( transport.control );
		assertNull( transport.value );
		assertNull( transport.event );
		
		remote._startAndWaitUp( 2345 );
		
		assertSame( What.TRANSPORT_QUERY, transport.what );
		assertNotNull( transport.query );
		assertSame( Transport.START, transport.control );
		assertNull( transport.value );
		assertNull( transport.event );
		
		assertSame( WaitUp.class, transport.query.getClass() );
		assertEquals( 2345, ((WaitUp) transport.query).maxDelay );
	}
	
	/** @throws Exception */
	@Test
	public void stop() throws Exception
	{
		assertNull( transport.what );
		assertNull( transport.query );
		assertNull( transport.control );
		assertNull( transport.value );
		assertNull( transport.event );
		
		remote._stop();
		
		assertSame( What.TRANSPORT_CONTROL, transport.what );
		assertNull( transport.query );
		assertSame( Transport.STOP, transport.control );
		assertNull( transport.value );
		assertNull( transport.event );
	}
	
	/** @throws Exception */
	@Test
	public void waitDown() throws Exception
	{
		assertNull( transport.what );
		assertNull( transport.query );
		assertNull( transport.control );
		assertNull( transport.value );
		assertNull( transport.event );
		
		remote._waitDown( 3456 );
		
		assertSame( What.TRANSPORT_QUERY, transport.what );
		assertNotNull( transport.query );
		assertNull( transport.control );
		assertNull( transport.value );
		assertNull( transport.event );
		
		assertSame( WaitDown.class, transport.query.getClass() );
		assertEquals( 3456, ((WaitDown) transport.query).maxDelay );
	}
	
	/** @throws Exception */
	@Test
	public void stopAndWaitDown() throws Exception
	{
		assertNull( transport.what );
		assertNull( transport.query );
		assertNull( transport.control );
		assertNull( transport.value );
		assertNull( transport.event );
		
		remote._stopAndWaitDown( 4567 );
		
		assertSame( What.TRANSPORT_QUERY, transport.what );
		assertNotNull( transport.query );
		assertSame( Transport.STOP, transport.control );
		assertNull( transport.value );
		assertNull( transport.event );
		
		assertSame( WaitDown.class, transport.query.getClass() );
		assertEquals( 4567, ((WaitDown) transport.query).maxDelay );
	}

	/** */
	public enum What
	{
		/** */ TRANSPORT_MESSAGE,
		/** */ TRANSPORT_QUERY,
		/** */ TRANSPORT_CONTROL,
		/** */ TRANSPORT_NOTIFY,
		/** */ BEGIN_CALL,
		/** */ END_CALL
	}
	
	/**
	 * DeliveryService for testing.
	 */
	static class MyDeliveryService implements DeliveryService
	{
		/** */ public What what;
		/** */ public Who recipient;
		/** */ public Message msg;
		/** */ public Object query;
		/** */ public Object query_result;
		/** */ public Object control;
		/** */ public Object value;
		/** */ public Object event;
		/** */ public Mailbox mb;
		/** */ public Type responseType;
		/** */ public Object result;
		
		public Mailbox begincall( Message msg ) throws Exception
		{
			what = What.BEGIN_CALL;
			this.msg = msg;
			return mb;
		}

		public Object endcall( Mailbox mb, Type responseType ) throws Exception
		{
			what = What.END_CALL;
			this.mb = mb;
			this.responseType = responseType;
			return result;
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

		public SessionMessage getSession()
		{
			throw new UnsupportedOperationException();
		}

		public void setSession( SessionMessage session )
		{
			throw new UnsupportedOperationException();
		}
	}

	/**
	 * A fake implementation of DefaultValueFactory for testing.
	 */
	static class MyDefaultValueFactory extends DefaultValueFactory
	{
		private final static TypeMap types = new TypeMap();
		
		private final static Class2TypeMap class2type = new Class2TypeMap();

		static
		{
			DefaultValueFactory.init( types, class2type );
		}

		/**
		 * Constructs the ValueFactoryFake.
		 * @param uri 
		 */
		public MyDefaultValueFactory( String uri )
		{
			super( uri, types, class2type );
		}
	}
	
	static class MyMailbox implements Mailbox
	{
		public boolean closeDelivery()
		{
			throw new UnsupportedOperationException();
		}

		public boolean closeRead() throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public Long getMessageId()
		{
			throw new UnsupportedOperationException();
		}

		public boolean message( Who sender, Message msg ) throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public Element read() throws InterruptedException
		{
			throw new UnsupportedOperationException();
		}

		public Element read( int maxDelay ) throws InterruptedException
		{
			throw new UnsupportedOperationException();
		}

		public void registerNotify( Notify notify, Object state, int maxDelay )
		{
			throw new UnsupportedOperationException();
		}

		public void unregisterNotify( Notify notify )
		{
			throw new UnsupportedOperationException();
		}

		public boolean isClosed()
		{
			throw new UnsupportedOperationException();
		}

		public boolean isEmpty()
		{
			throw new UnsupportedOperationException();
		}

		public boolean isFull()
		{
			throw new UnsupportedOperationException();
		}
	}
}

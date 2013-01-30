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

package org.apache.etch.bindings.java.transport;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.TypeMap;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.Class2TypeMap;
import org.apache.etch.bindings.java.support.DefaultValueFactory;
import org.apache.etch.bindings.java.support.Mailbox;
import org.apache.etch.bindings.java.support.Validator_long;
import org.apache.etch.util.core.Who;
import org.apache.etch.util.core.io.Session;
import org.junit.Test;


/** Tests MailboxManager */
public class TestPlainMailboxManager
{
	private final ValueFactory vf = new MyValueFactory( "tcp:" );
	private final Who who = new Who() {};
	private final MyTransportMessage transport = new MyTransportMessage();
	private final MySessionMessage session = new MySessionMessage();
	private final PlainMailboxManager mmgr = new PlainMailboxManager( transport, "", null );
	{
		mmgr.setSession( session );
	}
	
	private Message constructAddMessage() throws Exception
	{
		return new Message( MyValueFactory.mt_add, vf );
	}
	
	private Message constructAddResultMessage() throws Exception
	{
		return new Message( MyValueFactory.mt_add_result, vf );
	}
	
	/** @throws Exception */
	@Test
	public void construction() throws Exception
	{
		assertSame( mmgr, transport.getSession() );
		assertSame( session, mmgr.getSession() );
		assertSame( transport, mmgr.getTransport() );
	}

	/** @throws Exception */
	@Test
	public void transportMessage1() throws Exception
	{
		// test sending an event message
		
		assertNull( transport.what );
		assertNull( transport.recipient );
		assertNull( transport.msg );
		
		Message msg = constructAddMessage();
		assertNull( msg.getMessageId() );
		assertNull( msg.getInReplyTo() );
		
		mmgr.transportMessage( who, msg );
		
		assertSame( transport.what, What.TRANSPORT_MESSAGE );
		assertSame( transport.recipient, who );
		assertSame( transport.msg, msg );
		
		assertEquals( 0, mmgr.size() );
		assertNotNull( msg.getMessageId() );
		assertNull( msg.getInReplyTo() );
		assertNull( mmgr.getMailbox( msg.getMessageId() ) );
	}

	/** @throws Exception */
	@Test
	public void transportMessage2() throws Exception
	{
		// test sending a result message
		
		assertNull( transport.what );
		assertNull( transport.recipient );
		assertNull( transport.msg );
		
		Message msg = constructAddResultMessage();
		assertNull( msg.getMessageId() );
		assertNull( msg.getInReplyTo() );
		msg.setInReplyTo( 1L );
		
		mmgr.transportMessage( who, msg );
		
		assertSame( transport.what, What.TRANSPORT_MESSAGE );
		assertSame( transport.recipient, who );
		assertSame( transport.msg, msg );
		
		assertEquals( 0, mmgr.size() );
		assertNotNull( msg.getMessageId() );
		assertEquals( 1L, msg.getInReplyTo() );
		assertNull( mmgr.getMailbox( msg.getMessageId() ) );
	}

	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void transportMessage3() throws Exception
	{
		// test sending a message that has already been sent (has a message id)
		
		assertNull( transport.what );
		assertNull( transport.recipient );
		assertNull( transport.msg );
		
		Message msg = constructAddMessage();
		assertNull( msg.getMessageId() );
		msg.setMessageId( 1L );

		// this should trigger msg already sent
		mmgr.transportMessage( who, msg );
	}

	/** @throws Exception */
	@Test
	public void transportCall1() throws Exception
	{
		// test sending a call message
		
		mmgr.sessionNotify( Session.UP );
		
		assertNull( transport.what );
		assertNull( transport.recipient );
		assertNull( transport.msg );
		
		Message msg = constructAddMessage();
		assertNull( msg.getMessageId() );
		assertNull( msg.getInReplyTo() );
		
		Mailbox mb = mmgr.transportCall( who, msg );
		
		assertSame( transport.what, What.TRANSPORT_MESSAGE );
		assertSame( transport.recipient, who );
		assertSame( transport.msg, msg );
		assertNotNull( mb );
		
		assertEquals( 1, mmgr.size() );
		assertNotNull( msg.getMessageId() );
		assertNull( msg.getInReplyTo() );
		assertSame( mb, mmgr.getMailbox( msg.getMessageId() ) );
		
		mb.closeRead();
		
		assertEquals( 0, mmgr.size() );
		assertNull( mmgr.getMailbox( msg.getMessageId() ) );
	}

	/** @throws Exception */
	@Test
	public void transportCall2() throws Exception
	{
		// test sending a call message
		
		mmgr.sessionNotify( Session.UP );
		
		assertNull( transport.what );
		assertNull( transport.recipient );
		assertNull( transport.msg );
		
		Message msg = constructAddMessage();
		assertNull( msg.getMessageId() );
		assertNull( msg.getInReplyTo() );
		
		Mailbox mb = mmgr.transportCall( who, msg );
		
		assertSame( transport.what, What.TRANSPORT_MESSAGE );
		assertSame( transport.recipient, who );
		assertSame( transport.msg, msg );
		assertNotNull( mb );
		
		assertEquals( 1, mmgr.size() );
		assertNotNull( msg.getMessageId() );
		assertNull( msg.getInReplyTo() );
		assertSame( mb, mmgr.getMailbox( msg.getMessageId() ) );
		
		mb.closeDelivery();
		
		assertEquals( 0, mmgr.size() );
		assertNull( mmgr.getMailbox( msg.getMessageId() ) );
	}

	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void transportCall3() throws Exception
	{
		// test sending a call message that has already been sent (has a message id)
		
		assertNull( transport.what );
		assertNull( transport.recipient );
		assertNull( transport.msg );
		
		Message msg = constructAddMessage();
		assertNull( msg.getMessageId() );
		assertNull( msg.getInReplyTo() );
		msg.setMessageId( 1L );

		try
		{
			// this should trigger msg already sent
			mmgr.transportCall( who, msg );
		}
		finally
		{
			assertEquals( 0, mmgr.size() );
			assertNull( mmgr.getMailbox( msg.getMessageId() ) );
		}
	}

	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void transportCall4() throws Exception
	{
		assertNull( transport.what );
		assertNull( transport.recipient );
		assertNull( transport.msg );
		
		Message msg = constructAddResultMessage();
		assertNull( msg.getMessageId() );
		assertNull( msg.getInReplyTo() );
		msg.setInReplyTo( 1L );
		
		try
		{
			// this should trigger message is marked as a reply
			mmgr.transportCall( who, msg );
		}
		finally
		{
			assertEquals( 0, mmgr.size() );
			assertNull( mmgr.getMailbox( msg.getMessageId() ) );
		}
	}

	/////////////////////////////
	// tests of sessionMessage //
	/////////////////////////////
	
	/** @throws Exception */
	@Test
	public void sessionMessage1() throws Exception
	{
		// message without an inReplyTo, session wants it.
		
		assertNull( session.what );
		assertNull( session.sender );
		assertNull( session.msg );
		
		Message msg = constructAddMessage();
		
		session.msg_handled = true;
		boolean msg_handled = mmgr.sessionMessage( who, msg );
		assertTrue( msg_handled );
		
		assertSame( What.SESSION_MESSAGE, session.what );
		assertSame( who, session.sender );
		assertSame( msg, session.msg );
	}
	
	/** @throws Exception */
	@Test
	public void sessionMessage2() throws Exception
	{
		// message without an inReplyTo, session doesn't want it.
		
		assertNull( session.what );
		assertNull( session.sender );
		assertNull( session.msg );
		
		Message msg = constructAddMessage();
		
		session.msg_handled = false;
		boolean msg_handled = mmgr.sessionMessage( who, msg );
		assertFalse( msg_handled );
		
		assertSame( What.SESSION_MESSAGE, session.what );
		assertSame( who, session.sender );
		assertSame( msg, session.msg );
	}
	
	/** @throws Exception */
	@Test
	public void sessionMessage3() throws Exception
	{
		// message with an inReplyTo which doesn't match a mailbox
		
		assertNull( session.what );
		assertNull( session.sender );
		assertNull( session.msg );
		
		Message msg = constructAddResultMessage();
		msg.setInReplyTo( 1L );
		
		session.msg_handled = true;
		boolean msg_handled = mmgr.sessionMessage( who, msg );
		assertFalse( msg_handled );

		assertNull( session.what );
		assertNull( session.sender );
		assertNull( session.msg );
	}
	
	/** @throws Exception */
	@Test
	public void sessionMessage4() throws Exception
	{
		// message with an inReplyTo which doesn't match a mailbox
		
		assertNull( session.what );
		assertNull( session.sender );
		assertNull( session.msg );
		
		Message msg = constructAddResultMessage();
		msg.setInReplyTo( 1L );
		
		session.msg_handled = false;
		boolean msg_handled = mmgr.sessionMessage( who, msg );
		assertFalse( msg_handled );

		assertNull( session.what );
		assertNull( session.sender );
		assertNull( session.msg );
	}

	/** @throws Exception */
	@Test
	public void sessionMessage5() throws Exception
	{
		// message with an inReplyTo which matches a mailbox which is open
		
		mmgr.sessionNotify( Session.UP );
		session.what = null;
		session.event = null;
		
		assertNull( session.what );
		assertNull( session.sender );
		assertNull( session.msg );
		
		assertNull( transport.what );
		assertNull( transport.recipient );
		assertNull( transport.msg );
		
		Message msg = constructAddMessage();
		Mailbox mb = mmgr.transportCall( who, msg );
		
		assertSame( transport.what, What.TRANSPORT_MESSAGE );
		assertSame( transport.recipient, who );
		assertSame( transport.msg, msg );
		assertNotNull( mb );
		
		assertEquals( 1, mmgr.size() );
		assertNotNull( msg.getMessageId() );
		assertNull( msg.getInReplyTo() );
		assertSame( mb, mmgr.getMailbox( msg.getMessageId() ) );
		
		Message rmsg = msg.reply( MyValueFactory.mt_add_result );
		boolean msg_handled = mmgr.sessionMessage( who, rmsg );
		assertTrue( msg_handled );
		
		assertNull( session.what );
		assertNull( session.sender );
		assertNull( session.msg );
		
		Mailbox.Element x = mb.read();
		assertSame( who, x.sender );
		assertSame( rmsg, x.msg );
		
		assertEquals( 1, mmgr.size() );
		assertSame( mb, mmgr.getMailbox( msg.getMessageId() ) );
		
		mb.closeRead();
		
		assertEquals( 0, mmgr.size() );
		assertNull( mmgr.getMailbox( msg.getMessageId() ) );
	}

	/** @throws Exception */
	@Test
	public void sessionMessage6() throws Exception
	{
		// message with an inReplyTo which matches a mailbox which is open
		
		mmgr.sessionNotify( Session.UP );
		session.what = null;
		session.event = null;
		
		assertNull( session.what );
		assertNull( session.sender );
		assertNull( session.msg );
		
		assertNull( transport.what );
		assertNull( transport.recipient );
		assertNull( transport.msg );
		
		Message msg = constructAddMessage();
		Mailbox mb = mmgr.transportCall( who, msg );
		
		assertSame( transport.what, What.TRANSPORT_MESSAGE );
		assertSame( transport.recipient, who );
		assertSame( transport.msg, msg );
		assertNotNull( mb );
		
		assertEquals( 1, mmgr.size() );
		assertNotNull( msg.getMessageId() );
		assertNull( msg.getInReplyTo() );
		assertSame( mb, mmgr.getMailbox( msg.getMessageId() ) );
		
		mb.closeRead();
		
		assertEquals( 0, mmgr.size() );
		assertNull( mmgr.getMailbox( msg.getMessageId() ) );
		
		Message rmsg = msg.reply( MyValueFactory.mt_add_result );
		boolean msg_handled = mmgr.sessionMessage( who, rmsg );
		assertFalse( msg_handled );
		
		assertNull( session.what );
		assertNull( session.sender );
		assertNull( session.msg );
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
		
		Object result = mmgr.sessionQuery( QUERY );

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
		
		mmgr.sessionControl( CONTROL, VALUE );

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
		
		mmgr.sessionNotify( EVENT );

		assertSame( What.SESSION_NOTIFY, session.what );
		assertSame( EVENT, session.event );
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
		
		Object result = mmgr.transportQuery( QUERY );

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
		
		mmgr.transportControl( CONTROL, VALUE );

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
		
		mmgr.transportNotify( EVENT );

		assertSame( What.TRANSPORT_NOTIFY, transport.what );
		assertSame( EVENT, transport.event );
	}

	/** */
	public enum What
	{
		/** */ SESSION_MESSAGE,
		/** */ SESSION_QUERY,
		/** */ SESSION_CONTROL,
		/** */ SESSION_NOTIFY,
		/** */ TRANSPORT_MESSAGE,
		/** */ TRANSPORT_QUERY,
		/** */ TRANSPORT_CONTROL,
		/** */ TRANSPORT_NOTIFY
	}
	
	/** */
	public static class MyTransportMessage implements TransportMessage
	{
		/** */ public What what;
		/** */ public Who recipient;
		/** */ public Message msg;
		/** */ public Object query;
		/** */ public Object query_result;
		/** */ public Object control;
		/** */ public Object value;
		/** */ public Object event;
		
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
			return session;
		}

		public void setSession( SessionMessage session )
		{
			this.session = session;
		}
		
		private SessionMessage session;
	}
	
	/** */
	public static class MySessionMessage implements SessionMessage
	{
		/** */ public What what;
		/** */ public Who sender;
		/** */ public Message msg;
		/** */ public boolean msg_handled;
		/** */ public Object query;
		/** */ public Object query_result;
		/** */ public Object control;
		/** */ public Object value;
		/** */ public Object event;
		
		public boolean sessionMessage( Who sender, Message msg ) throws Exception
		{
			what = What.SESSION_MESSAGE;
			this.sender = sender;
			this.msg = msg;
			return msg_handled;
		}

		public Object sessionQuery( Object query )
		{
			what = What.SESSION_QUERY;
			this.query = query;
			return query_result;
		}

		public void sessionControl( Object control, Object value )
		{
			what = What.SESSION_CONTROL;
			this.control = control;
			this.value = value;
		}

		public void sessionNotify( Object event )
		{
			what = What.SESSION_NOTIFY;
			this.event = event;
		}
	}
	
	/** Private value factory for testing. */
	public static class MyValueFactory extends DefaultValueFactory
	{
		/**
		 * Construct MyValueFactory.
		 * @param uri 
		 */
		public MyValueFactory( String uri )
		{
			super( uri, types, class2type );
		}
		
		private final static TypeMap types = new TypeMap();
		
		private final static Class2TypeMap class2type = new Class2TypeMap();

		static
		{
			DefaultValueFactory.init( types, class2type );
		}

		/** */
		public final static Type mt_add = types.get( "add" );
		
		/** */
		public final static Type mt_add_result = types.get( "add_result" );
		
		static
		{
			mt_add.putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
			
			mt_add_result.putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
			mt_add_result.putValidator( DefaultValueFactory._mf__inReplyTo, Validator_long.get( 0 ) );
		}
	}
}

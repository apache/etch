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
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.TypeMap;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.Class2TypeMap;
import org.apache.etch.bindings.java.support.DefaultValueFactory;
import org.apache.etch.bindings.java.support.Validator_int;
import org.apache.etch.bindings.java.support.Validator_long;
import org.apache.etch.util.FlexBuffer;
import org.apache.etch.util.Resources;
import org.apache.etch.util.core.Who;
import org.apache.etch.util.core.io.SessionPacket;
import org.apache.etch.util.core.io.Transport;
import org.apache.etch.util.core.io.TransportPacket;
import org.junit.Test;


/** */
public class TestMessagizer
{
	private final static byte VERSION = 3; // BinaryTaggedData.VERSION;
	private final static byte NONE = -127; // TypeCode.NONE;
	
	/** */
	public TestMessagizer()
	{
		transport = new MyTransportPacket();
		session = new MySessionMessage();
		
		Resources res = new Resources();
		res.put( Transport.VALUE_FACTORY, vf );
		
		m = new Messagizer( transport, "foo:?Messagizer.format=binary", res );
		m.setSession( session );
	}

	private final ValueFactory vf = new MyValueFactory( "tcp:" );

	private final MyTransportPacket transport;

	private final MySessionMessage session;

	private final Messagizer m;
	
	/** @throws Exception */
	@Test
	public void construct1() throws Exception
	{
		assertSame( transport, m.getTransport() );
		assertSame( session, m.getSession() );
		assertSame( m, transport.getSession() );
	}

	////////////////////////////////
	// Test PacketHandler methods //
	////////////////////////////////

//	/** @throws Exception */
//	@Test
//	public void up() throws Exception
//	{
//		PacketHandler ph = m;
//
//		assertNull( mh.what );
//		assertNull( mh.src );
//
//		ph.up( ps );
//
//		assertSame( What.TESTMESSAGEHANDLERUP, mh.what );
//		assertSame( m, mh.src );
//		assertSame( ps, m.getSource() );
//	}
	
	private final Who who = new Who() { /* nothing */ };

	/** @throws Exception */
	@Test
	public void packet1() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[]
		{ VERSION, 1, 0, NONE } );
		
		session.msg_handled = true;
		
		m.sessionPacket( who, buf );
		
		assertSame( What.SESSION_MESSAGE, session.what );
		assertSame( who, session.sender );
		assertSame( MyValueFactory.mt_add, session.msg.type() );
		assertEquals( 0, session.msg.size() );
		assertNull( session.event );
	}

	/** @throws Exception */
	@Test
	public void packet2() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[]
		{ VERSION, 1, 0, NONE } );
		
		session.msg_handled = false;
		
		m.sessionPacket( who, buf );
		
		assertSame( What.SESSION_NOTIFY, session.what );
		assertSame( who, session.sender );
		assertSame( MyValueFactory.mt_add, session.msg.type() );
		assertEquals( 0, session.msg.size() );
		assertNotNull( session.event );
		
		UnwantedMessage um = (UnwantedMessage) session.event;
		assertSame( session.sender, um.sender );
		assertSame( session.msg, um.msg );
	}

	/** @throws Exception */
	@Test
	public void packet3() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[]
		{ VERSION, 2, 0, NONE } );
		
		session.msg_handled = true;
		
		m.sessionPacket( who, buf );
		
		assertSame( What.SESSION_MESSAGE, session.what );
		assertSame( who, session.sender );
		assertNotNull( session.msg );
		assertSame( MyValueFactory.mt_add_result, session.msg.type() );
		assertEquals( 0, session.msg.size() );
		assertNull( session.event );
	}

	/** @throws Exception */
	@Test
	public void packet4() throws Exception
	{
		FlexBuffer buf = new FlexBuffer( new byte[]
		{ VERSION, 2, 0, NONE } );
		
		session.msg_handled = false;
		
		m.sessionPacket( who, buf );
		
		assertSame( What.SESSION_NOTIFY, session.what );
		assertSame( who, session.sender );
		assertSame( MyValueFactory.mt_add_result, session.msg.type() );
		assertEquals( 0, session.msg.size() );
		assertNotNull( session.event );
		
		UnwantedMessage um = (UnwantedMessage) session.event;
		assertSame( session.sender, um.sender );
		assertSame( session.msg, um.msg );
	}

	/** @throws Exception */
	@Test
	public void message1() throws Exception
	{
		Message msg = new Message( MyValueFactory.mt_add, vf );
		transport.header_size = 0;
		
		assertNull( transport.what );
		
		m.transportMessage( who, msg );

		assertSame( What.TRANSPORT_PACKET, transport.what );
		assertSame( who, transport.recipient );
		assertNotNull( transport.buf );
		assertEquals( 4, transport.buf.length );
		assertEquals( VERSION, transport.buf[0] ); // version
		assertEquals( (byte) 1, transport.buf[1] ); // message type
		assertEquals( (byte) 0, transport.buf[2] ); // length
		assertEquals( NONE, transport.buf[3] ); // none
	}

	/** @throws Exception */
	@Test
	public void message2() throws Exception
	{
		Message msg = new Message( MyValueFactory.mt_add_result, vf );
		transport.header_size = 8;
		
		assertNull( transport.what );
		
		m.transportMessage( who, msg );

		assertSame( What.TRANSPORT_PACKET, transport.what );
		assertSame( who, transport.recipient );
		assertNotNull( transport.buf );
		assertEquals( 12, transport.buf.length );
		assertEquals( (byte) 0, transport.buf[0] ); // packet header...
		assertEquals( (byte) 0, transport.buf[1] );
		assertEquals( (byte) 0, transport.buf[2] );
		assertEquals( (byte) 0, transport.buf[3] );
		assertEquals( (byte) 0, transport.buf[4] );
		assertEquals( (byte) 0, transport.buf[5] );
		assertEquals( (byte) 0, transport.buf[6] );
		assertEquals( (byte) 0, transport.buf[7] );
		assertEquals( VERSION, transport.buf[8] ); // version
		assertEquals( (byte) 2, transport.buf[9] ); // message type
		assertEquals( (byte) 0, transport.buf[10] ); // length
		assertEquals( NONE, transport.buf[11] ); // none
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
		
		Object result = m.sessionQuery( QUERY );

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
		
		m.sessionControl( CONTROL, VALUE );

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
		
		m.sessionNotify( EVENT );

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
		
		Object result = m.transportQuery( QUERY );

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
		
		m.transportControl( CONTROL, VALUE );

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
		
		m.transportNotify( EVENT );

		assertSame( What.TRANSPORT_NOTIFY, transport.what );
		assertSame( EVENT, transport.event );
	}

	/**
	 * Reports activities which occur.
	 */
	public enum What
	{
		/** */ TRANSPORT_PACKET,
		/** */ TRANSPORT_QUERY,
		/** */ TRANSPORT_CONTROL,
		/** */ TRANSPORT_NOTIFY,
		/** */ SESSION_MESSAGE,
		/** */ SESSION_QUERY,
		/** */ SESSION_CONTROL,
		/** */ SESSION_NOTIFY
	};

	/** */
	public static class MyTransportPacket implements TransportPacket
	{
		/** */ public int header_size;
		/** */ public What what;
		/** */ public Who recipient;
		/** */ public byte[] buf;
		/** */ public Object query;
		/** */ public Object query_result;
		/** */ public Object control;
		/** */ public Object value;
		/** */ public Object event;
		
		public int headerSize()
        {
	        return header_size;
        }

		public void transportPacket( Who recipient, FlexBuffer buf ) throws Exception
        {
	        what = What.TRANSPORT_PACKET;
	        this.recipient = recipient;
	        this.buf = buf.getAvailBytes();
        }

		public Object transportQuery( Object query ) throws Exception
		{
			what = What.TRANSPORT_QUERY;
			this.query = query;
			return query_result;
		}

		public void transportControl( Object control, Object value ) throws Exception
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

		public SessionPacket getSession()
		{
			return session;
		}

		public void setSession( SessionPacket session )
		{
			this.session = session;
		}
		
		private SessionPacket session;
	}

	/** */
	public static class MySessionMessage implements SessionMessage
	{
		/** */
		public What what;
		/** */
		public Who sender;
		/** */
		public Message msg;
		/** */
		public boolean msg_handled;
		/** */
		public Object query;
		/** */
		public Object query_result;
		/** */
		public Object control;
		/** */
		public Object value;
		/** */
		public Object event;

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
		public final static Type mt_add = types.add( new Type( 1, "add" ) );

		/** */
		public final static Type mt_add_result = types.add( new Type( 2, "add_result" ) );

		/** */
		public final static Field mf_x = new Field( 3, "x" );

		/** */
		public final static Field mf_y = new Field( 4, "y" );

		/** */
		public final static Field mf_result = new Field( 5, "xresult" );
		
		static
		{
			mt_add.putValidator( mf_x, Validator_int.get( 0 ) );
			mt_add.putValidator( mf_y, Validator_int.get( 0 ) );
			mt_add.putValidator( _mf__messageId, Validator_long.get( 0 ) );

			mt_add_result.putValidator( mf_result, Validator_int.get( 0 ) );
			mt_add_result.putValidator( _mf__messageId, Validator_long.get( 0 ) );
			mt_add_result.putValidator( _mf__inReplyTo, Validator_long.get( 0 ) );
		}
	}
}

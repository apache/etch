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

package org.apache.etch.bindings.java.transport.filters;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.TypeMap;
import org.apache.etch.bindings.java.support.Class2TypeMap;
import org.apache.etch.bindings.java.support.DefaultValueFactory;
import org.apache.etch.bindings.java.transport.SessionMessage;
import org.apache.etch.bindings.java.transport.TransportMessage;
import org.apache.etch.bindings.java.transport.filters.PwAuth.UserPassword;
import org.apache.etch.util.Log;
import org.apache.etch.util.Resources;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.Who;
import org.apache.etch.util.core.io.Session;
import org.apache.etch.util.core.io.Transport;
import org.junit.Test;


/** */
public class TestPwAuth
{
	private final MyTransport transport = new MyTransport();
	private final MySession session = new MySession();
	private final MyValueFactory vf = new MyValueFactory( "tcp:" );
	private final Resources resources = new Resources();
	{
		resources.put( Transport.VALUE_FACTORY, vf );
		Log.addSink( null );
	}
	
	private PwAuth newPwAuth( String uri ) throws Exception
	{
		return new PwAuth( transport, new URL( uri ), resources );
	}
	
	/** @throws Exception */
	@Test
	public void constructor1() throws Exception
	{
		Log.report( "--constructor1" );
		
		transport.is_server = true;
		
		final PwAuth pwAuth = newPwAuth( "tcp://fred:1234@host:9999" );
		pwAuth.setSession( session );
		
		assertSame( pwAuth, transport.getSession() );
		assertSame( session, pwAuth.getSession() );
		
		assertEquals( true, pwAuth.getServer() );
		assertNull( pwAuth.transportQuery( PwAuth.USER ) );
		assertNull( pwAuth.transportQuery( PwAuth.PASSWORD ) );
	}
	
	/** @throws Exception */
	@Test
	public void constructor2() throws Exception
	{
		Log.report( "--constructor2" );
		
		transport.is_server = false;
		
		final PwAuth pwAuth = newPwAuth( "tcp://fred:1234@host:9999" );
		pwAuth.setSession( session );
		
		assertSame( pwAuth, transport.getSession() );
		assertSame( session, pwAuth.getSession() );
		
		assertEquals( false, pwAuth.getServer() );
		assertEquals( "fred", pwAuth.transportQuery( PwAuth.USER ) );
		assertEquals( "1234", pwAuth.transportQuery( PwAuth.PASSWORD ) );
	}
	
	/** @throws Exception */
	@Test
	public void constructor3() throws Exception
	{
		Log.report( "--constructor3" );
		
		transport.is_server = false;
		
		final PwAuth pwAuth = newPwAuth( "tcp://mary:2345@host:9999" );
		pwAuth.setSession( session );
		
		assertSame( pwAuth, transport.getSession() );
		assertSame( session, pwAuth.getSession() );
		
		assertEquals( false, pwAuth.getServer() );
		assertEquals( "mary", pwAuth.transportQuery( PwAuth.USER ) );
		assertEquals( "2345", pwAuth.transportQuery( PwAuth.PASSWORD ) );
	}
	
	/** @throws Exception */
	@Test
	public void constructor4() throws Exception
	{
		Log.report( "--constructor4" );
		
		transport.is_server = false;
		
		final PwAuth pwAuth = newPwAuth( "tcp://alice@host:9999" );
		pwAuth.setSession( session );
		
		assertSame( pwAuth, transport.getSession() );
		assertSame( session, pwAuth.getSession() );
		
		assertEquals( false, pwAuth.getServer() );
		assertEquals( "alice", pwAuth.transportQuery( PwAuth.USER ) );
		assertNull( pwAuth.transportQuery( PwAuth.PASSWORD ) );
	}
	
	/** @throws Exception */
	@Test
	public void constructor5() throws Exception
	{
		Log.report( "--constructor5" );
		
		transport.is_server = false; // IS_SERVER
		
		final PwAuth pwAuth = newPwAuth( "tcp://host:9999" );
		pwAuth.setSession( session );
		
		assertSame( pwAuth, transport.getSession() );
		assertSame( session, pwAuth.getSession() );
		
		assertEquals( false, pwAuth.getServer() );
		assertNull( pwAuth.transportQuery( PwAuth.USER ) );
		assertNull( pwAuth.transportQuery( PwAuth.PASSWORD ) );
	}
	
	/** @throws Exception */
	@Test( expected = Exception.class )
	public void constructor6() throws Exception
	{
		Log.report( "--constructor6" );
		
		transport.is_server = new Exception( "is_server failed" );
		
		newPwAuth( "tcp://fred:1234@host:9999" );
	}
	
	/** @throws Exception */
	@Test
	public void clientup1() throws Exception
	{
		Log.report( "--clientup1" );
		
		transport.is_server = false; // IS_SERVER
		
		final PwAuth pwAuth = newPwAuth( "tcp://fred:1234@host:9999" );
		pwAuth.setSession( session );
		
		pwAuth.sessionNotify( Session.UP );
		
		assertTrue( session.up );
		
		assertSame( What.TRANSPORT_MESSAGE, transport.what );
		Message msg = transport.msg;
		assertEquals( "_Etch_PwAuthReq", msg.type().getName() );
		assertEquals( "fred", msg.get( new Field( "user" ) ) );
		assertEquals( "1234", msg.get( new Field( "password" ) ) );
	}
	
	/** @throws Exception */
	@Test
	public void clientup2() throws Exception
	{
		Log.report( "--clientup2" );
		
		transport.is_server = false;
		session.password = "2345";
		
		final PwAuth pwAuth = newPwAuth( "tcp://fred@host:9999" );
		pwAuth.setSession( session );
		
		pwAuth.sessionNotify( Session.UP );
		
		assertTrue( session.up );
		
		assertSame( What.TRANSPORT_MESSAGE, transport.what );
		Message msg = transport.msg;
		assertEquals( "_Etch_PwAuthReq", msg.type().getName() );
		assertEquals( "fred", msg.get( new Field( "user" ) ) );
		assertEquals( "2345", msg.get( new Field( "password" ) ) );
	}
	
	/** @throws Exception */
	@Test
	public void clientup3() throws Exception
	{
		Log.report( "--clientup3" );
		
		transport.is_server = false;
		session.user = "mary";
		session.password = "3456";
		
		final PwAuth pwAuth = newPwAuth( "tcp://host:9999" );
		pwAuth.setSession( session );
		
		pwAuth.sessionNotify( Session.UP );
		
		assertTrue( session.up );
		
		assertSame( What.TRANSPORT_MESSAGE, transport.what );
		Message msg = transport.msg;
		assertEquals( "_Etch_PwAuthReq", msg.type().getName() );
		assertEquals( "mary", msg.get( new Field( "user" ) ) );
		assertEquals( "3456", msg.get( new Field( "password" ) ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void clientup4() throws Exception
	{
		Log.report( "--clientup4" );
		
		transport.is_server = false;
		// session.user = null;
		session.password = "1234";
		
		final PwAuth pwAuth = newPwAuth( "tcp://host:9999" );
		pwAuth.setSession( session );
		
		pwAuth.sessionNotify( Session.UP );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void clientup5() throws Exception
	{
		Log.report( "--clientup5" );
		
		transport.is_server = false;
		session.user = "fred";
//		session.password = null;
		
		final PwAuth pwAuth = newPwAuth( "tcp://host:9999" );
		pwAuth.setSession( session );
		
		pwAuth.sessionNotify( Session.UP );
	}
	
	/** @throws Exception */
	@Test( expected = Exception.class )
	public void clientup6() throws Exception
	{
		Log.report( "--clientup6" );
		
		transport.is_server = false;
		session.user = "fred";
		session.password = new Exception( "password failed" );
		
		final PwAuth pwAuth = newPwAuth( "tcp://host:9999" );
		pwAuth.setSession( session );
		
		pwAuth.sessionNotify( Session.UP );
	}
	
	/** @throws Exception */
	@Test( expected = Exception.class )
	public void clientup7() throws Exception
	{
		Log.report( "--clientup7" );
		
		transport.is_server = false;
		session.user = new Exception( "user failed" );
		session.password = "1234";
		
		final PwAuth pwAuth = newPwAuth( "tcp://host:9999" );
		pwAuth.setSession( session );
		
		pwAuth.sessionNotify( Session.UP );
	}
	
	/** @throws Exception */
	@Test
	public void clientdown1() throws Exception
	{
		Log.report( "--clientdown1" );
		
		transport.is_server = false;
		
		final PwAuth pwAuth = newPwAuth( "tcp://host:9999" );
		pwAuth.setSession( session );
		
		pwAuth.sessionNotify( Session.DOWN );
		
		assertFalse( session.up );
	}
	
	/** @throws Exception */
	@Test
	public void serverup1() throws Exception
	{
		Log.report( "--serverup1" );
		
		transport.is_server = true;
		
		final PwAuth pwAuth = newPwAuth( "tcp://host:9999" );
		pwAuth.setSession( session );
		
		pwAuth.sessionNotify( Session.UP );
		
		assertTrue( session.up );
	}
	
	/** @throws Exception */
	@Test
	public void serverdown1() throws Exception
	{
		Log.report( "--serverdown1" );
		
		transport.is_server = true;
		
		final PwAuth pwAuth = newPwAuth( "tcp://host:9999" );
		pwAuth.setSession( session );
		
		pwAuth.sessionNotify( Session.DOWN );
		
		assertFalse( session.up );
		assertNull( session.userPassword.user );
		assertNull( session.userPassword.password );
	}
	
	// sessionMessage, server, req
	// sessionMessage, server, resp
	// sessionMessage, client, req
	// sessionMessage, client, resp
	
	/**
	 * @param o
	 * @return o
	 * @throws Exception
	 */
	static Object cook( Object o ) throws Exception
	{
		if (o instanceof Exception)
		{
			Exception e = (Exception) o;
			e.fillInStackTrace();
			throw e;
		}
		return o;
	}
	
	/** */
	enum What
	{
		/** */ TRANSPORT_MESSAGE,
		/** */ SESSION_MESSAGE
	}
	
	@SuppressWarnings("hiding")
	private static class MyTransport implements TransportMessage
	{
		/** */
		public MyTransport()
		{
			// nothing to do.
		}

		/** */ public What what;
		/** */ public Who recipient;
		/** */ public Message msg;
		/** */ public Object is_server;
		
		public void transportMessage( Who recipient, Message msg )
			throws Exception
		{
			Log.report( "transportMessage", "recipient", recipient, "msg", msg );
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
			Log.report( "transportQuery", "query", query );
			
			if (query == Transport.IS_SERVER)
				return cook( is_server );
			
			throw new UnsupportedOperationException( "unknown query "+query );
		}

		public void transportControl( Object control, Object value )
			throws Exception
		{
			Log.report( "transportControl", "control", control, "value", value );
			throw new UnsupportedOperationException( "unknown control "+control );
		}

		public void transportNotify( Object event ) throws Exception
		{
			throw new UnsupportedOperationException( "unknown notify "+event );
		}
	}
	
	@SuppressWarnings("hiding")
	private static class MySession implements SessionMessage
	{
		/** */
		public MySession()
		{
			// nothing to do.
		}

		/** */ public What what;
		/** */ public Who sender;
		/** */ public Message msg;
		/** */ public boolean handled;
		/** */ public Object user;
		/** */ public Object password;
		/** */ public Boolean up;
		/** */ public UserPassword userPassword;
		
		public boolean sessionMessage( Who sender, Message msg )
			throws Exception
		{
			Log.report( "sessionMessage", "sender", sender, "msg", msg );
			what = What.SESSION_MESSAGE;
			this.sender = sender;
			this.msg = msg;
			return handled;
		}

		public Object sessionQuery( Object query ) throws Exception
		{
			Log.report( "sessionQuery", "query", query );
			
			if (query == PwAuth.USER)
				return cook( user );
			
			if (query == PwAuth.PASSWORD)
				return cook( password );
			
			throw new UnsupportedOperationException( "unknown query "+query );
		}

		public void sessionControl( Object control, Object value )
			throws Exception
		{
			Log.report( "sessionControl", "control", control, "value", value );
			throw new UnsupportedOperationException( "unknown control "+control );
		}

		public void sessionNotify( Object event ) throws Exception
		{
			if (event == Session.UP)
			{
				up = true;
				return;
			}
			
			if (event == Session.DOWN)
			{
				up = false;
				return;
			}
			
			if (event instanceof UserPassword)
			{
				userPassword = (UserPassword) event;
				return;
			}
			
			throw new UnsupportedOperationException( "unknown notify "+event );
		}
	}
	
	private static class MyValueFactory extends DefaultValueFactory
	{
		/**
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
	}
}

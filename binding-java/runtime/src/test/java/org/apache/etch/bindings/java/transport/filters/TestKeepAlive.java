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
import static org.junit.Assert.assertSame;

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.TypeMap;
import org.apache.etch.bindings.java.support.Class2TypeMap;
import org.apache.etch.bindings.java.support.DefaultValueFactory;
import org.apache.etch.bindings.java.support.Validator_int;
import org.apache.etch.bindings.java.transport.SessionMessage;
import org.apache.etch.bindings.java.transport.TransportMessage;
import org.apache.etch.util.Log;
import org.apache.etch.util.Resources;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.Who;
import org.apache.etch.util.core.io.Session;
import org.apache.etch.util.core.io.Transport;
import org.junit.Test;


/** */
public class TestKeepAlive
{
	private final MyTransport transport = new MyTransport();
	private final MySession session = new MySession();
	private final MyValueFactory vf = new MyValueFactory( "tcp:" );
	private final Resources resources = new Resources();
	{
		resources.put( Transport.VALUE_FACTORY, vf );
		Log.addSink( null );
	}
	
	private KeepAlive newKeepAlive(String uri) throws Exception
	{
		return new KeepAlive(transport,new URL(uri),resources);
	}
	
	/** @throws Exception */
	@Test
	public void constructor1() throws Exception
	{
		transport.is_server = true;
		String uri = "tcp://localhost:4801?filter=KeepAlive";
		KeepAlive filter = newKeepAlive(uri);
		filter.setSession( session );
		
		assertSame(transport.getSession(),filter);
		assertSame(filter.getSession(),session);
		assertSame(filter.getServer(),true);
		assertEquals(filter.getCount(),4);
		assertEquals(filter.getDelay(),15);
	}
	
	/** @throws Exception */
	@Test
	public void constructor2() throws Exception
	{
		transport.is_server = true;
		String uri = "tcp://localhost:4801?filter=KeepAlive&KeepAlive.delay=20&KeepAlive.count=5";
		KeepAlive filter = newKeepAlive(uri);
		filter.setSession( session );
		
		assertSame(transport.getSession(),filter);
		assertSame(filter.getSession(),session);
		assertSame(filter.getServer(),true);
		assertEquals(filter.getCount(),5);
		assertEquals(filter.getDelay(),20);
	}
	
	/** @throws Exception */
	@Test
	public void constructor3() throws Exception
	{
		transport.is_server = false;
		String uri = "tcp://localhost:4801?filter=KeepAlive";
		KeepAlive filter = newKeepAlive(uri);
		filter.setSession( session );
		
		assertSame(transport.getSession(),filter);
		assertSame(filter.getSession(),session);
		assertSame(filter.getServer(),false);
		assertEquals(filter.getCount(),4);
		assertEquals(filter.getDelay(),15);
	}
	
	/** @throws Exception */
	@Test
	public void clientup() throws Exception
	{
		transport.is_server = false;
		String uri = "tcp://localhost:4801?filter=KeepAlive&KeepAlive.delay=3";
		KeepAlive filter = newKeepAlive(uri);
		filter.setSession( session );		
		filter.sessionNotify( Session.UP );
//		System.out.println("Sleeping for 5 seconds");
		Thread.sleep( 5000 );		 
//		System.out.println("Out of sleep");
		assertSame( What.TRANSPORT_MESSAGE, transport.what );
		Message msg = transport.msg;
		assertEquals( "_Etch_KeepAliveReq", msg.type().getName() );
		assertEquals( 4, msg.get( new Field( "count" ) ) );
		assertEquals( 3, msg.get( new Field( "delay" ) ) );
	}
	
	/** @throws Exception */
	@Test
	public void clientup1() throws Exception
	{
		transport.is_server = false;
		String uri = "tcp://localhost:4801?filter=KeepAlive&KeepAlive.delay=2";
		KeepAlive filter = newKeepAlive(uri);
		filter.setSession( session );		
		filter.sessionNotify( Session.UP );
//		System.out.println("Sleeping for 4 seconds");
		Thread.sleep( 4000 );		 
//		System.out.println("Out of sleep");
		assertSame( What.TRANSPORT_MESSAGE, transport.what );
		Message msg = transport.msg;
		assertEquals( "_Etch_KeepAliveReq", msg.type().getName() );
		assertEquals( 4, msg.get( new Field( "count" ) ) );
		assertEquals( 2, msg.get( new Field( "delay" ) ) );
	}
	
	/** @throws Exception */
	@Test
	public void clientdown() throws Exception
	{
		Log.report( "--clientdown" );
		
		transport.is_server = false;
		
		String uri = "tcp://localhost:4801?filter=KeepAlive"; 
		
		KeepAlive filter = newKeepAlive(uri);
		filter.setSession( session );		
		filter.sessionNotify( Session.DOWN );
		
		assertFalse( session.up );
	}
	
	/** @throws Exception */
	@Test
	public void serverUp() throws Exception
	{
		transport.is_server = true;
		String uri = "tcp://localhost:4801?filter=KeepAlive"; 		
		KeepAlive filter = newKeepAlive(uri);
		filter.setSession( session );		
		filter.sessionNotify( Session.UP );	
		
		Message req = constructMessage();
		
		filter.sessionMessage( null, req );
		
		assertEquals(req.reply(),transport.msg);
		
	}
	
	/** @throws Exception */
	@Test
	public void serverdown() throws Exception
	{
		Log.report( "--clientdown" );
		
		transport.is_server = true;
		
		String uri = "tcp://localhost:4801?filter=KeepAlive"; 
		
		KeepAlive filter = newKeepAlive(uri);
		filter.setSession( session );		
		filter.sessionNotify( Session.DOWN );
		
		assertFalse( session.up );
	}
	
/*	@Test
	public void resetClient() throws Exception
	{
		transport.is_server = false;
		String uri = "tcp://localhost:4801?filter=KeepAlive&KeepAlive.delay=2&KeepAlive.count=4"; 	
		KeepAlive filter = newKeepAlive(uri);
		filter.setSession( session );		
		filter.sessionNotify( Session.UP );	
		
		System.out.println("Sleeping for 10 seconds");
		Thread.sleep( 20000 );		 
		System.out.println("Out of sleep");
		assertTrue(transport.isReset);		
	} */
	
	
	private Message constructMessage() throws Exception
	{
		int delay = 15;
		int count = 4;
		
		MyValueFactory vf1 = new MyValueFactory( "tcp:" );
		
		Field mf_delay = new Field( "delay" );
		Field mf_count = new Field( "count" );
		
		Type mt_request = new Type( "_Etch_KeepAliveReq" );
		mt_request.putValidator( mf_delay, Validator_int.get( 0 ) );
		mt_request.putValidator( mf_count, Validator_int.get( 0 ) );
		vf1.addType( mt_request );

		Type mt_response = new Type( "_Etch_KeepAliveResp" );
		vf1.addType( mt_response );
		
		mt_request.setResult( mt_response );
		
		Message msg = new Message( mt_request, vf1 );
		msg.put( mf_delay, delay );
		msg.put( mf_count, count );
		
		return msg;
	}

	
	
	
	
	
	/** */
	enum What
	{
		/** */ TRANSPORT_MESSAGE,
		/** */ SESSION_MESSAGE
	}
	
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
		/** */ public boolean isReset = false;
		
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
			
	//		return query;
			throw new UnsupportedOperationException( "unknown query "+query );
		}

		public void transportControl( Object control, Object value )
			throws Exception
		{
			Log.report( "transportControl1", "control", control, "value", value );
			
			if (control == Transport.RESET) {
				Log.report("Control Variable is Reset, changing the reset flag",control);
				isReset = true;
			}
			
	//		throw new UnsupportedOperationException( "unknown control "+control );
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
			return query;
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

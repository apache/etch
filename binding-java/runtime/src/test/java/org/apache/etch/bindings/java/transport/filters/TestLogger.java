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
import static org.junit.Assert.assertNotSame;
import static org.junit.Assert.assertSame;

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.TypeMap;
import org.apache.etch.bindings.java.support.Class2TypeMap;
import org.apache.etch.bindings.java.support.DefaultValueFactory;
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
public class TestLogger
{
	
	private final MyTransport transport = new MyTransport();
	private final MySession session = new MySession();
	private final MyValueFactory vf = new MyValueFactory("tcp:" );
	private final Resources resources = new Resources();
	{
		resources.put( Transport.VALUE_FACTORY, vf );
		Log.addSink( null );
	}

	
	private Logger newLogger(String uri) throws Exception
	{
		return new Logger(transport,new URL(uri),null);
	}
	
	/** @throws Exception */
	@Test
	public void constructor1() throws Exception {
		String uri = "tcp://localhost:4802?filter=Logger";
		Logger.testFlag = true;
		Logger logger = newLogger(uri);
		assertEquals(logger.getFileName(),"Log.txt");
		assertNotSame( logger.getFileName(),logger.getTestRenamedFileName());
		
	}
	
	/** @throws Exception */
	@Test
	public void constructor2() throws Exception {
		String uri = "tcp://localhost:4802?filter=Logger&Logger.filePath=Test.txt";
		Logger.testFlag = true;
		Logger logger = newLogger(uri);
		assertEquals(logger.getFileName(),"Test.txt");
		assertNotSame( logger.getFileName(),logger.getTestRenamedFileName());
	}
	
	/** @throws Exception */
	@Test
	public void logger() throws Exception {
		String uri = "tcp://localhost:4802?filter=Logger";
		Logger.testFlag = true;
		Logger logger = newLogger(uri);
		logger.setSession( session );
		assertSame(transport.getSession(),logger);
		assertSame(logger.getSession(),session);
	}
	
	/** @throws Exception */
	@Test
	public void logger1() throws Exception {
		String uri = "tcp://localhost:4802?filter=Logger";
		Logger.testFlag = true;
		Logger logger = newLogger(uri);
		logger.setSession( session );
		assertSame(transport.getSession(),logger);
		assertSame(logger.getSession(),session);
		Message m = constructMessage();
		logger.transportMessage( null, m );
		assertEquals(m.toString(),transport.msg.toString());
	}
	
	/** @throws Exception */
	@Test
	public void logger2() throws Exception {
		String uri = "tcp://localhost:4802?filter=Logger";
		Logger.testFlag = true;
		Logger logger = newLogger(uri);
		logger.setSession( session );
		assertSame(transport.getSession(),logger);
		assertSame(logger.getSession(),session);
		Message m = constructMessage();
		logger.sessionMessage( null, m );
		assertEquals(m.toString(),session.msg.toString());
	}
	
	
	
	private Message constructMessage() throws Exception
	{
		
		Type mt_request = new Type( "_Etch_Logger_Test" );
		
		
		Type mt_response = new Type( "_Etch_Logger_Test_Resp" );
			
		mt_request.setResult( mt_response );
		
		vf.addType( mt_request);
		
		Message msg = new Message( mt_request, vf );
	
		return msg;
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
			
			throw new UnsupportedOperationException( "unknown query "+query );
		}

		public void transportControl( Object control, Object value )
			throws Exception
		{
			
			
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
		/** */ public Boolean up;
		
		
		public boolean sessionMessage( Who sender, Message msg )
			throws Exception
		{
			Log.report( "sessionMessage", "sender", sender, "msg", msg );
			what = What.SESSION_MESSAGE;
			this.sender = sender;
			this.msg = msg;
			return true;
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

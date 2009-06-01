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

package org.apache.etch.services.ns.support;


import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.bindings.java.support.TransportFactory;
import org.apache.etch.bindings.java.transport.SessionMessage;
import org.apache.etch.bindings.java.transport.TransportMessage;
import org.apache.etch.services.ns.NameService.Entry;
import org.apache.etch.services.ns.support.DefaultNSLib;
import org.apache.etch.util.AlarmListener;
import org.apache.etch.util.AlarmManager;
import org.apache.etch.util.Resources;
import org.apache.etch.util.StringUtil;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.Who;
import org.apache.etch.util.core.io.Session;
import org.apache.etch.util.core.io.Transport;


/**
 * Transport factory abstraction for the Naming Service enhancement
 */

public class EtchTransportFactory extends TransportFactory
{
	public final static String LISTENER_URI = "suri";
	public final static String LISTENER_REGISTERED_URI = "curi";
	public final static String NS_URI = "nsuri";
	public final static String RECONNECT_TERM = "reconnectDelay"; 
	
	static  
	{
		TransportFactory.define( "etch", new EtchTransportFactory() );
		NSLib.setInstance( new DefaultNSLib() );
	}
	
	@Override
	protected Transport<ServerFactory> newListener( String uri,
			Resources resources ) throws Exception
	{
		String listenerUri;
		URL u = new URL( uri );
		
		// Extract uri for the listener to listen on 
		if ( !u.hasTerm( LISTENER_URI ) || StringUtil.isEmpty( listenerUri = u.getTerm( LISTENER_URI ) ) )
			throw new IllegalArgumentException( "listener uri not configured properly within the etch uri" );
		
		if ( !u.hasTerm( LISTENER_REGISTERED_URI ) || StringUtil.isEmpty( u.getTerm( LISTENER_REGISTERED_URI) ) )
			throw new IllegalArgumentException( "listener registered uri " +
			"not configured properly within the etch uri" );
		
		// call to create transport stack underneath
		Transport<ServerFactory> transport = TransportFactory.getListener( listenerUri, resources );
		EtchSessionListener esl = new EtchSessionListener( transport, uri, resources );
		
		return esl;
	}

	@Override
	protected TransportMessage newTransport( String uri, Resources resources )
			throws Exception
	{
		// create an etch blob and return
		return new EtchTransport( uri, resources );
	}
	
	// Etch blob that intercepts client messages 
	private class EtchTransport implements TransportMessage, SessionMessage, AlarmListener
	{
		public TransportMessage transport;
		public SessionMessage session;
		public Resources resources;
		public String uri;
		public int reconnect = 0;
		
		/**
		 * 
		 * @param uri uri based on the etch scheme
		 * @param resources 
		 */
		public EtchTransport( String uri, Resources resources )
		{
			this.resources = resources;
			this.uri = uri;
			
			URL u = new URL( uri );

			if ( StringUtil.isEmpty( u.getUri() ) )
				throw new IllegalArgumentException( "source uri == null" );

			if ( u.hasTerm( RECONNECT_TERM ) )
				reconnect = u.getIntegerTerm( RECONNECT_TERM, 0 );
		}
		
		public void transportMessage( Who recipient, Message msg )
				throws Exception
		{
			transport.transportMessage( recipient, msg );
		}

		public SessionMessage getSession()
		{
			return session;
		}

		public void setSession( SessionMessage session )
		{
			this.session = session;
		}

		public void transportControl( Object control, Object value )
				throws Exception
		{
			// Create a new transport stack
			if ( control == START_AND_WAIT_UP || control == START )
			{	
				Entry targetEntry = DefaultNSLib.staticLookup( uri ); 
				
				if ( targetEntry == null )
					throw new Exception( "No such registered listener: " + uri );
				
				String targetUri = targetEntry.getTargetUri();
				
				TransportMessage m = TransportFactory.getTransport( targetUri, resources );
				
				m.setSession( this );
				
				try
				{
					m.transportControl( control, value );
					
					// only if above works
					transport = m;

				}
				catch( Exception e )
				{
					// in case the transport doesn't start up 
					// or throws a timeout exception
					m.transportControl( STOP, null );
					m.setSession( null );
					throw e;
				}
				return;
			}
			
			// destroy the transport stack when connection goes down
			if ( control == STOP || control == STOP_AND_WAIT_DOWN )
			{
				TransportMessage m = transport;
				if ( m != null )
				{
					transport = null;
					m.transportControl( control, value );
				}
				return;
			}
			
			if ( transport != null )
				transport.transportControl( control, value );
			else
				throw new UnsupportedOperationException( "no transport " +
						"to implement the control: " + control );
		}

		public void transportNotify( Object event ) throws Exception
		{
			if ( transport != null )
				transport.transportNotify( event );
		}

		public Object transportQuery( Object query ) throws Exception
		{
			if ( transport != null )
				return transport.transportQuery( query );
			
			throw new UnsupportedOperationException( "no transport " +
						"to implement the query: " + query );

		}

		public boolean sessionMessage( Who sender, Message msg )
				throws Exception
		{
			return session.sessionMessage( sender, msg );
		}

		public void sessionControl( Object control, Object value )
				throws Exception
		{
			session.sessionControl( control, value );
		}

		public void sessionNotify( Object event ) throws Exception
		{
			/*
			 * Handle the listener going down
			 */
			session.sessionNotify( event );
			
			if ( event == DOWN )
			{
				if ( this.transport == null )
				{
					// this notify is as a result of the client shutdown 
					AlarmManager.shutdown();
					return;
				}
				
				if ( reconnect > 0 )
					AlarmManager.staticAdd( this, null, reconnect );
				
				// The transport has been stopped
				// Now, discard this transport stack
				transport = null;
			}
		}

		public Object sessionQuery( Object query ) throws Exception
		{
			return session.sessionQuery( query );
		}

		public int wakeup( AlarmManager manager, Object state, long due )
		{
			try
			{
				// throws exception if connection is DOWN
				transportControl( START_AND_WAIT_UP, 4000 );
				
				// remove alarm if transportControl successful
				return 0;
			}
			catch( Exception e )
			{
				try
				{
					this.sessionNotify( e );
				}
				catch( Exception e1 )
				{
					e1.printStackTrace();
				}
				return reconnect;
			}
		}
	}
	
	private class EtchSessionListener implements Transport<ServerFactory>
	{
		
		private Transport<ServerFactory> transport;
		private String uri;
		String nsUri;
		
		public EtchSessionListener( Transport<ServerFactory> transport, String uri, Resources resources )
		{
			this.transport = transport;
			transport.setSession( session );
			this.uri = uri;
		}
		private ServerFactory session = new ServerFactory()
		{
			private ServerFactory _session;
			
			public void newServer( String arg0, Resources arg1,
					TransportMessage arg2 ) throws Exception
			{
				_session.newServer( arg0, arg1, arg2 );
			}

			public ValueFactory newValueFactory()
			{
				return _session.newValueFactory();
			}

			public void sessionControl( Object arg0, Object arg1 )
					throws Exception
			{
				_session.sessionControl( arg0, arg1 );
			}

			public void sessionNotify( Object arg0 ) throws Exception
			{
				_session.sessionNotify( arg0 );
			}

			public Object sessionQuery( Object arg0 ) throws Exception
			{
				return _session.sessionQuery( arg0 );
			}

			public Session getSession()
			{
				return _session;
			}

			public void setSession( Session arg0 )
			{
				_session = (ServerFactory)arg0;
			}

			public void transportControl( Object arg0, Object arg1 )
					throws Exception
			{
				// TODO Auto-generated method stub
				
			}

			public void transportNotify( Object arg0 ) throws Exception
			{
				// TODO Auto-generated method stub
				
			}

			public Object transportQuery( Object arg0 ) throws Exception
			{
				// TODO Auto-generated method stub
				return null;
			}
		};
		
		public void transportControl( Object control, Object value )
				throws Exception
		{

			transport.transportControl( control, value );
			
			if ( control == START || control == START_AND_WAIT_UP )
			{	
				// register to the naming service here
				NSLib.staticRegister( getSession(), uri, null, 0 );
				
				return;
			}
			
			if ( control == STOP || control == STOP_AND_WAIT_DOWN )
			{
				// unregister from the name service
				NSLib.staticUnregister( getSession(), uri, false );
				
				return;
			}
		}

		public void transportNotify( Object event ) throws Exception
		{
			transport.transportNotify( event );
		}

		public Object transportQuery( Object query ) throws Exception
		{
			return transport.transportQuery( query );
		}

		public ServerFactory getSession()
		{
			return (ServerFactory) session.getSession();
		}
		
		public void setSession( ServerFactory session )
		{
			this.session.setSession( session );
		}
	}

	
}

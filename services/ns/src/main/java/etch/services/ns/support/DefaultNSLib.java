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

package etch.services.ns.support;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import org.apache.etch.util.AlarmListener;
import org.apache.etch.util.AlarmManager;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.io.Session;

import etch.services.ns.BaseNameServiceClient;
import etch.services.ns.NameServiceClient;
import etch.services.ns.NameServiceHelper;
import etch.services.ns.RemoteNameServiceServer;
import etch.services.ns.NameService.Entry;

public class DefaultNSLib extends NSLib
{
	private final static int INITIAL_ALARM_DELAY = 1;
	
	public NSLib getImplInstance()
	{
		return new DefaultNSLib();
	}
	
	/**
	 * Collection of <session, record> entries to keep track of all listeners connected 
	 * to this instance of NSLib
	 */
	public Map<Session, Record> recordsBySession = Collections.synchronizedMap( 
			new HashMap<Session, Record>() );
	
	/*
	 * Map of <nsUri, RemoteNameServiceServer> 
	 */
	private Map<String, RemoteNameServiceServer> serversByNsUri = 
		new HashMap<String, RemoteNameServiceServer>();
	
	/**
	 * Create a new RemoteNameServiceServer with the nsUri supplied
	 * 
	 * @param nsUri uri of the target name service
	 * @return a new RemoteNameServiceServer
	 */
	private static RemoteNameServiceServer createNewServer( String nsUri ) 
		throws Exception
	{	
		return NameServiceHelper.newServer( nsUri, null, 
					new NameServiceHelper.NameServiceClientFactory()
					{
						@Override
						public NameServiceClient newNameServiceClient(
								RemoteNameServiceServer server )
								throws Exception
						{
							return new BaseNameServiceClient(); 
						}
					}
		);
	}
	
	/**
	 * Starts this instance of the NSLib if it is not already started
	 */
	private static void start( RemoteNameServiceServer server ) throws Exception
	{
		try
		{
			server._startAndWaitUp( 4000 );
		}
		catch( Exception e )
		{
			e.printStackTrace();
			server._stop();
			throw e;
		}
	}
	
	/**
	 * Disconnect from the name service
	 */
	private static void stop( RemoteNameServiceServer server ) throws Exception
	{
		try
		{
			server._stopAndWaitDown( 4000 );
		}
		catch( Exception e )
		{
			e.printStackTrace();
			throw e;
		}
	}

	public RemoteNameServiceServer getServer( String nsUri ) throws Exception
	{
		synchronized( serversByNsUri )
		{
			if ( ! serversByNsUri.containsKey( nsUri ) )
			{
				RemoteNameServiceServer server = createNewServer( nsUri );
				serversByNsUri.put( nsUri, server );
				return server;
			}
			else
				return serversByNsUri.get( nsUri );
		}
	}

	/**
	 * Convenience object holding relevant listener data
	 */
	private class Record implements AlarmListener
	{
		String nsUri;
		String sourceUri;
		Map<?,?> qualities;
		String targetUri;
		int ttl;
		
		public Record( String nsUri, String sourceUri, Map<?,?> qualities, String targetUri, int ttl )
		{
			this.nsUri = ( nsUri == null || nsUri.isEmpty() )? defaultNsUri : nsUri;
			this.sourceUri = sourceUri;
			this.qualities = qualities;
			this.targetUri = targetUri;
			this.ttl = ttl;
		}

		@Override
		public int wakeup( AlarmManager manager, Object state, long due )
		{
			try
			{
				register( this, state );
			}
			catch( Exception e )
			{
				e.printStackTrace();
			}
			return RECONNECT_DELAY;
		}
	}
	
	public void register( Session session, String uri, Map<?,?> qualities, int ttl )
	{
		if ( session == null )
			throw new IllegalArgumentException( "session == null" );

		URL u = new URL( uri );

		if ( ! u.hasTerm( EtchTransportFactory.LISTENER_REGISTERED_URI ) )
			throw new IllegalArgumentException( "listener registered uri " +
			"not configured properly within the etch uri" );
		
		String sourceUri = u.getUri();
		String targetUri = u.getTerm( EtchTransportFactory.LISTENER_REGISTERED_URI );
		String nsUri = null;
		
		if ( sourceUri == null || sourceUri.isEmpty() )
			throw new IllegalArgumentException( "sourceUri == null" );
		
		// extract if present in uri
		if ( u.hasTerm( EtchTransportFactory.NS_URI ) )
			nsUri = u.getTerm( EtchTransportFactory.NS_URI );
		
		if ( nsUri == null || nsUri.isEmpty() )
			nsUri = defaultNsUri;
		
		Record record = new Record( nsUri, sourceUri, qualities, targetUri, ttl );
		//Assertion.check( recordsBySession.containsKey( session ), "session already exists" );
		recordsBySession.put( session, record );
		
		// add register task to the alarm manager
		AlarmManager.staticAdd( record, session, INITIAL_ALARM_DELAY );
	}
	
	/**
	 * Private method to actually register
	 * @param record
	 */
	
	private void register( Record record, Object state ) throws Exception
	{
		if ( record == null )
			throw new IllegalArgumentException( "record == null" );
		
		try
		{
			RemoteNameServiceServer server = getServer( record.nsUri );
			synchronized( server )
			{	
				start( server );	// start the server
				
				try
				{
					server.register( record.sourceUri, record.qualities,	// register 
							record.targetUri, record.ttl );
				}
				catch( RuntimeException e )
				{
					System.out.println("Couldn't register with name service. " +
							"Trying again after " + RECONNECT_DELAY + " seconds ..." );
					((Session)state).sessionNotify( e );
				}
				finally
				{
					stop( server );	// stop the server
				}
			}
		}
		catch( Exception e )
		{
			e.printStackTrace();
		}
	}
	
	public void unregister( Session session, String uri, boolean deleteEntry )
	{
		if ( session == null )
			throw new IllegalArgumentException( "session == null" );
		
		URL u = new URL( uri );
		
		String sourceUri = u.getUri();
		if ( sourceUri == null || sourceUri.isEmpty() )
			throw new IllegalArgumentException( "sourceUri == null" );
		
		String nsUri = null;
		if ( u.hasTerm( EtchTransportFactory.NS_URI ) )
			nsUri = u.getTerm( EtchTransportFactory.NS_URI );
		if ( nsUri == null || nsUri.isEmpty() )
			nsUri = getDefaultNsUri();
		
		// remove alarm for that session/record
		AlarmManager.staticRemove( recordsBySession.remove( session ) );
		
		RemoteNameServiceServer server = serversByNsUri.get( nsUri );
		try
		{
			synchronized ( server ) 
			{
				start( server );
				try
				{
					server.unregister( sourceUri, deleteEntry );
				}
				finally
				{
					stop( server );
				}
			}
		}
		catch( Exception e )
		{
			try
			{
				session.sessionNotify( e );
			}
			catch( Exception e1 )
			{
				e1.printStackTrace();
			}
		}
	}
	
	public Entry lookup( String uri )
	{
		String nsUri = null;
		URL u = new URL( uri );
		if ( u.hasPort() )
			nsUri = "tcp://" + u.getHost() + ":" + u.getPort();
		
		String sourceUri = u.getUri();
		
		if ( nsUri == null || nsUri.isEmpty() )
			nsUri = getDefaultNsUri();
		
		if ( sourceUri == null || sourceUri.isEmpty() )
			throw new IllegalArgumentException( "sourceUri == null" );
		
		Entry result = null;
		
		try
		{
			RemoteNameServiceServer server = getServer( nsUri );
			synchronized ( server )
			{
				start( server );
				
				try
				{
					result = server.lookup( sourceUri );
				}
				finally
				{
					stop( server );
				}
			}
		}
		catch( Exception e )
		{
			e.printStackTrace();
		}
		
		return result;
	}
	
	private String defaultNsUri = "tcp://127.0.0.1:8002"; 

	public String getDefaultNsUri()
	{
		return defaultNsUri;
	}

	public void setDefaultNsUri( String defaultNsUri )
	{
		this.defaultNsUri = defaultNsUri;
	}

	@Override
	public void shutdown()
	{
		recordsBySession.clear();
		serversByNsUri.clear();
	}
}

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

package org.apache.etch.services.config.example;

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.services.config.BaseConfigurationClient;
import org.apache.etch.services.config.ConfigurationClient;
import org.apache.etch.services.config.ConfigurationHelper;
import org.apache.etch.services.config.ConfigurationServer;
import org.apache.etch.services.config.RemoteConfigurationServer;
import org.apache.etch.services.config.YamlConfig;
import org.apache.etch.services.config.ConfigurationHelper.ConfigurationClientFactory;
import org.apache.etch.services.config.example.ConfigExampleHelper.ConfigExampleServerFactory;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.io.Session;
import org.apache.etch.util.core.io.Transport;

/**
 * Main program for ConfigExampleServer. This program makes a listener to accept
 * connections from MainConfigExampleClient.
 */
public class MainConfigExampleListener implements ConfigExampleServerFactory
{
	private static final String LOCAL = "services/config/example/local";
	private static final String CONFIG_URI = "configUri";
	private static final String LISTEN_URI = "listenUri";

	/**
	 * Main program for ConfigExampleServer.
	 * 
	 * @param args command line arguments.
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		// use our local configuration to find our remote configuration service
		// and keep us updated if it changes.
		
		local = new YamlConfig( new MyLocalConfigurationClient(), LOCAL );
		System.out.println( "loaded local configuration named '"+LOCAL+"'" );
		configUriId = local.getConfigPath( local.getRoot(), CONFIG_URI );
		System.out.println( "subscribing to local "+CONFIG_URI );
		local.subscribe( configUriId );
	}
	
	private static ConfigurationServer local;
	
	private static Object configUriId;
	
	private static class MyLocalConfigurationClient implements ConfigurationClient
	{
		public void configValuesChanged( Object[] updated )
		{
			for (Object id: updated)
			{
				if (id.equals( configUriId ))
				{
					try
					{
						configUriChanged();
					}
					catch ( Exception e )
					{
						e.printStackTrace();
					}
					continue;
				}
			}
		}
	}
	
	private static void configUriChanged() throws Exception
	{
		configUri = local.getString( configUriId );
		System.out.println( "local "+CONFIG_URI+" changed: "+configUri );
		
		RemoteConfigurationServer s = server;
		if (s != null)
		{
			System.out.println( "shutting down remote config service connection" );
			server = null;
			s._stop();
			s = null;
		}
		
		server = ConfigurationHelper.newServer( configUri, null,
			new MyConfigurationClientFactory() );
		System.out.println( "starting remote config service connection" );
		server._start();
	}
	
	private static String configUri;
	
	private static RemoteConfigurationServer server;
	
	private static class MyConfigurationClientFactory
		implements ConfigurationClientFactory
	{
		public ConfigurationClient newConfigurationClient(
			RemoteConfigurationServer server ) throws Exception
		{
			return new MyRemoteConfigurationClient( server );
		}
	}
	
	private static class MyRemoteConfigurationClient extends BaseConfigurationClient
	{
		public MyRemoteConfigurationClient( RemoteConfigurationServer server )
		{
			this.server = server;
		}
		
		private final RemoteConfigurationServer server;

		@Override
		public void _sessionNotify( Object event ) throws Exception
		{
			if (event == Session.UP)
			{
				System.out.println( "remote config service connection is up" );
				String name = new URL( configUri ).getUri();
				System.out.println( "loading remote configuration named '"+name+"'" );
				Object root = server.loadConfig( name );
				System.out.println( "loaded remote configuration named '"+name+"'" );
				listenUriId = server.getConfigPath( root, LISTEN_URI );
				System.out.println( "subscribing to remote "+LISTEN_URI );
				server.subscribe( listenUriId );
			}
		}
		
		private Object listenUriId;
		
		@Override
		public void configValuesChanged( Object[] updated )
		{
			for (Object id: updated)
			{
				if (id.equals( listenUriId ))
				{
					try
					{
						listenUriChanged();
					}
					catch ( Exception e )
					{
						e.printStackTrace();
					}
					continue;
				}
			}
		}

		private void listenUriChanged() throws Exception
		{
			String listenUri = server.getString( listenUriId );
			System.out.println( "remote "+LISTEN_URI+" changed: "+listenUri );
			resetListener( listenUri );
		}
	}

	private static void resetListener( String listenUri ) throws Exception
	{
		ServerFactory l = listener;
		if (l != null)
		{
			System.out.println( "stopping listener "+l );
			listener = null;
			l.transportControl( Transport.STOP, null );
			l = null;
		}
		
		listener = ConfigExampleHelper.newListener( listenUri, null, factory );
		
		// Start the Listener
		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		System.out.println( "started listener "+listener );
	}
	
	private static ServerFactory listener;
	
	private final static MainConfigExampleListener factory =
		new MainConfigExampleListener();

	public ConfigExampleServer newConfigExampleServer(
		RemoteConfigExampleClient client ) throws Exception
	{
		return new ImplConfigExampleServer( client );
	}
}

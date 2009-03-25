/*
 * $Id$
 * 
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership. The ASF
 * licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
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
public class MainConfigExampleListener4 implements ConfigExampleServerFactory
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
		// Create the server session factory.
		factory = new MainConfigExampleListener4();
		
		// Create the config client factory.
		configClientFactory = new MyConfigurationClientFactory();
		
		// Open the local config file.
		local = new YamlConfig( new MyLocalConfigurationClient(), LOCAL );
		System.out.println( "loaded local configuration named '" + LOCAL + "'" );

		// Subscribe to changes in the local config file.
		local.subscribe( local.getRoot() );
		System.out.println( "subscribed to changes in '" + LOCAL + "'" );
	}

	private static ConfigExampleServerFactory factory;
	
	private static ConfigurationClientFactory configClientFactory;

	private static ConfigurationServer local;

	private static class MyLocalConfigurationClient implements
		ConfigurationClient
	{
		public void configValuesChanged( Object[] updated )
		{
			try
			{
				String configUri = local.getStringPath( local.getRoot(), CONFIG_URI );
				System.out.println( "local " + CONFIG_URI + " changed: " + configUri );
				configUriChanged( configUri );
			}
			catch ( Exception e )
			{
				e.printStackTrace();
			}
		}
	}

	private static void configUriChanged( String configUri ) throws Exception
	{
		if (server != null)
		{
			System.out.println( "stopping remote config service" );
			server._stopAndWaitDown( 4000 );
			server = null;
		}

		server = ConfigurationHelper.newServer( configUri, null, configClientFactory );
		configName = new URL( configUri ).getUri();
		
		System.out.println( "starting remote config service connection "+server );
		server._start();
	}

	private static RemoteConfigurationServer server;
	
	private static String configName;

	private static class MyConfigurationClientFactory implements
		ConfigurationClientFactory
	{
		public ConfigurationClient newConfigurationClient(
			RemoteConfigurationServer server ) throws Exception
		{
			return new MyRemoteConfigurationClient( server );
		}
	}

	private static class MyRemoteConfigurationClient extends
		BaseConfigurationClient
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
				
				System.out.println( "loading remote configuration named '"
					+ configName + "'" );
				root = server.loadConfig( configName );
				System.out.println( "loaded remote configuration named '"
					+ configName + "'" );
				
				System.out.println( "subscribing to remote configuration" );
				server.subscribe( root );
			}
		}
		
		private Object root;

		@Override
		public void configValuesChanged( Object[] updated )
		{
			try
			{
				String uri = server.getStringPath( root, LISTEN_URI );
				System.out.println( "listen uri = " + uri );
				resetListener( uri );
			}
			catch ( Exception e )
			{
				e.printStackTrace();
			}
		}
	}

	private static void resetListener( String listenUri ) throws Exception
	{
		// Stop any old listener.
		if (listener != null)
		{
			System.out.println( "stopping listener " + listener );
			listener.transportControl( Transport.STOP_AND_WAIT_DOWN, 4000 );
			listener = null;
		}

		// Create the listener stack.
		listener = ConfigExampleHelper.newListener( listenUri, null, factory );
		System.out.println( "listener created " + listener );
		
		// Start the Listener
		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		System.out.println( "listener started " + listener );
	}

	private static ServerFactory listener;

	public ConfigExampleServer newConfigExampleServer(
		RemoteConfigExampleClient client )
	{
		return new ImplConfigExampleServer( client );
	}
}

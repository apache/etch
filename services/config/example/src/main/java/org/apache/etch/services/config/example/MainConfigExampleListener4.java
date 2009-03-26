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
 * connections from MainConfigExampleClient. This is a standard etch listener
 * modified to load the listen uri from a remote config whenever it changes.
 * The remote config is defined in a local config, which is also monitored for
 * changes.
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
		
		// Create the remote config client factory.
		configClientFactory = new MyConfigurationClientFactory();
		
		// Open the local config.
		local = new YamlConfig( new MyLocalConfigurationClient(), LOCAL );
		System.out.println( "loaded local configuration named '" + LOCAL + "'" );

		// Subscribe to changes in the local config.
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
			System.out.println( "local config changed" );
			try
			{
				resetRemoteConfig();
			}
			catch ( Exception e )
			{
				e.printStackTrace();
			}
		}
	}

	private static void resetRemoteConfig() throws Exception
	{
		if (remote != null)
		{
			System.out.println( "stopping remote config" );
			remote._stopAndWaitDown( 4000 );
			remote = null;
		}
		
		// Get the remote config uri from the local config.
		configUri = local.getStringPath( local.getRoot(), CONFIG_URI );
		System.out.println( "remote config uri = " + configUri );

		// Create the remote config stack.
		remote = ConfigurationHelper.newServer( configUri, null, configClientFactory );
		System.out.println( "remote config created "+remote );
		
		// Start the remote config stack.
		System.out.println( "starting remote config "+remote );
		remote._start();
	}

	private static String configUri;
	
	private static RemoteConfigurationServer remote;

	private static class MyConfigurationClientFactory implements
		ConfigurationClientFactory
	{
		public ConfigurationClient newConfigurationClient(
			RemoteConfigurationServer server ) throws Exception
		{
			return new MyRemoteConfigurationClient();
		}
	}

	private static class MyRemoteConfigurationClient extends
		BaseConfigurationClient
	{
		@Override
		public void _sessionNotify( Object event ) throws Exception
		{
			if (event == Session.UP)
			{
				System.out.println( "remote config service connection is up" );
				
				String configName = new URL( configUri ).getUri();
				System.out.println( "remote config name = '"
					+ configName + "'" );
				
				root = remote.loadConfig( configName );
				System.out.println( "loaded remote config named '"
					+ configName + "'" );
				
				System.out.println( "subscribing to remote config" );
				remote.subscribe( root );
			}
		}

		@Override
		public void configValuesChanged( Object[] updated )
		{
			try
			{
				resetListener();
			}
			catch ( Exception e )
			{
				e.printStackTrace();
			}
		}
	}
	
	private static Object root;

	private static void resetListener() throws Exception
	{
		// Stop the old listener.
		if (listener != null)
		{
			System.out.println( "stopping listener " + listener );
			listener.transportControl( Transport.STOP_AND_WAIT_DOWN, 4000 );
			listener = null;
		}
		
		// Get the listener uri from the remote config.
		String uri = remote.getStringPath( root, LISTEN_URI );
		System.out.println( "listen uri = " + uri );

		// Create the listener stack.
		listener = ConfigExampleHelper.newListener( uri, null, factory );
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

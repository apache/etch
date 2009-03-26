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
import org.apache.etch.services.config.ConfigurationClient;
import org.apache.etch.services.config.ConfigurationServer;
import org.apache.etch.services.config.YamlConfig;
import org.apache.etch.services.config.example.ConfigExampleHelper.ConfigExampleServerFactory;
import org.apache.etch.util.core.io.Transport;

/**
 * Main program for ConfigExampleServer. This program makes a listener to accept
 * connections from MainConfigExampleClient. This is a standard etch listener
 * modified to load the listen uri from a local config file at startup and
 * whenever the file changes.
 */
public class MainConfigExampleListener3 implements ConfigExampleServerFactory
{
	private static final String LOCAL = "services/config/example/local2";

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
		factory = new MainConfigExampleListener3();
		
		// Open the local config.
		local = new YamlConfig( new MyLocalConfigurationClient(), LOCAL );
		System.out.println( "loaded local configuration named '" + LOCAL + "'" );

		// Subscribe to changes in the local config.
		local.subscribe( local.getRoot() );
		System.out.println( "subscribed to changes in '" + LOCAL + "'" );
	}
	
	private static ConfigExampleServerFactory factory;
	
	private static ConfigurationServer local;
	
	private static class MyLocalConfigurationClient implements
		ConfigurationClient
	{
		public void configValuesChanged( Object[] updated )
		{
			System.out.println( "local config changed" );
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

	private static void resetListener() throws Exception
	{
		// Stop the old listener.
		if (listener != null)
		{
			System.out.println( "stopping listener " + listener );
			listener.transportControl( Transport.STOP_AND_WAIT_DOWN, 4000 );
			listener = null;
		}
		
		// Get the listener uri from the local config.
		String uri = local.getStringPath( local.getRoot(), LISTEN_URI );
		System.out.println( "listen uri = " + uri );
		
		// Create the listener stack.
		listener = ConfigExampleHelper.newListener( uri, null, factory );
		System.out.println( "listener created " + listener );

		// Start the listener stack.
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

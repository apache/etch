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
import org.apache.etch.services.config.ConfigurationServer;
import org.apache.etch.services.config.YamlConfig;
import org.apache.etch.services.config.example.ConfigExampleHelper.ConfigExampleServerFactory;
import org.apache.etch.util.core.io.Transport;

/**
 * Main program for ConfigExampleServer. This program makes a listener to accept
 * connections from MainConfigExampleClient. This is a standard etch listener
 * modified to load the listen uri from a local config at startup.
 */
public class MainConfigExampleListener2 implements ConfigExampleServerFactory
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
		// Open the local config.
		ConfigurationServer local = new YamlConfig( null, LOCAL );
		System.out.println( "loaded local configuration named '" + LOCAL + "'" );

		// Get the listener uri from the local config.
		String uri = local.getStringPath( local.getRoot(), LISTEN_URI );
		System.out.println( "listen uri = " + uri );

		// Create the listener stack.
		ServerFactory listener = ConfigExampleHelper.newListener( uri, null,
			new MainConfigExampleListener2() );
		System.out.println( "listener created " + listener );

		// Start the listener stack.
		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		System.out.println( "listener started " + listener );
	}

	public ConfigExampleServer newConfigExampleServer(
		RemoteConfigExampleClient client )
	{
		return new ImplConfigExampleServer( client );
	}
}

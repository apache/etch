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

package org.apache.etch.services.config;

import org.apache.etch.bindings.java.support.ObjSession;
import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.services.config.Configuration.ConfigurationException;
import org.apache.etch.util.core.io.Session;
import org.apache.etch.util.core.io.Transport;



/**
 * Main program for ConfigurationServer. This program makes a listener to accept
 * connections from MainConfigurationClient.
 */
public class MainConfigurationListener implements ConfigurationHelper.ConfigurationServerFactory
{
	/**
	 * Main program for ConfigurationServer.
	 * 
	 * @param args command line arguments.
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		// TODO Change to correct URI
		String uri = "tcp://0.0.0.0:4001";
		
		ServerFactory listener = ConfigurationHelper.newListener( uri, null,
			new MainConfigurationListener() );

		// Start the Listener
		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
	}

	public ConfigurationServer newConfigurationServer(
		RemoteConfigurationClient client ) throws ConfigurationException
	{
		return new MyYamlConfig( client );
	}
	
	/**
	 * Wrapper around YamlConfig which watches for {@link Session#DOWN} and
	 * unloads the config (thus canceling all subscriptions).
	 */
	public static class MyYamlConfig extends YamlConfig implements ObjSession
	{
		/**
		 * Constructs MyYamlConfig.
		 * @param client
		 * @throws ConfigurationException
		 */
		public MyYamlConfig( ConfigurationClient client ) throws ConfigurationException
		{
			super( client );
		}

		public Object _sessionQuery( Object query ) throws Exception
		{
			throw new UnsupportedOperationException( "unknown query "+query );
		}
		
		public void _sessionControl( Object control, Object value )
			throws Exception
		{
			throw new UnsupportedOperationException( "unknown control "+control );
		}

		public void _sessionNotify( Object event ) throws Exception
		{
			if (event == Session.DOWN)
				unloadConfig();
		}
	}
}

/* $Id$
 *
 * Copyright 2009-2010 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package cisco.uc.cuae.etchrouter.test.utils;

import cisco.uc.cuae.etchrouter.test.plugin.ImplTestPluginServer;
import cisco.uc.cuae.etchrouter.test.plugin.RemoteTestPluginClient;
import cisco.uc.cuae.etchrouter.test.plugin.TestPluginHelper;
import cisco.uc.cuae.etchrouter.test.plugin.TestPluginServer;
import etch.bindings.java.support.ServerFactory;
import etch.util.core.io.Transport;

public class PluginTestServerRunner implements TestPluginHelper.TestPluginServerFactory
{

	private String _name;
	private String _url;
	private int _numCalls = 0;
	
	Transport<ServerFactory> _listener = null;
	
	public PluginTestServerRunner( String name, String url )
	{
		_name = name;
		_url = url;
	}
	
	public void start() throws Exception
	{
		System.out.println(String.format("Starting plugin \"%s\" with URL \"%s\"...", _name, _url));
		_listener = TestPluginHelper.newListener( _url, null, this );
		// Start the Listener
		_listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
	}
	
	public void stop() throws Exception
	{
		System.out.println(String.format("Stopping plugin \"%s\" with URL \"%s\"...", _name, _url));
		_listener.transportControl( Transport.STOP_AND_WAIT_DOWN, 4000 );
	}

	public String getName()
	{
		return _name;
	}
	
	public synchronized void methodCalled()
	{
		_numCalls++;
	}
	
	public synchronized int getNumCalls()
	{
		return _numCalls;
	}
	
	public TestPluginServer newTestPluginServer( RemoteTestPluginClient client )
	{
		return new ImplTestPluginServer( client, this );
	}
}

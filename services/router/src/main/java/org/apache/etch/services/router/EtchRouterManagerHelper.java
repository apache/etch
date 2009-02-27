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
package org.apache.etch.services.router;

import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.DefaultServerFactory;
import org.apache.etch.bindings.java.support.DeliveryService;
import org.apache.etch.bindings.java.support.Pool;
import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.bindings.java.support.TransportHelper;
import org.apache.etch.bindings.java.transport.DefaultDeliveryService;
import org.apache.etch.bindings.java.transport.MailboxManager;
import org.apache.etch.bindings.java.transport.PlainMailboxManager;
import org.apache.etch.bindings.java.transport.TransportMessage;
import org.apache.etch.services.router.ConnectionStackInfo.ConnectionType;
import org.apache.etch.util.Resources;
import org.apache.etch.util.core.io.Transport;

/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class EtchRouterManagerHelper extends TransportHelper
{

	/**
	 * 
	 * @param routerMgr
	 * @param uri
	 * @param resources
	 * @param implFactory
	 * @return
	 * @throws Exception
	 */
	public static ServerFactory newListener( final EtchRouterManager routerMgr, final String uri,
		Resources resources )
		throws Exception
	{
		final Resources res = initResources( resources );
		res.put( EtchRouterManager._ETCH_ROUTER_MANAGER, routerMgr );

		final Transport<ServerFactory> listener = EtchRouterMgrTransportFactory.getListener( uri, res );
		
		return new DefaultServerFactory( listener, routerMgr )
		{
				public void newServer( String uri, Resources resources,
						TransportMessage transport ) throws Exception
				{
					ValueFactory vf = (ValueFactory) resources.get( Transport.VALUE_FACTORY );
					MailboxManager x = new PlainMailboxManager( transport, uri, res );
					DeliveryService d = new DefaultDeliveryService( x, uri, res );
					ConnectionStackInfo info = routerMgr.getConnectionStackInfo( d );
					ERRemoteClient client = new ERRemoteClient( d, vf, routerMgr, info );
					ImplServer server = new ImplServer( client, routerMgr );
					Pool qp = (Pool) res.get( QUEUED_POOL );
					Pool fp = (Pool) res.get( FREE_POOL );
					new ERStub( d, server, qp, fp );
					client._start();
				}
	
				public ValueFactory newValueFactory()
				{
					return new DynamicValueFactory( uri );
				}
				
				@Override
				public String toString()
				{
					return "PerfHelper.ServerFactory/" + listener;
				}
		};
	}
	
	/**
	 * Create a connection to an Etch plug-in server listener
	 *  
	 * @param etchRouterManager
	 * @param uri
	 * @param resources
	 * @return
	 * @throws Exception
	 */
	public static ERRemoteServer newServer( EtchRouterManager etchRouterManager, String uri,
		Resources resources, String pluginName) throws Exception
	{
		final Resources res = initResources( resources );
		final DynamicValueFactory vf = etchRouterManager.getValueFactory();
		res.put( Transport.VALUE_FACTORY, vf );
		res.put( EtchRouterManager._ETCH_ROUTER_MANAGER, etchRouterManager );

		DeliveryService d = EtchRouterMgrTransportFactory.getTransport( uri, res );
		ConnectionStackInfo info = etchRouterManager.getConnectionStackInfo( d );
		info.setConnectionType( ConnectionType.PLUGIN_SERVER_CONN, pluginName );
		ERRemoteServer server = new ERRemoteServer( d, vf, etchRouterManager, info );
		ImplClient client = new ImplClient( server, etchRouterManager );
		Pool qp = (Pool) res.get( QUEUED_POOL );
		Pool fp = (Pool) res.get( FREE_POOL );
		new ERStub( d, client, qp, fp );
		return server;
	}
}

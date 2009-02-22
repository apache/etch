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
package cisco.uc.cuae.etchrouter;

import cisco.uc.cuae.etchrouter.ConnectionStackInfo.ConnectionType;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.support.DefaultServerFactory;
import etch.bindings.java.support.DeliveryService;
import etch.bindings.java.support.Pool;
import etch.bindings.java.support.ServerFactory;
import etch.bindings.java.support.TransportHelper;
import etch.util.Resources;
import etch.util.core.io.Transport;

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
	public static Transport<ServerFactory> newListener( final EtchRouterManager routerMgr, final String uri,
		Resources resources )
		throws Exception
	{
		final Resources res = initResources( resources );
		res.put( EtchRouterManager._ETCH_ROUTER_MANAGER, routerMgr );

		return EtchRouterMgrTransportFactory.getListener( uri, res,
			new DefaultServerFactory( routerMgr )
			{
				public void newServer( DeliveryService d, ValueFactory vf ) throws Exception
				{
					ConnectionStackInfo info = routerMgr.getConnectionStackInfo( d );
					ERRemoteClient client = new ERRemoteClient( d, vf, routerMgr, info );
					ImplServer server = new ImplServer( client, routerMgr );
					Pool qp = (Pool) res.get( QUEUED_POOL );
					Pool fp = (Pool) res.get( FREE_POOL );
					new ERStub( d, server, qp, fp );
				}
	
				public ValueFactory newValueFactory()
				{
					return new DynamicValueFactory( uri );
				}
			} );
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

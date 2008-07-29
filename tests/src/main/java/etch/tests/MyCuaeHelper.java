/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
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

package etch.tests;

import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.support.DefaultServerFactory;
import etch.bindings.java.support.DeliveryService;
import etch.bindings.java.support.Pool;
import etch.bindings.java.support.ServerFactory;
import etch.bindings.java.support.TransportFactory;
import etch.bindings.java.transport.SessionMessage;
import etch.util.Resources;
import etch.util.core.io.Transport;

/**
 * Transport helper for Cuae. All methods are static.
 */
abstract public class MyCuaeHelper extends CuaeHelper
{
	/**
	 * Constructs a new server session listener per specifications in uri and
	 * resources. This listener will accept requests from clients for new server
	 * sessions.
	 *
	 * @param uri contains specifications for the server session listener and
	 * for the server session transport stack.
	 *
	 * @param resources additional resources to aid in constructing new server
	 * sessions.
	 *
	 * @param implFactory factory used to construct a new instance implementing
	 * CuaeServer. The new instance will receive and process messages from
	 * the client session.
	 *
	 * @return a server session listener.
	 *
	 * @throws Exception
	 */
	public static Transport<ServerFactory> newListener( final String uri, Resources resources,
		final MyCuaeServerFactory implFactory ) throws Exception
	{
		final Resources res = initResources( resources );
		if (!res.containsKey( Transport.VALUE_FACTORY ))
			res.put( Transport.VALUE_FACTORY, new MyValueFactoryCuae( "tcp:" ) );

		return TransportFactory.getListener( uri, res, new DefaultServerFactory( implFactory )
		{
			public void newServer( DeliveryService d, ValueFactory vf ) throws Exception
			{
				Pool qp = (Pool) res.get( QUEUED_POOL );
				Pool fp = (Pool) res.get( FREE_POOL );
				implFactory.newMyCuaeServer( d, qp, fp, (MyValueFactoryCuae) vf );
			}

			public ValueFactory newValueFactory()
			{
				return new MyValueFactoryCuae( uri );
			}
		} );
	}

	/**
	 * Factory for a Listener to use to create new instances of MyCuaeServer.
	 */
	public interface MyCuaeServerFactory
	{
		/**
		 * @param svc
		 * @param qp
		 * @param fp
		 * @param vf
		 * @return a StubIntf that accepts and processes server messages.
		 */
		public SessionMessage newMyCuaeServer( DeliveryService svc, Pool qp,
			Pool fp, MyValueFactoryCuae vf );
	}
}

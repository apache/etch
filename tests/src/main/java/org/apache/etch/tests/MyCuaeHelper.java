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

package org.apache.etch.tests;

import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.DefaultServerFactory;
import org.apache.etch.bindings.java.support.DeliveryService;
import org.apache.etch.bindings.java.support.Pool;
import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.bindings.java.support.TransportFactory;
import org.apache.etch.bindings.java.transport.DefaultDeliveryService;
import org.apache.etch.bindings.java.transport.MailboxManager;
import org.apache.etch.bindings.java.transport.PlainMailboxManager;
import org.apache.etch.bindings.java.transport.SessionMessage;
import org.apache.etch.bindings.java.transport.TransportMessage;
import org.apache.etch.util.Resources;
import org.apache.etch.util.core.io.Transport;



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
	public static ServerFactory newListener( final String uri, Resources resources,
		final MyCuaeServerFactory implFactory ) throws Exception
	{
		final Resources res = initResources( resources );

		Transport<ServerFactory> listener =  TransportFactory.getListener( uri, res );
		
		return new DefaultServerFactory( listener, implFactory )
		{
			public void newServer( TransportMessage t, String uri, Resources r ) throws Exception
			{
				ValueFactory vf = (ValueFactory) r.get( Transport.VALUE_FACTORY );
				MailboxManager x = new PlainMailboxManager( t, uri, r );
				DeliveryService d = new DefaultDeliveryService( x, uri, r );
				Pool qp = (Pool) r.get( QUEUED_POOL );
				Pool fp = (Pool) r.get( FREE_POOL );
				implFactory.newMyCuaeServer( d, qp, fp, (MyValueFactoryCuae) vf );
				d.transportControl( START, null );
			}

			public ValueFactory newValueFactory( String uri )
			{
				return new MyValueFactoryCuae( uri );
			}
		};
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

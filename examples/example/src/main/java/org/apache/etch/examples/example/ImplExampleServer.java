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

package org.apache.etch.examples.example;

import java.util.Date;

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.ComboStub;
import org.apache.etch.bindings.java.support.ComboValueFactory;
import org.apache.etch.bindings.java.support.DeliveryService;
import org.apache.etch.bindings.java.support.Pool;
import org.apache.etch.bindings.java.support.RemoteBase;
import org.apache.etch.bindings.java.support.RetryMessageException;
import org.apache.etch.bindings.java.support.TransportHelper;
import org.apache.etch.bindings.java.transport.UnwantedMessage;
import org.apache.etch.examples.example.FooHelper.FooServerFactory;
import org.apache.etch.util.Resources;
import org.apache.etch.util.core.io.Transport;


/**
 * Your custom implementation of BaseExampleServer. Add methods here to provide
 * implementations of messages from the client.
 */
public class ImplExampleServer extends BaseExampleServer
{
	/**
	 * Constructs the ImplExampleServer.
	 *
	 * @param client a connection to the client session. Use this to send a
	 * message to the client.
	 */
	public ImplExampleServer( RemoteExampleClient client )
	{
		this.client = client;
	}
	
	/**
	 * A connection to the client session. Use this to send a
	 * message to the client.
	 */
	@SuppressWarnings( "unused" )
	private final RemoteExampleClient client;

	@Override
	public Integer helloServer( String msg, Date d )
	{
		return msg.hashCode();
	}
	
	@Override
	public void _sessionNotify( Object event ) throws Exception
	{
		if (event instanceof UnwantedMessage)
		{
			Message msg = ((UnwantedMessage) event).msg;
			System.out.println( "unwanted msg "+msg );
			
			Type type = msg.type();
			String uri = (String) client._transportQuery( "uri" );
			
			// try to load Foo

			ValueFactoryFoo nvf = new ValueFactoryFoo( uri );
			if (nvf.getType( type.getId() ) != null)
			{
				FooHelper.addServer( nvf, client );
				throw new RetryMessageException();
			}
			
			// no other service to try, fall through...
		}
		
		if (event instanceof Throwable)
			((Throwable) event).printStackTrace();
		else
			System.out.println( "event = "+event );
	}
}
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

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.support.RetryMessageException;
import org.apache.etch.bindings.java.transport.UnwantedMessage;

/**
 * Your custom implementation of BaseFooServer. Add methods here to provide
 * implementations of messages from the client.
 */
public class ImplFooServer extends BaseFooServer
{
	/**
	 * Constructs the ImplFooServer.
	 *
	 * @param client a connection to the client session. Use this to send a
	 * message to the client.
	 */
	public ImplFooServer( RemoteFooClient client )
	{
		this.client = client;
	}
	
	/**
	 * A connection to the client session. Use this to send a
	 * message to the client.
	 */
	private final RemoteFooClient client;
	
	@Override
	public Integer foo( String msg )
	{
		return msg.hashCode();
	}
	
	@Override
	public void _sessionNotify( Object event ) throws Exception
	{
		if (event instanceof UnwantedMessage)
		{
			Message msg = ((UnwantedMessage) event).msg;
			Type type = msg.type();
			System.out.println( "unknown msg type "+type );
			
			String uri = (String) client._transportQuery( "uri" );
			
			// try to load ValueFactoryExample

			ValueFactoryExample nvf = new ValueFactoryExample( uri );
			if (nvf.getType( type.getId() ) != null)
			{
				System.out.println( "loading "+ValueFactoryExample.class.getName() );
				ExampleHelper.addServer( nvf, client );
				throw new RetryMessageException();
			}
			
			// no other service to try, fall through...
		}
	}
}
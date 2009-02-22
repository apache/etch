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

package etch.services.ns;

import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

/**
 * Your custom implementation of BaseNameServiceServer. Add methods here to provide
 * implementations of messages from the client.
 */
public class ImplNameServiceServer extends BaseNameServiceServer
{
	/**
	 * Constructs the ImplNameServiceServer.
	 *
	 * @param client a connection to the client session. Use this to send a
	 * message to the client.
	 */
	public ImplNameServiceServer( RemoteNameServiceClient client )
	{
		this.client = client;
	}
	
	/**
	 * A connection to the client session. Use this to send a
	 * message to the client.
	 */
	@SuppressWarnings( "unused" )
	private final RemoteNameServiceClient client;
	
	private static Map<String, Entry> nsEntries =
		Collections.synchronizedMap( new HashMap<String, Entry>() );
	
	@Override
	public Boolean canRegister(String sourceUri, Map<?, ?> qualities,
			String targetUri) 
	{
		boolean canRegister = true;
		
		// Logic for canRegister goes here
		
		return canRegister;
	}
	
	@Override
	public void register(String sourceUri, Map<?, ?> qualities,
			String targetUri, Integer ttl) 
	{
		
		Entry newEntry = new Entry( sourceUri, qualities, targetUri, ttl, null, new Date(), false );
		nsEntries.put( sourceUri, newEntry );
		
		printLookupTable();
	}
	
	@Override
	public Boolean canLookup( String source )
	{
		boolean canLookup = true;
		
		// Logic for canLookup
		
		return canLookup;
	}
	
	@Override
	public Entry lookup( String source )
	{
		return nsEntries.get( source );
	}
	
	@Override
	public Boolean canUnregister( String sourceUri, Boolean deleteEntry )
	{
		boolean canUnregister = true;
		
		// Logic for canUnregister
		
		return canUnregister;
	}
	
	@Override
	public void unregister( String sourceUri, Boolean deleteEntry )
	{
		if ( deleteEntry )
			nsEntries.remove( sourceUri );
		else
		{
			nsEntries.get( sourceUri ).targetUri = null;
			nsEntries.get( sourceUri ).removed = true;
		}
		printLookupTable();
	}
	
	private void printLookupTable()
	{
		System.out.println("\nLookup Table: ");
		System.out.println("---------------\n");
		System.out.println(nsEntries.toString());
	}
}
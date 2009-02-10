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

package org.apache.etch.examples.distmap;

import org.apache.etch.examples.distmap.DistributedHashTable.Entry;
import org.apache.etch.util.Log;


/**
 * Main program for DistributedHashTableClient. This program makes a connection to the
 * listener created by MainDistributedHashTableListener.
 */
public class MainDistributedHashTableClient implements DistributedHashTableHelper.DistributedHashTableClientFactory
{
	/**
	 * Main program for DistributedHashTableClient.
	 * 
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		String uri = "tcp://localhost:4007";
		
		RemoteDistributedHashTableServer server = DistributedHashTableHelper.newServer( uri, null,
			new MainDistributedHashTableClient() );

		server._startAndWaitUp( 4000 );

		final boolean myBool1 = true;
		final byte myByte1 = 1;
		final short myShort1 = 234;
		final int myInt1 = 345345;
		final long myLong1 = 456456456456L;
		final float myFloat1 = 1.234f;
		final double myDouble1 = 2.345;
		final String myString1 = "abc";	
		
		Object myObj;

		// Start testing
		Log.addSink( null );
		
		// size - DHT is empty
		Log.report("DHT", "Size", server.size());
		
		// getAll - DHT is empty
		for(Entry entry: server.getAll())
			Log.report("getAll: ", entry.key, entry.value);
		
		// putObject for each constant type
		server.putObject("boolean1", myBool1);
		Log.report("putObject", "boolean value", myBool1);
		
		server.putObject("byte1", myByte1);
		Log.report("putObject", "byte value", myByte1);
		
		server.putObject("short1", myShort1);
		Log.report("putObject", "short value", myShort1);

		server.putObject("int1", myInt1);
		Log.report("putObject", "int value", myInt1);

		server.putObject("long1", myLong1);
		Log.report("putObject", "long value", myLong1);

		server.putObject("float1", myFloat1);
		Log.report("putObject", "float value", myFloat1);

		server.putObject("double1", myDouble1);
		Log.report("putObject", "double value", myDouble1);
		
		server.putObject("string1", myString1);
		Log.report("putObject", "string value", myString1);	
		
		// size - DHT has entries
		Log.report("DHT", "Size", server.size());
		
		// getAll - DHT has entries
		for(Entry entry: server.getAll())
			Log.report("getAll: ", entry.key, entry.value);
			
		// getObject for each constant type
		myObj = server.getObject("boolean1");
		Log.report("getObject", "myBool1", myObj);
		
		myObj = server.getObject("byte1");
		Log.report("getObject", "myByte1", myObj);	
		
		myObj = server.getObject("short1");
		Log.report("getObject", "myShort1", myObj);	
		
		myObj = server.getObject("int1");
		Log.report("getObject", "myInt1", myObj);	
		
		myObj = server.getObject("long1");
		Log.report("getObject", "myLong1", myObj);	
		
		myObj = server.getObject("float1");
		Log.report("getObject", "myFloat1", myObj);	
		
		myObj = server.getObject("double1");
		Log.report("getObject", "myDouble1", myObj);	
		
		myObj = server.getObject("string1");
		Log.report("getObject", "myString1", myObj);
		
		// removeObject for each constant type
		myObj = server.removeObject("boolean1");
		Log.report("removeObject", "boolean value", myObj);
		
		myObj = server.removeObject("byte1");
		Log.report("removeObject", "byte value", myObj);	
		
		myObj = server.removeObject("short1");
		Log.report("removeObject", "short value", myObj);	
		
		myObj = server.removeObject("int1");
		Log.report("removeObject", "int value", myObj);	
		
		myObj = server.removeObject("long1");
		Log.report("removeObject", "long value", myObj);	
		
		myObj = server.removeObject("float1");
		Log.report("removeObject", "float value", myObj);	
		
		myObj = server.removeObject("double1");
		Log.report("removeObject", "double value", myObj);	
				
		myObj = server.removeObject("string1");
		Log.report("removeObject", "string value", myObj);
		
		// size - Verify that DHT is empty after removing all items
		Log.report("DHT", "Size", server.size());
		
		// getAll - Verify that DHT is empty after removing all items
		for(Entry entry: server.getAll())
			Log.report("getAll: ", entry.key, entry.value);
				
		Log.report("Have a nice day!");

		server._stopAndWaitDown( 4000 );
	}

	public DistributedHashTableClient newDistributedHashTableClient( RemoteDistributedHashTableServer server )
		throws Exception
	{
		return new ImplDistributedHashTableClient( server );
	}
}

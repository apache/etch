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

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.examples.distmap.DistributedHashTable.Entry;
import org.apache.etch.util.Log;
import org.apache.etch.util.core.io.Transport;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;


/**
 * Tests the RemoteDistributedHashTableServer interface. Start the
 * TestRemoteDistributedHashTableListener first.
 */
public class TestRemoteDistributedHashTableServer
{
	final boolean myBool1 = true;
	final byte myByte1 = 127;
	final short myShort1 = 32767;
	final int myInt1 = 2147483647;
	final long myLong1 = 9223372036854775807l;
	final float myFloat1 = 3.4028235e38f;
	final double myDouble1 = 1.7976931348623157e308d;
	final String myString1 = "abc";
	RemoteDistributedHashTableServer server;
	
	final Map<String, Object> map =
		Collections.synchronizedMap( new HashMap<String, Object>() );
	
	/** transport */
	public static ServerFactory listener;

	/**
	 * Connect to service
	 * @throws Exception 
	 */
	@BeforeClass
	public static void startRemoteDistributedHashTableListener()
		throws Exception
	{
		Log.addSink(null);
		Log.report("StartRemoteDistributedHashTableListener");
			
		String uri = "tcp://localhost:4007";
		MainDistributedHashTableListener implFactory = new MainDistributedHashTableListener();
		listener = DistributedHashTableHelper.newListener( uri, null, implFactory );

		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
	
		Log.report( "StartDistributedHashTableRemoteListener",
				"status", "up",
				"listener", listener );
	}

	/**
	 * Shut down listener
	 * @throws Exception
	 */
	@AfterClass
	public static void shutListener()
		throws Exception
	{
		if (listener != null)
		{
			listener.transportControl( Transport.STOP_AND_WAIT_DOWN, 4000 );
			listener = null;
		}
	}	
	
	/**
	 * Connect client to serve
	 * @throws Exception
	 */
	@Before
	public void makeConnection()
		throws Exception
	{
		String uri = "tcp://localhost:4007";

		server = DistributedHashTableHelper.newServer( uri, null, new MainDistributedHashTableClient() );

		server._startAndWaitUp( 4000 );
	}

	/**
	 * Shutdown connection to server
	 * @throws Exception
	 */
	@After
	public void shutConnection()
		throws Exception
	{
		if (server != null)
		{
			server._stop();
		}
	}

	/**
	 * Verify that putObject, getObject and removeObject
	 * work for a boolean type 
	 * @throws Exception
	 */
	@Test
	public void testBoolean()
		throws Exception
	{
		Object myObj = server.putObject("boolean1", myBool1);
		assertNull( myObj );

		myObj = server.getObject("boolean1");
		assertEquals( myBool1, myObj );

		server.removeObject("boolean1");
		myObj = server.getObject("boolean1");
		assertNull( myObj );
	}
	
	/**
	 * Verify that putObject, getObject and removeObject
	 * work for a byte type 
	 * @throws Exception
	 */
	@Test
	public void testByte()
		throws Exception
	{
		Object myObj = server.putObject("byte1", myByte1);
		assertNull( myObj );

		myObj = server.getObject("byte1");
		assertEquals( myByte1, myObj );

		server.removeObject("byte1");
		myObj = server.getObject("byte1");
		assertNull( myObj );
	}

	/**
	 * Verify that putObject, getObject and removeObject
	 * work for a short type 
	 * @throws Exception
	 */
	@Test
	public void testShort()
		throws Exception
	{
		Object myObj = server.putObject("short1", myShort1);
		assertNull( myObj );

		myObj = server.getObject("short1");
		assertEquals( myShort1, myObj );

		server.removeObject("short1");
		myObj = server.getObject("short1");
		assertNull( myObj );
	}

	/**
	 * Verify that putObject, getObject and removeObject
	 * work for an int type 
	 * @throws Exception
	 */
	@Test
	public void testInt()
		throws Exception
	{
		Object myObj = server.putObject("int1", myInt1);
		assertNull( myObj );

		myObj = server.getObject("int1");
		assertEquals( myInt1, myObj );

		server.removeObject("int1");
		myObj = server.getObject("int1");
		assertNull( myObj );
	}

	/**
	 * Verify that putObject, getObject and removeObject
	 * work for a long type 
	 * @throws Exception
	 */
	@Test
	public void testLong()
		throws Exception
	{
		Object myObj = server.putObject("long1", myLong1);
		assertNull( myObj );

		myObj = server.getObject("long1");
		assertEquals( myLong1, myObj );

		server.removeObject("long1");
		myObj = server.getObject("long1");
		assertNull( myObj );
	}

	/**
	 * Verify that putObject, getObject and removeObject
	 * work for a float type 
	 * @throws Exception
	 */
	@Test
	public void testFloat()
		throws Exception
	{
		Object myObj = server.putObject("float1", myFloat1);
		assertNull( myObj );

		myObj = server.getObject("float1");
		assertEquals( myFloat1, myObj );

		server.removeObject("float1");
		myObj = server.getObject("float1");
		assertNull( myObj );
	}

	/**
	 * Verify that putObject, getObject and removeObject
	 * work for a double type 
	 * @throws Exception
	 */
	@Test
	public void testDouble()
		throws Exception
	{
		Object myObj = server.putObject("double1", myDouble1);
		assertNull( myObj );

		myObj = server.getObject("double1");
		assertEquals( myDouble1, myObj );

		server.removeObject("double1");
		myObj = server.getObject("double1");
		assertNull( myObj );
	}

	/**
	 * Verify that putObject, getObject and removeObject
	 * work for a string type 
	 * @throws Exception
	 */
	@Test
	public void testString()
		throws Exception
	{
		Object myObj = server.putObject("string1", myString1);
		assertNull( myObj );

		myObj = server.getObject("string1");
		assertEquals( myString1, myObj );

		server.removeObject("string1");
		myObj = server.getObject("string1");
		assertNull( myObj );
	}
	
	/**
	 * Verify size
	 * @throws Exception
	 */
	@Test
	public void testSize()
		throws Exception
	{
		/*
		 * Verify that size is initially zero
		 */
		int size = server.size();
		assertEquals(size, 0);

		/*
		 * Add items and verify size
		 */
		server.putObject("string1", "abc");
		server.putObject("string2", "xyz");
		server.putObject("int1", 0);
		server.putObject("int2", 125);

		size = server.size();
		assertEquals(size,4);
		
		/*
		 * Remove items and verify size
		 */
		server.removeObject("string1");
		server.removeObject("string2");
		server.removeObject("int1");
		server.removeObject("int2");

		size = server.size();
		assertEquals(size, 0);
	}
	
	/**
	 * Verify that getAll returns all entries 
	 * @throws Exception
	 */
	@Test
	public void testGetAll()
		throws Exception
	{
		/*
		 * Verify that map is empty to start
		 */
		int size = server.size();
		assertEquals(size, 0);

		/*
		 * Put items in map
		 */
		server.putObject("string1", "abc");
		server.putObject("string2", "xyz");
		server.putObject("int1", 0);
		server.putObject("int2", 125);

		/*
		 * Verify that 4 items were inserted into the map
		 */
		int counter = 0;
		for(Entry entry: server.getAll())
		{
			if(entry.key != null && entry.value != null)
			{
				counter++;
			}
		}

		assertEquals(counter, 4);

		/*
		 * Remove all items added to map
		 */
		server.removeObject("string1");
		server.removeObject("string2");
		server.removeObject("int1");
		server.removeObject("int2");

		/*
		 * Verify that map is empty when all known items are removed
		 */
		size = server.size();
		assertEquals(size, 0);
	}
}

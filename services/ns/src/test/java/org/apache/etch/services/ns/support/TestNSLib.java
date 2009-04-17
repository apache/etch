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

package org.apache.etch.services.ns.support;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.services.ns.MainNameServiceListener;
import org.apache.etch.services.ns.NameServiceHelper;
import org.apache.etch.services.ns.NameService.Entry;
import org.apache.etch.services.ns.support.NSLib;
import org.apache.etch.util.core.io.Session;
import org.apache.etch.util.core.io.Transport;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;


public class TestNSLib
{
	private static String defaultNSUri = null;
	private static String listenerUri = "etch:sourceListener?suri=listenerUri&curi=targetListener&nsuri=tcp://127.0.0.1:8002";
	private static String clientUri = "etch://127.0.0.1:8002/sourceListener";
	private static String testSourceUri1 = "sourceListener";
	private static String testtargetUri1 = "targetListener";
	private static ServerFactory nsListener; 
	
	@BeforeClass
	public static void startNS()
	{
		defaultNSUri = NSLib.staticGetDefaultNsUri();
		try
		{
			nsListener = NameServiceHelper.newListener( defaultNSUri, null,
					new MainNameServiceListener() );
			nsListener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		}
		catch( Exception e )
		{
			System.out.println( e );
		}
	}
	
	@Test ( expected = IllegalArgumentException.class )
	public void register1()
	{
		// null case
		
		NSLib.staticRegister( null, null, null, 0 );
	}
	
	@Test( expected = IllegalArgumentException.class )
	public void register2() throws Exception
	{
		String uri = "suri=listenerUri&curi=targetListener&nsuri=tcp://127.0.0.1:8002";
		NSLib.staticRegister( new MySession(), uri, null, 0 );
	}
	
	@Test
	public void register4() throws Exception
	{
		MySession session = new MySession();
		
		NSLib.staticRegister( session, listenerUri, null, 0 );
		
		// small delay to let registration complete
		Thread.sleep( 250 );
		
		Entry e = NSLib.staticLookup( clientUri );
		
		assertEquals( testSourceUri1, e.sourceUri );
		assertEquals( testtargetUri1, e.targetUri );
		assertEquals( 0, e.ttl );
		assertEquals( null, e.qualities );
		
		NSLib.staticUnregister( session, listenerUri, false );
		
		Entry e1 = NSLib.staticLookup( clientUri );
		
		assertEquals( testSourceUri1, e1.sourceUri );
		assertEquals( null, e1.targetUri );	// since removed
		assertEquals( 0, e1.ttl );
		assertEquals( null, e1.qualities );
	}
	
	@Test
	public void register5() throws Exception
	{
		// Don't pass the nsUri
		// everything should work because of defaultNSUri
		
		MySession session = new MySession();
		
		String uri = "etch:sourceListener?suri=listenerUri&curi=targetListener";
		
		NSLib.staticRegister( session, uri, null, 0 );
		
		// small delay to let registration complete
		Thread.sleep( 250 );
		
		String curi = "etch:sourceListener";
		Entry e = NSLib.staticLookup( clientUri );
		
		assertEquals( testSourceUri1, e.sourceUri );
		assertEquals( testtargetUri1, e.targetUri );
		assertEquals( 0, e.ttl );
		assertEquals( null, e.qualities );

		NSLib.staticUnregister( session, uri, false );
		
		Entry e1 = NSLib.staticLookup( curi );
		
		assertEquals( testSourceUri1, e1.sourceUri );
		assertEquals( null, e1.targetUri );	// since removed
		assertEquals( 0, e1.ttl );
		assertEquals( null, e1.qualities );
	}
	
	@Test
	public void register6() throws Exception
	{
		// 1. Don't pass the nsUri
		// 2. Pass "true" in unregister
		// Verify that entry was deleted
		
		MySession session = new MySession();
		
		String uri = "etch:sourceListener?suri=listenerUri&curi=targetListener";
		
		NSLib.staticRegister( session, uri, null, 0 );
		
		// small delay to let registration complete
		Thread.sleep( 250 );
		
		String curi = "etch:sourceListener";
		Entry e = NSLib.staticLookup( clientUri );
		
		assertEquals( testSourceUri1, e.sourceUri );
		assertEquals( testtargetUri1, e.targetUri );
		assertEquals( 0, e.ttl );
		assertEquals( null, e.qualities );

		NSLib.staticUnregister( session, uri, true );
		
		Entry e1 = NSLib.staticLookup( curi );
		
		assertNull( e1 );
	}
	
	@Test
	public void register7() throws Exception
	{
		// register 3 listeners
		
		MySession session = new MySession();
		NSLib.staticRegister( session, listenerUri, null, 0 );
		
		MySession session2 = new MySession();
		String listenerUri1 = "etch:sourceListener2?suri=listenerUri&curi=targetListener2&nsuri=tcp://127.0.0.1:8002";
		NSLib.staticRegister( session2, listenerUri1, null, 0 );
		
		MySession session3 = new MySession();
		String listenerUri2  = "etch:sourceListener3?suri=listenerUri&curi=targetListener3&nsuri=tcp://127.0.0.1:8002";
		NSLib.staticRegister( session3, listenerUri2, null, 0 );
		
		Thread.sleep( 250 );
		
		Entry e = NSLib.staticLookup( clientUri );
		assertEquals( testSourceUri1, e.sourceUri );
		assertEquals( testtargetUri1, e.targetUri );
		assertEquals( 0, e.ttl );
		assertEquals( null, e.qualities );
		
		String clientUri1 = "etch://127.0.0.1:8002/sourceListener2";
		e = NSLib.staticLookup( clientUri1 );
		assertEquals( testSourceUri1 + "2", e.sourceUri );
		assertEquals( testtargetUri1 + "2", e.targetUri );
		assertEquals( 0, e.ttl );
		assertEquals( null, e.qualities );
		
		String clientUri2 = "etch://127.0.0.1:8002/sourceListener3";
		e = NSLib.staticLookup( clientUri2 );
		assertEquals( testSourceUri1 + "3", e.sourceUri );
		assertEquals( testtargetUri1 + "3", e.targetUri );
		assertEquals( 0, e.ttl );
		assertEquals( null, e.qualities );
		
		NSLib.staticUnregister( session, listenerUri, true );
		NSLib.staticUnregister( session2, listenerUri1, true );
		NSLib.staticUnregister( session3, listenerUri2, true );
		
		/**
		 * Just to make sure that the above entries are removed from the name service
		 * (because we reconnect after every RECONNECT_DELAY millis)
		 */ 
		Thread.sleep( NSLib.RECONNECT_DELAY + 1000 );
		
		e = NSLib.staticLookup( clientUri );
		assertNull( e );
		
		e = NSLib.staticLookup( clientUri1 );
		assertNull( e );
		
		e = NSLib.staticLookup( clientUri2 );
		assertNull( e );
	}
	
	@Test ( expected = IllegalArgumentException.class )
	public void lookup1() throws Exception
	{
		NSLib.staticLookup( null );
	}
	
	@Test
	public void lookup2() throws Exception
	{
		// verify that you don't need a register to do a lookup
		NSLib.staticLookup( clientUri );
	}
	
	@Test
	public void lookup3() throws Exception
	{
		MySession session = new MySession();
		NSLib.staticRegister( session, listenerUri, null, 0 );
		
		Thread.sleep( 250 );
		
		String clientUri1 = "etch://127.0.0.1:8002/invalidSourceUri";
		Entry e = NSLib.staticLookup( clientUri1 );
		
		assertEquals( null, e );
	}
	
	@Test( expected = IllegalArgumentException.class )
	public void unregister1() throws Exception
	{
		NSLib.staticUnregister( null, null, false );
	}
	
	@Test( expected = IllegalArgumentException.class )
	public void unregister2() throws Exception
	{
		Session session = new MySession();
		String listenerUri1 = "suri=listenerUri&curi=targetListener&nsuri=tcp://127.0.0.1:8002";
		
		NSLib.staticUnregister( session, listenerUri1, false );
	}

	@Test
	public void unregister3() throws Exception
	{
		Session session = new MySession();
		NSLib.staticRegister( session, listenerUri, null, 0 );
		
		Thread.sleep( 250 );
		
		// try unregistering an invalid entry
		String listenerUri1 = "etch:sourceListener_invalid?suri=listenerUri&curi=targetListener&nsuri=tcp://127.0.0.1:8002";
		NSLib.staticUnregister( session, listenerUri1, true );

		// above unregister doesn't affect existing entry in NS table
		
		Entry e = NSLib.staticLookup( clientUri );
		
		assertEquals( testSourceUri1, e.sourceUri );
		assertEquals( testtargetUri1, e.targetUri );
		assertEquals( 0, e.ttl );
		assertEquals( null, e.qualities );
	}
	
	@Test
	public void nsDown1() throws Exception
	{
		/**
		 * test that the NSLib alarm functionality works fine
		 * - test that register(...) is being called every RECONNECT_DELAY units
		 */
		Session session = new MySession();
		NSLib.staticRegister( session, listenerUri, null, 0 );
		
		// stop the naming service
		nsListener.transportControl( Transport.STOP_AND_WAIT_DOWN, 4000 );
		
		Thread.sleep( 1000 );	//breather
		
		// start the naming service 
		nsListener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		
		Thread.sleep( NSLib.RECONNECT_DELAY );	// worst case reconnect would take this much time
		
		Entry e = NSLib.staticLookup( clientUri );
		
		assertEquals( testSourceUri1, e.sourceUri );
		assertEquals( testtargetUri1, e.targetUri );
		assertEquals( 0, e.ttl );
		assertEquals( null, e.qualities );
		
	}
	
	@AfterClass
	public static void endNS()
	{
		try
		{
			nsListener.transportControl( Transport.STOP_AND_WAIT_DOWN, 4000 );
		}
		catch( Exception e )
		{
			e.printStackTrace();
		}
	}
	
	/**
	 * Mock session 
	 *
	 */
	private class MySession implements Session
	{
		public void sessionControl( Object arg0, Object arg1 ) throws Exception
		{
			System.out.println( "session control" );
		}

		public void sessionNotify( Object arg0 ) throws Exception
		{	
			System.out.println( "session notify: " + arg0 );
		}

		public Object sessionQuery( Object arg0 ) throws Exception
		{
			System.out.println( "sessionQuery: " + arg0 );
			return null;
		}
		
	}
}

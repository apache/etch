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

package org.apache.etch.util.core.nio;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.TreeMap;

import org.apache.etch.util.Assertion;


/**
 * Models the external api of a Selector while keeping a list of available
 * selectors and their load.
 */
public class SuperSelector
{
	/**
	 * Marks the SuperSelector as open for business.
	 */
	public void start()
	{
		checkStarted( false );
		started = true;
	}
	
	private void checkStarted( boolean expected )
	{
		if (expected != started)
			throw new IllegalStateException( expected ? "not started" : "started" );
	}

	private boolean started;
	
	/**
	 * Shuts down the SuperSelector, closing all the existing selectors and
	 * handlers.
	 */
	public void shutdown()
	{
		started = false;
	}
	
	/**
	 * Adds the Selector to the set of available selectors, it is new created.
	 * @param selector
	 * @param avail the number of available slots.
	 */
	public void add( Selector selector, int avail )
	{
		checkStarted( true );
		Key k = new Key( avail, selector );
		if (!sel2keys.containsKey( selector ))
		{
			sel2keys.put( selector, k );
			keysAdd( k );
		}
	}

	private final Map<Selector, Key> sel2keys =
		Collections.synchronizedMap( new HashMap<Selector, Key>() );

	/**
	 * Updates the Selector's capacity in the set of available selectors.
	 * @param selector
	 * @param avail the number of available slots.
	 */
	public void update( Selector selector, int avail )
	{
		checkStarted( true );
		
		Key k = sel2keys.get( selector );
		Assertion.check( k != null, "k != null" );
		
		synchronized (_keys)
		{
			if (avail != k.avail())
			{
				keysRemove( k );
				k.setAvail( avail );
				keysAdd( k );
			}
		}
	}

	/**
	 * Removes the Selector from the set of available selectors. It is dead.
	 * @param selector 
	 */
	public void remove( Selector selector )
	{
		Key k = sel2keys.remove( selector );
		if (k != null)
			keysRemove( k );
	}
	
	private void keysAdd( Key k )
	{
		synchronized (_keys)
		{
			if (k.avail() > 0)
			{
				Key o = _keys.put( k, k );
//				System.out.printf( "add %s => %s\n", k, o );
				Assertion.check( o == null, "o == null" );
			}
			else
			{
				Assertion.check( _keys.get( k ) == null, "_keys.get( k ) == null" );
			}
		}
	}

	private void keysRemove( Key k )
	{
		synchronized (_keys)
		{
			Key o = _keys.remove( k );
//			System.out.printf( "remove %s => %s\n", k, o );
			Assertion.check( o == k || o == null, "o == k || o == null" );
		}
	}
	
	private Selector getSelector() throws IOException
	{
		synchronized (_keys)
		{
			try
			{
				Key k = _keys.firstKey();
				keysRemove( k );
				k.setAvail( k.avail()-1 );
				keysAdd( k );
				return k.selector;
			}
			catch ( NoSuchElementException e )
			{
				Selector s = new Selector( _id++, this );
				// constructor calls us back at #add(Selector, int)
				s.start();
				return s;
			}
		}
	}
	
	private final TreeMap<Key, Key> _keys = new TreeMap<Key, Key>();
	
	private int _id;

	/**
	 * Registers the handler with an available Selector. This may be called by
	 * an external thread or by one of the Selector threads.
	 * @param handler
	 * @throws IOException 
	 */
	public void register( Handler<?> handler ) throws IOException
	{
		checkStarted( true );
		getSelector().register( handler );
		
	}

	/**
	 * @param addr
	 * @param backlog
	 * @param factory
	 * @return a new AcceptHandler already registered with this selector.
	 * @throws Exception
	 */
	public AcceptHandler newAcceptHandler( InetSocketAddress addr, int backlog,
		AcceptHandlerFactory factory ) throws Exception
	{
		ServerSocketChannel ssc = ServerSocketChannel.open();
		ssc.configureBlocking( false );
		ServerSocket ss = ssc.socket();
		ss.bind( addr, backlog );
		AcceptHandler ah = factory.newAcceptHandler( ssc );
		register( ah );
		return ah;
	}
	
	/**
	 * @param addr
	 * @param factory
	 * @return a new StreamHandler already registered with this selector.
	 * @throws Exception
	 */
	public StreamHandler newStreamHandler( InetSocketAddress addr,
		StreamHandlerFactory factory ) throws Exception
	{
		SocketChannel sc = SocketChannel.open();
		sc.configureBlocking( false );
		StreamHandler sh = factory.newStreamHandler( sc, true );
		register( sh );
		sc.connect( addr );
		return sh;
	}

	/**
	 * @param sc
	 * @param factory
	 * @return a new StreamHandler already registered with this selector.
	 * @throws Exception
	 */
	public StreamHandler newStreamHandler( SocketChannel sc,
		StreamHandlerFactory factory ) throws Exception
	{
		sc.configureBlocking( false );
		StreamHandler sh = factory.newStreamHandler( sc, false );
		register( sh );
		return sh;
	}
	
	private static class Key implements Comparable<Key>
	{
		public Key( int avail, Selector selector )
		{
			_avail = avail;
			this.selector = selector;
		}

		private int _avail;
		
		private final Selector selector;
		
		@Override
		public String toString()
		{
			return String.format( "Key(%d, %s)", _avail, selector );
		}
		
		public int avail()
		{
			return _avail;
		}

		public void setAvail( int avail )
		{
			_avail = avail;
		}
		
		@Override
		public int hashCode()
		{
			return _avail ^ selector.hashCode();
		}
		
		@Override
		public boolean equals( Object obj )
		{
			if (obj == this)
				return true;
			
			if (obj == null || obj.getClass() != Key.class)
				return false;
			
			Key k = (Key) obj;
			return _avail == k._avail && selector == k.selector;
		}

		public int compareTo( Key k )
		{
			if (_avail < k._avail)
				return -1;
			
			if (_avail > k._avail)
				return 1;
			
			// avail == k.avail
			
			if (selector.id() < k.selector.id())
				return -1;
			
			if (selector.id() > k.selector.id())
				return 1;
			
			return 0;
		}
	}

	/**
	 * 
	 */
	public void dump()
	{
		synchronized (_keys)
		{
			for (Key k: _keys.values())
				System.out.printf( "key: %d %s\n", k.avail(), k.selector );
			
			for (Key k: _keys.values())
				k.selector.dump();
		}
	}

	/**
	 * @return the statically configured Selector, or one newly configured if
	 * none.
	 */
	public static SuperSelector get()
	{
		if (selector == null)
		{
			synchronized (sync)
			{
				if (selector == null)
				{
					selector = new SuperSelector();
					selector.start();
				}
			}
		}
		return selector;
	}
	
	/**
	 * Replaces the current statically configured selector with a new one.
	 * @param newSelector
	 * @return the old statically configured selected.
	 */
	public static SuperSelector put( SuperSelector newSelector )
	{
		synchronized (sync)
		{
			SuperSelector oldSelector = selector;
			selector = newSelector;
			return oldSelector;
		}
	}
	
	private static SuperSelector selector;
	
	private final static Object sync = new Object();
}

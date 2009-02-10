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
import java.nio.channels.CancelledKeyException;
import java.nio.channels.ClosedChannelException;
import java.nio.channels.ClosedSelectorException;
import java.nio.channels.SelectionKey;
import java.util.LinkedList;

import org.apache.etch.util.Assertion;


/**
 * An experiment in selection.
 */
public class Selector extends Thread
{
	/**
	 * Maximum number of keys allowed in this Selector.
	 */
	public static final int MAX_KEYS = 64;

	/**
	 * Constructs the selector.
	 * @param id a unique id for this selector.
	 * @param ss the containing super selector.
	 * @throws IOException 
	 */
	public Selector( int id, SuperSelector ss ) throws IOException
	{
		this.id = id;
		this.ss = ss;
		selector = java.nio.channels.Selector.open();
		ssAdd();
	}
	
	private final int id;
	
	private final SuperSelector ss;

	private final java.nio.channels.Selector selector;

	/**
	 * @return the unique id of this selector.
	 */
	public int id()
	{
		return id;
	}
	
	@Override
	public String toString()
	{
		return "Selector."+id;
	}

	////////////
	// THREAD //
	////////////

	@Override
	public void run()
	{
		try
		{
			while (selector.isOpen())
			{
				runActions();
				
				int n;
				while ( true )
				{
					try
					{
						n = selector.select();
						break;
					}
					catch( IOException e )
					{
						// Workaround, see sun bug 6693490
						//System.out.println("****** caught IOException");
						if ( e.getMessage() != null && e.getMessage().indexOf("File exists") != -1 )
							continue;
						throw e;
					}
				}
//				Log.report( "selected", "selector", this, "n", n );
				if (n > 0)
				{
					for (SelectionKey key : selector.selectedKeys())
						handleSelection( key );
					selector.selectedKeys().clear();
				}
			}
		}
		catch ( ClosedSelectorException e )
		{
			// ignore.
		}
		catch ( Exception e )
		{
			reportException( e );
		}
		finally
		{
			ss.remove( this );
			try
			{
				// TODO remove and cancel all the keys
				selector.close();
			}
			catch ( Exception e )
			{
				reportException( e );
			}
		}
	}
	
	private void reportException( Exception e )
	{
		System.err.printf( "%s: caught exception: %s\n", this, e );
		e.printStackTrace();
	}

	//////////////////
	// SAFE ACTIONS //
	//////////////////

	/**
	 * Schedules a registration of the handler with this Selector if it
	 * isn't full, creates a new Selector if this Selector is full.
	 * @param handler
	 */
	public void register( final Handler<?> handler )
	{
		checkNotShutdown();
		
		enqueueAction( false, new Action()
		{
			public void run()
			{
				try
				{
					actionRegister( handler );
				}
				catch ( Exception e )
				{
					try
					{
						actionCancel( handler, e );
					}
					catch ( Exception e1 )
					{
						handler.reportException( e1 );
					}
				}
			}
		} );
	}

	private void checkNotShutdown()
	{
		if (shutdown)
			throw new IllegalStateException( "shutdown" );
	}

	/**
	 * Schedules an update to the handler's interest ops with this Selector.
	 * @param handler
	 */
	public void updateInterestOps( final Handler<?> handler )
	{
		enqueueAction( false, new Action()
		{
			public void run()
			{
				try
				{
					actionUpdateInterestOps( handler );
				}
				catch ( Exception e )
				{
					try
					{
						actionCancel( handler, e );
					}
					catch ( Exception e1 )
					{
						handler.reportException( e1 );
					}
				}
			}
		} );
	}

	/**
	 * Schedules a cancel of the handler with this Selector.
	 * @param handler
	 */
	public void cancel( final Handler<?> handler )
	{
		enqueueAction( true, new Action()
		{
			public void run()
			{
				try
				{
					actionCancel( handler, null );
				}
				catch ( Exception e )
				{
					handler.reportException( e );
				}
			}
		} );
	}

	/**
	 * Shuts down the selector.
	 */
	public void shutdown()
	{
		if (shutdown)
			return;
		
		shutdown = true;
		
		enqueueAction( false, new Action()
		{
			public void run()
			{
				try
				{
					actionShutdown();
				}
				catch ( Exception e )
				{
					reportException( e );
				}
			}
		} );
	}
	
	private boolean shutdown;

	////////////////////
	// UNSAFE ACTIONS //
	////////////////////

	private void actionRegister( Handler<?> handler ) throws Exception
	{
		// this action is called from the selector thread.
		
		int n = selector.keys().size();
		if (n >= MAX_KEYS)
		{
			ss.register( handler );
			return;
		}

		int ops = handler.getInterestOps();
		
		Assertion.check( !handler.channel().isRegistered(), "!handler.channel().isRegistered()" );
		Assertion.check( handler.key() == null, "handler.key() == null" );
		
		SelectionKey key = handler.channel().register( selector, ops, handler );
		handler.setLastInterestOps( ops );
		handler.registered( this, key );
	}

	private void actionUpdateInterestOps( Handler<?> handler ) throws Exception
	{
		// this action is called from the selector thread.
		
		SelectionKey key = handler.key();
		if (key != null)
		{
			int ops = handler.getInterestOps();
			if (ops != handler.getLastInterestOps())
			{
				key.interestOps( ops );
				handler.setLastInterestOps( ops );
			}
		}
	}

	private void actionCancel( Handler<?> handler, Exception e ) throws Exception
	{
		// this action is called from the selector thread.
		
		handler.canceled( e );
	}
	
	private void actionShutdown() throws Exception
	{
		selector.close();
	}

	/////////////////
	// ACTION LIST //
	/////////////////

	private void runActions()
	{
		Action action;
		while ((action = dequeueAction()) != null)
			action.run();
		ssUpdate();
	}
	
	private void ssAdd()
	{
		size = selector.keys().size();
		ss.add( this, MAX_KEYS-size );
	}

	private void ssUpdate()
	{
		size = selector.keys().size();
		ss.update( this, MAX_KEYS-size );
	}
	
	private int size;

	private void enqueueAction( boolean priority, Action action )
	{
		synchronized (actions)
		{
			boolean wasEmpty = actions.isEmpty();
			
			if (priority)
				actions.addFirst( action );
			else
				actions.addLast( action );
			
			if (wasEmpty)
				selector.wakeup();
		}
	}

	private Action dequeueAction()
	{
		synchronized (actions)
		{
			return actions.poll();
		}
	}

	private final LinkedList<Action> actions = new LinkedList<Action>();
	
	//////////////////////
	// HANDLE SELECTION //
	//////////////////////
	
	private void handleSelection( SelectionKey key )
	{
		Handler<?> handler = (Handler<?>) key.attachment();
		
		if (handler == null)
		{
			try
			{
				key.channel().close();
			}
			catch ( IOException e )
			{
				reportException( e );
			}
			return;
		}

		try
		{
			handler.selected();
			actionUpdateInterestOps( handler );
		}
		catch ( ClosedChannelException e )
		{
			// ignore
		}
		catch ( CancelledKeyException e )
		{
			// ignore
		}
		catch ( Exception e )
		{
			try
			{
				actionCancel( handler, e );
			}
			catch ( Exception e1 )
			{
				handler.reportException( e1 );
			}
		}
	}

	/**
	 * Performs an action on a handler, such as registering it with the
	 * selector, updating its interest ops, or closing it.
	 */
	public interface Action
	{
		/**
		 * Runs the action when the selector is available to control.
		 */
		public void run();
	}

	/**
	 * Dumps the handlers in this selector.
	 */
	public void dump()
	{
		for (SelectionKey k: selector.keys())
		{
			Object a = k.attachment();
			if (a == null)
			{
				System.out.printf( "%s: channel %s\n", this, k.channel() );
				continue;
			}
			System.out.printf( "%s: attachment %s\n", this, a );
		}
	}
}

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
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import org.apache.etch.util.AlarmListener;
import org.apache.etch.util.AlarmManager;
import org.apache.etch.util.Todo;
import org.apache.etch.util.TodoManager;


/**
 * A shared pool of buffers.
 */
public class ByteBufferPool implements AlarmListener
{
	/**
	 * Constructs a ByteBufferPool.
	 * @param bufferSize the size of each individual buffer.
	 * @param min the minimum number of buffers to keep in the pool.
	 * @param limit the maximum number of buffers to allow.
	 * @param interval the number of milliseconds between history tickle.
	 * @param length the length of history to maintain.
	 */
	public ByteBufferPool( int bufferSize, int min, int limit,
		int interval, int length )
	{
		if (bufferSize <= 0)
			throw new IllegalArgumentException( "bufferSize <= 0" );
		
		if (interval < 0)
			throw new IllegalArgumentException( "interval < 0" );
		
		if (interval == 0 || length == 0)
		{
			// if either is 0, force both to 0.
			interval = 0;
			length = 0;
		}
		
		this.bufferSize = bufferSize;
		this.interval = interval;
		
		history = new History( min, limit, length );
		
		if (interval > 0)
			AlarmManager.staticAdd( this, null, interval );
	}

	/** The size of buffers to allocate. */
	private final int bufferSize;
	
	/** Time in milliseconds between history tickles. */
	private final int interval;

	private final History history;

	/**
	 * @return the size of the buffers being managed.
	 */
	public int bufferSize()
	{
		return bufferSize;
	}

	/**
	 * @return the minimum number of buffers to maintain.
	 */
	public int min()
	{
		return history.min();
	}

	/**
	 * @return the maximum number of buffers to allocate.
	 */
	public int limit()
	{
		return history.limit();
	}

	/**
	 * @return the history tickle interval in ms.
	 */
	public int interval()
	{
		return interval;
	}

	/**
	 * @return the history tickle length.
	 */
	public int length()
	{
		return history.length();
	}

	/**
	 * Allocates a buffer from the pool.
	 * 
	 * @param notify object to be notified when a buffer is released into the
	 * pool.
	 * @return a buffer from the pool or null if the pool is empty and notify is
	 * not null.
	 * @throws IOException the pool is empty and notify is null.
	 */
	public ByteBuffer alloc( Notify notify ) throws IOException
	{
		synchronized (saved)
		{
			if (!history.used( 1 ))
			{
				if (notify == null)
					throw new IOException( "out of buffers" );

				// notify != null
				register( notify );
				return null;
			}

			try
			{
				return allocBuf();
			}
			catch ( Error e )
			{
				history.used( -1 );
				throw e;
			}
			catch ( RuntimeException e )
			{
				history.used( -1 );
				throw e;
			}
		}
	}

	/**
	 * Allocates some buffers from the pool.
	 * 
	 * @param notify object to be notified when a buffer is released into the
	 * pool.
	 * @param count the number of buffers required.
	 * @return an array of allocated buffers.
	 * @throws IOException
	 */
	public ByteBuffer[] alloc( Notify notify, int count ) throws IOException
	{
		if (count <= 0)
			throw new IllegalArgumentException( "count <= 0" );

		if (count > history.limit())
			throw new IllegalArgumentException( "count > limit" );

		synchronized (saved)
		{
			if (!history.used( count ))
			{
				if (notify == null)
					throw new IOException( "out of buffers" );

				// notify != null
				register( notify );
				return null;
			}

			ByteBuffer[] bufs = new ByteBuffer[count];
			
			try
			{
				for (int i = 0; i < count; i++)
					bufs[i] = allocBuf();
			}
			catch ( Error e )
			{
				release( bufs );
				throw e;
			}
			catch ( RuntimeException e )
			{
				release( bufs );
				throw e;
			}
			
			return bufs;
		}
	}

	/**
	 * Releases a buffer back into the pool. If there are waiters for a buffer
	 * being released into the pool, they are notified.
	 * 
	 * @param buf
	 */
	public void release( ByteBuffer buf )
	{
		synchronized (saved)
		{
			releaseBuf( buf );
			notifyWaiters();
		}
	}

	/**
	 * Releases some buffers back into the pool. If there are waiters for a
	 * buffer being released into the pool, they are notified.
	 * 
	 * @param bufs
	 */
	public void release( ByteBuffer[] bufs )
	{
		if (bufs == null || bufs.length == 0)
			return;

		synchronized (saved)
		{
			for (ByteBuffer buf : bufs)
				releaseBuf( buf );

			notifyWaiters();
		}
	}

	private ByteBuffer allocBuf()
	{
		// used has already been incremented.
		
		// we need a buffer, get a saved one if any...

		if (!saved.isEmpty())
		{
			ByteBuffer buf = saved.remove( saved.size() - 1 );
			buf.clear();
			return buf;
		}

		ByteBuffer buf = ByteBuffer.allocateDirect( bufferSize );
		history.alloc( 1 );
		return buf;
	}

	private void releaseBuf( ByteBuffer buf )
	{
		if (buf == null)
			return;
		
		history.used( -1 );

		// save the buffer if there aren't already too many, drop the rest.
		
		if (saved.size() < history.suggested())
			saved.add( buf );
		else
			history.alloc( -1 );
	}
	
	private void trimSaved()
	{
		synchronized (saved)
		{
			int n = history.suggested();
			while (saved.size() > n)
			{
				saved.remove( saved.size()-1 );
				history.alloc( -1 );
			}
		}
	}

	private final List<ByteBuffer> saved = new ArrayList<ByteBuffer>();

	/**
	 * Registers for notification of available buffer. Note that when you are
	 * notified, your registration is canceled. You can always re-register in
	 * the bufferAvailable method.
	 * 
	 * @param notify
	 */
	public void register( Notify notify )
	{
		synchronized (waiters)
		{
			waiters.add( notify );
		}
	}

	/**
	 * Removes a registration for notification of available buffer.
	 * 
	 * @param notify
	 */
	public void unregister( Notify notify )
	{
		synchronized (waiters)
		{
			waiters.remove( notify );
		}
	}

	private void notifyWaiters()
	{
		final Notify[] notifies;

		synchronized (waiters)
		{
			int n = waiters.size();
			if (n == 0)
				return;

			notifies = waiters.toArray( new Notify[n] );
			waiters.clear();
		}

		TodoManager.addTodo( new Todo()
		{
			public void doit( TodoManager mgr ) throws Exception
			{
				for (Notify n : notifies)
					n.bufferAvailable( ByteBufferPool.this );
			}

			public void exception( TodoManager mgr, Exception e )
			{
				e.printStackTrace();
			}
		} );
	}

	private final List<Notify> waiters = new LinkedList<Notify>();

	public int wakeup( AlarmManager manager, Object state, long due )
	{
		history.tickle();
		trimSaved();
		return interval;
	}
	
	/**
	 * Called when this ByteBufferPool is no longer active. Removes the history
	 * tickle.
	 */
	public void shutdown()
	{
		AlarmManager.staticRemove( this );
	}

	/**
	 * Interface to notify interested parties about available buffers.
	 */
	public interface Notify
	{
		/**
		 * Notifies that the pool has an available buffer.
		 * 
		 * @param pool
		 */
		public void bufferAvailable( ByteBufferPool pool );
	}

	/**
	 * @return the count of buffers currently allocated.
	 */
	public int used()
	{
		return history.getUsed();
	}
}

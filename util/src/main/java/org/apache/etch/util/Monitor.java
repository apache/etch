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

package org.apache.etch.util;


/**
 * A class which we can use to monitor conditions.
 * 
 * @param <T> The type of object that may be stored in the monitor.
 */
public class Monitor<T>
{
	/**
	 * Constructs the Monitor.
	 * 
	 * @param description a description of this monitor.
	 * 
	 * @param initialValue the initial value of this monitor.
	 */
	public Monitor( String description, T initialValue )
	{
		this.description = description;
		this.value = initialValue;
	}
	
	/**
	 * Constructs the Monitor with null initial value.
	 * 
	 * @param description a description of this monitor.
	 */
	public Monitor( String description )
	{
		this( description, null );
	}
	
	/**
	 * @return the description of this monitor.
	 */
	public String getDescription()
	{
		return description;
	}
	
	private final String description;
	
	private T value;

	@Override
	public String toString()
	{
		return "Monitor "+description+": "+value;
	}
	
	/**
	 * @return the current value of the monitor.
	 */
	public T get()
	{
		return value;
	}
	
	/**
	 * Sets the current value.
	 * 
	 * @param newValue the value to be set.
	 * 
	 * @return the old value.
	 */
	public synchronized T set( T newValue )
	{
		T oldValue = value;
		value = newValue;
		notifyAll();
		return oldValue;
	}

	/**
	 * Waits until value equals the desired value and
	 * then sets the value. Will wait forever.
	 * 
	 * @param desiredValue the value we want.
	 * 
	 * @param newValue the value to be set.
	 * 
	 * @return the old value
	 * 
	 * @throws InterruptedException if waiting was interrupted.
	 * 
	 * @throws TimeoutException if we waited too long.
	 */
	public T waitUntilEqAndSet( T desiredValue, T newValue )
		throws InterruptedException, TimeoutException
	{
		return waitUntilEqAndSet( desiredValue, 0, newValue );
	}

	/**
	 * Waits until value equals the desired value and
	 * then sets the value.
	 * 
	 * @param desiredValue the value we want.
	 * 
	 * @param maxDelay the max amount of time in ms to wait.
	 * If 0 is specified, we will wait forever.
	 * 
	 * @param newValue the value to be set.
	 * 
	 * @return the old value
	 * 
	 * @throws InterruptedException if waiting was interrupted.
	 * 
	 * @throws TimeoutException if we waited too long.
	 */
	public synchronized T waitUntilEqAndSet( T desiredValue, int maxDelay,
		T newValue ) throws InterruptedException, TimeoutException
	{
		waitUntilEq( desiredValue, maxDelay );
		return set( newValue );
	}

	/**
	 * Waits until value equals the desired value. Will wait forever.
	 * 
	 * @param desiredValue the value we want.
	 * 
	 * @throws InterruptedException if waiting was interrupted.
	 * 
	 * @throws TimeoutException if we waited too long.
	 */
	public void waitUntilEq( T desiredValue )
		throws InterruptedException, TimeoutException
	{
		waitUntilEq( desiredValue, 0 );
	}

	/**
	 * Waits until value equals the desired value.
	 * 
	 * @param desiredValue the value we want.
	 * 
	 * @param maxDelay the max amount of time in ms to wait.
	 * If 0 is specified, we will wait forever.
	 * 
	 * @throws InterruptedException if waiting was interrupted.
	 * 
	 * @throws TimeoutException if we waited too long.
	 */
	public synchronized void waitUntilEq( T desiredValue, int maxDelay )
		throws InterruptedException, TimeoutException
	{
		checkDelay( maxDelay );
		
		long now = Timer.getNanos();
		long end = endTime( now, maxDelay );
		
		long d;
		while (!eq( value, desiredValue ) && (d = remTime( end, now )) > 0)
		{
			wait( d );
			now = Timer.getNanos();
		}
		
		if (!eq( value, desiredValue ))
			throw new TimeoutException( "timeout" );
	}

	/**
	 * Waits until value does not equal the undesired value and then
	 * sets the value. Will wait forever.
	 * 
	 * @param undesiredValue the value we do not want.
	 * 
	 * @param newValue the value to be set.
	 * 
	 * @return the old value
	 * 
	 * @throws InterruptedException if waiting was interrupted.
	 * 
	 * @throws TimeoutException if we waited too long.
	 */
	public T waitUntilNotEqAndSet( T undesiredValue, T newValue )
		throws InterruptedException, TimeoutException
	{
		return waitUntilNotEqAndSet( undesiredValue, 0, newValue );
	}

	/**
	 * Waits until value does not equal the undesired value and then
	 * sets the value.
	 * 
	 * @param undesiredValue the value we do not want.
	 * 
	 * @param maxDelay the max amount of time in ms to wait.
	 * If 0 is specified, we will wait forever.
	 * 
	 * @param newValue the value to be set.
	 * 
	 * @return the old value
	 * 
	 * @throws InterruptedException if waiting was interrupted.
	 * 
	 * @throws TimeoutException if we waited too long.
	 */
	public synchronized T waitUntilNotEqAndSet( T undesiredValue, int maxDelay,
		T newValue ) throws InterruptedException, TimeoutException
	{
		waitUntilNotEq( undesiredValue, maxDelay );
		return set( newValue );
	}
	
	/**
	 * Waits until value does not equal the undesired value. Will
	 * wait forever.
	 * 
	 * @param undesiredValue the value we do not want.
	 * 
	 * @return the current value.
	 * 
	 * @throws InterruptedException if waiting was interrupted.
	 * 
	 * @throws TimeoutException if we waited too long.
	 */
	public T waitUntilNotEq( T undesiredValue )
		throws InterruptedException, TimeoutException
	{
		return waitUntilNotEq( undesiredValue, 0 );
	}
	
	/**
	 * Waits until value does not equal the undesired value.
	 * 
	 * @param undesiredValue the value we do not want.
	 * 
	 * @param maxDelay the max amount of time in ms to wait.
	 * If 0 is specified, we will wait forever.
	 * 
	 * @return the current value.
	 * 
	 * @throws InterruptedException if waiting was interrupted.
	 * 
	 * @throws TimeoutException if we waited too long.
	 */
	public synchronized T waitUntilNotEq( T undesiredValue, int maxDelay )
		throws InterruptedException, TimeoutException
	{
		checkDelay( maxDelay );
		
		long now = Timer.getNanos();
		long end = endTime( now, maxDelay );
		
		long d;
		while (eq( value, undesiredValue ) && (d = remTime( end, now )) > 0)
		{
			wait( d );
			now = Timer.getNanos();
		}
		
		if (eq( value, undesiredValue ))
			throw new TimeoutException( "timeout" );
		
		return value;
	}

	/////////////////////
	// PRIVATE METHODS //
	/////////////////////
	
	private void checkDelay( int maxDelay )
	{
		if (maxDelay < 0)
			throw new IllegalArgumentException( "maxDelay < 0" );
	}

	/**
	 * Calculates the ending time for waiting.
	 * 
	 * @param now current nanotime.
	 * 
	 * @param maxDelay the max amount of time in ms to wait.
	 * If 0 is specified, we will wait forever.
	 * 
	 * @return the nanotime for ending.
	 */
	private long endTime( long now, int maxDelay )
	{
		return maxDelay > 0 ? now + maxDelay * Timer.NANOS_PER_MILLI : Long.MAX_VALUE;
	}
	
	/**
	 * Calculates the number of milliseconds from now to end.
	 * 
	 * @param end nanotime for ending.
	 * 
	 * @param now current nanotime
	 * 
	 * @return the number of milliseconds from now to end.
	 */
	private long remTime( long end, long now )
	{
		return (end - now) / Timer.NANOS_PER_MILLI;
	}

	/**
	 * Compares the specified values.
     * 
	 * @param v1 a value to compare, which may be null.
	 * 
	 * @param v2 another value to compare, which may be null.
	 * 
	 * @return true if the values are equal, false otherwise. If both
	 * values are null, they are considered equal.
	 */
	private boolean eq( T v1, T v2 )
	{
		if (v1 != null && v2 != null)
			return v1.equals( v2 );
		
		return v1 == v2;
	}
}

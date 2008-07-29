/* $Id$
 *
 * Created by wert on Mar 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.util;

/**
 * Generic implementation of Startable.
 */
abstract public class AbstractStartable implements Startable
{
	final public synchronized void start() throws Exception
	{
		if (isStarted())
			throw new IllegalStateException( "is already started" );
		
		try
		{
			started = true;
			start0();
		}
		catch ( Exception e )
		{
			setStopped();
			stop0();
			throw e;
		}
	}

	final public synchronized void stop() throws Exception
	{
		checkIsStarted();
		setStopped();
		stop0();
	}
	
	/**
	 * Signals (from a subclasser) that the startable has stopped.
	 */
	final protected void setStopped()
	{
		started = false;
	}
	
	final public boolean isStarted()
	{
		return started;
	}
	
	final public void checkIsStarted()
	{
		if (!isStarted())
			throw new IllegalStateException( "is not started" );
	}

	private boolean started;
	
	/**
	 * @throws Exception
	 */
	abstract protected void start0() throws Exception;
	
	/**
	 * @throws Exception
	 */
	abstract protected void stop0() throws Exception;
}

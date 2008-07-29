/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
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

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
 * Generic implementation of Startable.
 */
abstract public class AbstractStartable implements Startable
{
	final public void start() throws Exception
	{
		synchronized (this)
		{
			if (isStarted())
				throw new IllegalStateException( "is already started" );

			started = true;
		}
		
		try
		{
			start0();
		}
		catch ( Exception e )
		{
			stop();
			throw e;
		}
	}

	final public void stop() throws Exception
	{
		synchronized (this)
		{
			checkIsStarted();
			setStopped();
		}
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

/* $Id: Startable.java 21882 2006-04-11 18:26:09Z builder $
 *
 * Created by wert on Feb 10, 2006.
 * 
 * Copyright (c) 2007 Metreos, Inc. All rights reserved.
 */

package etch.util;

/**
 * Description of Startable
 */
public interface Startable
{
	/**
	 * @throws Exception
	 */
	public void start() throws Exception;
	
	/**
	 * @throws Exception 
	 */
	public void stop() throws Exception;
	
	/**
	 * @return true if started.
	 */
	public boolean isStarted();

	/**
	 * Checks that the startable is started.
	 * @throws IllegalStateException if not started.
	 */
	public void checkIsStarted();
}

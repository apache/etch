/* $Id$
 *
 * Created by wert on Mar 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.util;


/**
 * Implementation of startable which creates a thread and runs.
 */
abstract public class Runner
	extends AbstractStartable implements Runnable
{
	/**
	 * Constructs the Runner.
	 */
	public Runner()
	{
		// nothing to do.
	}
	
	/**
	 * @param handler the handler to notify of started and stopped events.
	 */
	public void setHandler( RunnerHandler handler )
	{
		this.handler = handler;
	}
	
	/**
	 * The handler is used to report started and stopped events.
	 */
	private RunnerHandler handler;
	
	@Override
	protected void start0() throws Exception
	{
		thread = new Thread( this );
		thread.start();
	}

	@Override
	protected void stop0() throws Exception
	{
		Thread t = thread;
		if (t != null)
		{
			thread = null;
			//t.join();
		}
	}
	
	private Thread thread;
	
	final public void run()
	{
		fireStarted();
		
		try
		{
			boolean first = true;
			while (isStarted())
			{
				if (!run0( first ))
					break;
				first = false;
			}
		}
		catch ( Exception e )
		{
			fireException( "run", e );
		}
		finally
		{
			setStopped();
			fireStopped();
			//Log.report( "thread dead" );
		}
	}
	
	private void fireStarted()
	{
		handler.started();
	}

	/**
	 * Reports an exception caught by the thread managing the runnable.
	 * @param what
	 * @param e
	 */
	protected void fireException( String what, Exception e )
	{
		// e.printStackTrace();
		try
		{
			handler.exception( what, e );
		}
		catch ( Exception x )
		{
			x.printStackTrace();
		}
	}

	private void fireStopped()
	{
		handler.stopped();
	}

	/**
	 * @param first true the first time we are called
	 * @return true if we should keep running.
	 * @throws Exception
	 */
	abstract protected boolean run0( boolean first ) throws Exception;
}

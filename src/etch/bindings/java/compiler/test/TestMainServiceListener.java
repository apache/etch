/* $Id$
 *
 * Created by rebarraz on Oct 3, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */
package etch.bindings.java.compiler.test;

import static org.junit.Assert.fail;
import org.junit.Test;


/**
 * Tests for proper generation of a Main[Service]Listeners.
 */
public class TestMainServiceListener
{
	// Tests
	//--------------------------------------------------------------------------

	/**
	 * Run the Listener.
	 */
	@Test
	public void mainMethod()
	{
		ListenerThread m = new ListenerThread();
		m.run();

		try
		{
			m.join(1000);
		}
		catch (Exception e)
		{
			fail();
		}
	}

	/**
	 * Call newTestServer.
	 */
	@Test
	public void newTestServerMethod()
	{
		MainTestListener m = new MainTestListener();
		m.newTestServer(null);
	}

	// Helpers
	//--------------------------------------------------------------------------

	/**
	 * Simple thread class for calling main and having all of Thread's methods.
	 */
	private static class ListenerThread extends Thread
	{
		/**
		 * Go at it.
		 */
		public void run()
		{
			try
			{
				MainTestListener.main(null);
			}
			catch (Exception e)
			{
			}
		}
	}
}

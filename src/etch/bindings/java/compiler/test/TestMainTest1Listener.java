/* $Id$
 *
 * Created by rebarraz on Oct 3, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */
package etch.bindings.java.compiler.test;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import org.junit.Test;

/**
 * Tests for proper generation of a Main[Service]Listeners.
 */
public class TestMainTest1Listener
{
	// Tests
	//--------------------------------------------------------------------------

//	/**
//	 * Run the Listener.
//	 */
/*	@Test
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
		}

		if (m.hasErrors())
			fail();
	}
	*/

	/**
	 * Call newTestServer.
	 */
	@Test
	public void newTestServerMethod()
	{
		MainTest1Listener m = new MainTest1Listener();
		Object o = m.newTest1Server(null);
		assertNotNull( o );
		assertTrue( o instanceof ImplTest1Server );
		assertTrue( o instanceof BaseTest1Server );
	}

	// Helpers
	//--------------------------------------------------------------------------

//	/**
//	 * Prints exception info to stderr in a most descriptive way.
//	 * @param e the exception to print
//	 */
//	private static void printException(final Exception e)
//	{
//		e.printStackTrace(System.err);
//	}

//	/**
//	 * Simple thread class for calling main and having all of Thread's methods.
//	 */
//	private static class ListenerThread extends Thread
//	{
//		/**
//		 * false until an unexpected exception occurs.
//		 */
//		private boolean myHasErrors;
//
//		/**
//		 * Start off with no errors.
//		 */
//		public ListenerThread()
//		{
//			myHasErrors = false;
//		}
//
//		/**
//		 * Go at it.
//		 */
//		public void run()
//		{
//			try
//			{
//				MainTestListener.main(null);
//			}
//			catch (Exception e)
//			{
//				printException(e);
//				myHasErrors = true;
//			}
//		}
//
//		/**
//		 * Were any errors encountered?
//		 * @return true if an error was encountered, false otherwise.
//		 */
//		public boolean hasErrors()
//		{
//			return this.myHasErrors;
//		}
//	}
}

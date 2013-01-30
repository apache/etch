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

package org.apache.etch.tests;

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

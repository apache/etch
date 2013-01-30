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

import static org.junit.Assert.assertTrue;

import java.util.concurrent.Semaphore;

import org.apache.etch.bindings.java.util.StrIntHashMap;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;


/**
 * @author rebarraz
 *
 */
public class TestMainTest1Client
{
	// Settings
	//--------------------------------------------------------------------------

	// Instance of the class.
	private ImplTest1Client myMainTestClient;

	// Instances of arguments types
	private Boolean			myBool;
	private Byte			myByte;
	private Short			myShort;
	private Integer 		myInt;
	private Long			myLong;
	private Float			myFloat;
	private Double  		myDouble;
	private Object			myObj;
	private String			myString;
	private StrIntHashMap	myMap;
	private org.apache.etch.tests.Test1.E1 myE1;
	private org.apache.etch.tests.Test1.S1 myS1;
	private org.apache.etch.tests.Test1.S2 myS2;
	private org.apache.etch.tests.Test1.S3 myS3;
	private org.apache.etch.tests.Test1.S4 myS4;
	private boolean[]		myBools;
	private byte[]			myBytes;
	private short[]			myShorts;
	private int[]			myInts;
	private long[]			myLongs;
	private float[]			myFloats;
	private double[]		myDoubles;
	private Object[]		myObjs;
	private String[]		myStrings;
	private StrIntHashMap[]	myMaps;
	private org.apache.etch.tests.Test1.E1[] myE1s;
	private org.apache.etch.tests.Test1.S1[] myS1s;
	private org.apache.etch.tests.Test1.S2[] myS2s;


	// Maintenance
	// -------------------------------------------------------------------------

	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp()
		throws Exception
	{
		myMainTestClient = new ImplTest1Client( null );
	}


	// Tests
	//--------------------------------------------------------------------------

	/**
	 */
	
	@Test
	public void classInheritance()
	{
		assertTrue( myMainTestClient instanceof ImplTest1Client );
		assertTrue( myMainTestClient instanceof BaseTest1Client );
	}
	
	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void nothingMethod()
	{
		myMainTestClient.nothing();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void incrMethod()
	{
		myInt = myMainTestClient.incr(myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void addMethod()
	{
		myInt = myMainTestClient.add(myInt, myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void subMethod()
	{
		myInt = myMainTestClient.sub(myInt, myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void sumMethod()
	{
		myInt = myMainTestClient.sum(myInts);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void transMethod()
	{
		myInt = myMainTestClient.trans(myE1, myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void distMethod()
	{
		myDouble = myMainTestClient.dist(myS1, myS1);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void fillMethod()
	{
		myInts = myMainTestClient.fill(myInt, myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void fillObjectMethod()
	{
		myObjs = myMainTestClient.fillObject(myInt, myObj);
	}

	/**
	 * @throws Exception
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void blowMethod()
		throws Exception
	{
		myMainTestClient.blow(myString, myInt);
	}

	/**
	 * @throws Exception
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void beetsMethod()
		throws Exception
	{
		myInt = myMainTestClient.beets(myE1);
	}

	/**
	 * @throws Exception
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void throwExcp5Method()
		throws Exception
	{
		myMainTestClient.throwExcp5(myString, myInt, myObj);
	}

	/**
	 * @throws Exception
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void throwExcp6Method()
		throws Exception
	{
		myMainTestClient.throwExcp6(myString, myInt, myObjs);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_booleanMethod()
	{
		myBool = myMainTestClient.p_boolean(myBool);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_boolean_arrayMethod()
	{
		myBools = myMainTestClient.p_boolean_array(myBools);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_byteMethod()
	{
		myByte = myMainTestClient.p_byte(myByte);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_byte_arrayMethod()
	{
		myBytes = myMainTestClient.p_byte_array(myBytes);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_shortMethod()
	{
		myShort = myMainTestClient.p_short(myShort);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_short_arrayMethod()
	{
		myShorts = myMainTestClient.p_short_array(myShorts);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_intMethod()
	{
		myInt = myMainTestClient.p_int(myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_int_arrayMethod()
	{
		myInts = myMainTestClient.p_int_array(myInts);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_longMethod()
	{
		myLong = myMainTestClient.p_long(myLong);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_long_arrayMethod()
	{
		myLongs = myMainTestClient.p_long_array(myLongs);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_floatMethod()
	{
		myFloat = myMainTestClient.p_float(myFloat);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_float_arrayMethod()
	{
		myFloats = myMainTestClient.p_float_array(myFloats);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_doubleMethod()
	{
		myDouble = myMainTestClient.p_double(myDouble);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_double_arrayMethod()
	{
		myDoubles = myMainTestClient.p_double_array(myDoubles);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_stringMethod()
	{
		myString = myMainTestClient.p_string(myString);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_string_arrayMethod()
	{
		myStrings = myMainTestClient.p_string_array(myStrings);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_E1Method()
	{
		myE1 = myMainTestClient.p_E1(myE1);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_E1_arrayMethod()
	{
		myE1s = myMainTestClient.p_E1_array(myE1s);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_S1Method()
	{
		 myS1 = myMainTestClient.p_S1(myS1);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_S1_arrayMethod()
	{
		 myS1s = myMainTestClient.p_S1_array(myS1s);

	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_S2Method()
	{
		myS2 = myMainTestClient.p_S2(myS2);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_S2_arrayMethod()
	{
		myS2s = myMainTestClient.p_S2_array(myS2s);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_BlobMethod()
	{
		myMap = myMainTestClient.p_Blob(myMap);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_Blob_ArrayMethod()
	{
		myMaps = myMainTestClient.p_Blob_array(myMaps);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_objectMethod()
	{
		myObj = myMainTestClient.p_object(myObj);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_object_arrayMethod()
	{
		myMainTestClient.p_object_array(null);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_object_structMethod()
	{
		myS3 = myMainTestClient.p_object_struct(myS3);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_object_struct_arrayMethod()
	{
		myS4 = myMainTestClient.p_object_struct_array(myS4);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void alwaysWordsMethod()
	{
		myInt = myMainTestClient.alwaysWorks();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void isTrueMethod()
	{
		myBool = myMainTestClient.isTrue();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void alwaysFailsMethod()
	{
		myMainTestClient.alwaysFails();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void isFalseMethod()
	{
		myBool = myMainTestClient.isFalse();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m1Method()
	{
		myMainTestClient.m1();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m1Method()
	{
		myBool = myMainTestClient.can_m1();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m2Method()
	{
		myMainTestClient.m2();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m2Method()
	{
		myBool = myMainTestClient.can_m2(null);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m3Method()
	{
		myMainTestClient.m3();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m3Method()
	{
		myBool = myMainTestClient.can_m3(null);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m4Method()
	{
		myMainTestClient.m4();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m4Method()
	{
		myBool = myMainTestClient.can_m4(myBool);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m5Method()
	{
		myMainTestClient.m5();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m5Method()
	{
		myBool = myMainTestClient.can_m5(myByte);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m7Method()
	{
		myMainTestClient.m7();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m7Method()
	{
		myBool = myMainTestClient.can_m7(myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m8Method()
	{
		myMainTestClient.m8();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m8Method()
	{
		myBool = myMainTestClient.can_m8(myLong);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m9Method()
	{
		myMainTestClient.m9();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m9Method()
	{
		myBool = myMainTestClient.can_m9(myFloat);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m10Method()
	{
		myMainTestClient.m10();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m10Method()
	{
		myBool = myMainTestClient.can_m10(myDouble);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m11Method()
	{
		myMainTestClient.m11();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m11Method()
	{
		myBool = myMainTestClient.can_m11(myString);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m12Method()
	{
		myMainTestClient.m12();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m12Method()
	{
		myBool = myMainTestClient.can_m12(myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m13Method()
	{
		myMainTestClient.m13(myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m13Method()
	{
		myBool = myMainTestClient.can_m13(myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m14Method()
	{
		myMainTestClient.m14(myS1);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m14Method()
	{
		myBool = myMainTestClient.can_m14(myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m15Method()
	{
		myMainTestClient.m15();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m15Method()
	{
		myBool = myMainTestClient.can_m15(myE1);
	}

	/**
	 * Test to make sure the main method can run (at all).
	 */
/*	@Test
	public void mainMethod()
	{
		final ClientThread m = new ClientThread();
		m.run();

		try
		{
			m.join(4000);
		}
		catch (final Exception e)
		{
		}

		if (m.hasErrors())
			fail();

	}
	*/

	/**
	 *
	 * @throws Exception
	 */
	@Test
	public void mainServiceListenerAndClient()
		throws Exception
	{
		final Lock lock = new Lock();

		final ThreadClient client = new ThreadClient(lock);
		final ThreadListener listener = new ThreadListener(lock);

		listener.start();
		client.start();

		// Wait for test to finish.
		lock.getLock(3);
		lock.releaseLock(3);

		if (client.hasErrors() || listener.hasErrors())
		{
			Assert.fail();
		}
	}


	// Helpers
	//--------------------------------------------------------------------------

	/**
	 * Prints exception info to stderr in a most descriptive way.
	 * @param e the exception to print
	 */
	public static void printException(final Exception e)
	{
		e.printStackTrace(System.err);
	}

//	/**
//	 * Simple thread class for calling main WITHOUT a listener.
//	 */
//	private static class ClientThread extends Thread
//	{
//		/**
//		 * false until unexpected stuff goes down
//		 */
//		private boolean myHasErrors;
//
//		/**
//		 * No errors to begin with.
//		 */
//		public ClientThread()
//		{
//			myHasErrors = false;
//		}
//
//		/**
//		 * Go at it.
//		 */
//		@Override
//		public void run()
//		{
//			try
//			{
//				MainTest1Client.main(new String[] {});
//			}
//			catch (ConnectException e1)
//			{
//			}
//			catch (InterruptedException e2)
//			{
//			}
//			catch (Exception e3)
//			{
//				// Unexpected
//				System.out.println("WHAT!???");
//				printException(e3);
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


	/**
	 * Painfully simple locking mechanism for ordering multithreaded blocks.
	 */
	private static class Lock
	{
		/**
		 * Counter for simple (manual) ordering mechanism.
		 */
		private int myCount;

		/**
		 * Binary semaphore. The -real- lock.
		 */
		private Semaphore mySemaphore;

		/**
		 * Start the count at 0 with the lock available and no errors recorded.
		 */
		public Lock()
		{
			myCount = 0;
			mySemaphore = new Semaphore(1);
		}

		/**
		 * Blocks until this lock can be grabbed by the count.
		 * @param c
		 */
		public void getLock(final int c)
		{
			while(c > myCount)
			{
				try
				{
					Thread.sleep( 10 );
				}
				catch ( InterruptedException e )
				{
					// ignore
				}
			}

			try
			{
				mySemaphore.acquire();
			}
			catch(final Exception e)
			{
				// Won't happen thanks to atomic release.
			}
		}

		/**
		 * Releases the lock if this lock is held.
		 * @param c
		 */
		public synchronized void releaseLock(final int c)
		{
			if (c == myCount && mySemaphore.availablePermits() == 0)
			{
				mySemaphore.release();
				++myCount;
			}
		}
	}

	/**
	 * Thread used to run the MainServiceClient.
	 */
	private static class ThreadClient extends Thread
	{
		/**
		 * Shared lock.
		 */
		private Lock ourLock;

		/**
		 * Record when an error occurs with client.
		 */
		private boolean myHasErrors;

		/**
		 * Share the lock and start with no errors in sight.
		 * @param l the lock
		 */
		public ThreadClient(final Lock l)
		{
			ourLock = l;
			myHasErrors = false;
		}

		/**
		 * Start client.
		 */
		@Override
		public void run()
		{
			// 0. Wait for the listener to start his engine.

			// 1. Start the client.  Let it start, connect, and die gracefully.
			ourLock.getLock(1);
			try
			{
				MainTest1Client.main( new String[] {} );
			}
			catch (final Exception e)
			{
				// SHOULDN'T HAPPEN!
				TestMainTest1Client.printException(e);

				myHasErrors = true;
			}
			ourLock.releaseLock(1);

			// 2. The listener dies.
		}

		/**
		 * Were any errors encountered?
		 * @return true if an error was encountered, false otherwise.
		 */
		public boolean hasErrors()
		{
			return myHasErrors;
		}
	}

	/**
	 * Turn the Listener into a simple thread that just listens.
	 */
	private static class SimpleThreadListener extends Thread
	{
		/**
		 * Record when an error happens with the listener.
		 */
		private boolean myHasErrors;

		/**
		 * No errors in sight.
		 */
		public SimpleThreadListener()
		{
			myHasErrors = false;
		}

		/**
		 * Listen.
		 */
		@Override
		public void run()
		{
			try
			{
				MainTest1Listener.main( new String[] {} );
			}
			catch (final Exception e)
			{
				// SHOULDN'T HAPPEN!
				TestMainTest1Client.printException(e);

				myHasErrors = true;
			}
		}

		/**
		 * Were any errors encountered?
		 * @return true if an error was encountered, false otherwise.
		 */
		public boolean hasErrors()
		{
			return myHasErrors;
		}
	}

	/**
	 * Thread for running and controlling the MainServiceListener.
	 */
	private static class ThreadListener extends Thread
	{
		/**
		 * Share the lock.
		 */
		private Lock ourLock;

		/**
		 * Record if an error happens with the listener.
		 */
		private boolean myHasErrors;

		/**
		 * Share the lock and start off with no errors in sight.
		 * @param l the lock
		 */
		public ThreadListener(final Lock l)
		{
			ourLock = l;
			myHasErrors = false;
		}

		/**
		 * Start the listener, let it wait as some water runs through the pipes.
		 * Then, signal to the client to start. Wait for the client to signal
		 * it's death. Finally, cleanup the listener.
		 */
		@Override
		public void run()
		{
			// 0. Start the Listener.
			ourLock.getLock(0);
			final SimpleThreadListener listener = new SimpleThreadListener();
			listener.start();
			ourLock.releaseLock(0);

			// 1. Meanwhile, back on the ranch... A new Client is born. And
			// killed.  Long live the client!

			// 2. Harshly stop the listener and ignore its whining.
			ourLock.getLock(2);
			try
			{
				myHasErrors = listener.hasErrors();
				listener.wait(1);
			}
			catch(final Exception e)
			{
				// Do nothing. We beat the listener into submission.
			}
			ourLock.releaseLock(2);
		}

		/**
		 * Were any errors encountered?
		 * @return true if an error was encountered, false otherwise.
		 */
		public boolean hasErrors()
		{
			return myHasErrors;
		}
	}
}

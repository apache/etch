/**
 *
 */
package etch.bindings.java.compiler.test;


import static org.junit.Assert.assertTrue;

import java.util.concurrent.Semaphore;

import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import etch.bindings.java.util.StrIntHashMap;


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
	private etch.bindings.java.compiler.test.Test1.E1 myE1;
	private etch.bindings.java.compiler.test.Test1.S1 myS1;
	private etch.bindings.java.compiler.test.Test1.S2 myS2;
	private etch.bindings.java.compiler.test.Test1.S3 myS3;
	private etch.bindings.java.compiler.test.Test1.S4 myS4;
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
	private etch.bindings.java.compiler.test.Test1.E1[] myE1s;
	private etch.bindings.java.compiler.test.Test1.S1[] myS1s;
	private etch.bindings.java.compiler.test.Test1.S2[] myS2s;


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
		assertTrue( this.myMainTestClient instanceof ImplTest1Client );
		assertTrue( this.myMainTestClient instanceof BaseTest1Client );
	}
	
	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void nothingMethod()
	{
		this.myMainTestClient.nothing();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void incrMethod()
	{
		this.myInt = this.myMainTestClient.incr(this.myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void addMethod()
	{
		this.myInt = this.myMainTestClient.add(this.myInt, this.myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void subMethod()
	{
		this.myInt = this.myMainTestClient.sub(this.myInt, this.myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void sumMethod()
	{
		this.myInt = this.myMainTestClient.sum(this.myInts);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void transMethod()
	{
		this.myInt = this.myMainTestClient.trans(this.myE1, this.myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void distMethod()
	{
		this.myDouble = this.myMainTestClient.dist(this.myS1, this.myS1);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void fillMethod()
	{
		this.myInts = this.myMainTestClient.fill(this.myInt, this.myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void fillObjectMethod()
	{
		this.myObjs = this.myMainTestClient.fillObject(this.myInt, this.myObj);
	}

	/**
	 * @throws Exception
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void blowMethod()
		throws Exception
	{
		this.myMainTestClient.blow(this.myString, this.myInt);
	}

	/**
	 * @throws Exception
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void beetsMethod()
		throws Exception
	{
		this.myInt = this.myMainTestClient.beets(this.myE1);
	}

	/**
	 * @throws Exception
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void throwExcp5Method()
		throws Exception
	{
		this.myMainTestClient.throwExcp5(this.myString, this.myInt, this.myObj);
	}

	/**
	 * @throws Exception
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void throwExcp6Method()
		throws Exception
	{
		this.myMainTestClient.throwExcp6(this.myString, this.myInt, this.myObjs);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_booleanMethod()
	{
		this.myBool = this.myMainTestClient.p_boolean(this.myBool);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_boolean_arrayMethod()
	{
		this.myBools = this.myMainTestClient.p_boolean_array(this.myBools);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_byteMethod()
	{
		this.myByte = this.myMainTestClient.p_byte(this.myByte);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_byte_arrayMethod()
	{
		this.myBytes = this.myMainTestClient.p_byte_array(this.myBytes);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_shortMethod()
	{
		this.myShort = this.myMainTestClient.p_short(this.myShort);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_short_arrayMethod()
	{
		this.myShorts = this.myMainTestClient.p_short_array(this.myShorts);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_intMethod()
	{
		this.myInt = this.myMainTestClient.p_int(this.myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_int_arrayMethod()
	{
		this.myInts = this.myMainTestClient.p_int_array(this.myInts);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_longMethod()
	{
		this.myLong = this.myMainTestClient.p_long(this.myLong);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_long_arrayMethod()
	{
		this.myLongs = this.myMainTestClient.p_long_array(this.myLongs);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_floatMethod()
	{
		this.myFloat = this.myMainTestClient.p_float(this.myFloat);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_float_arrayMethod()
	{
		this.myFloats = this.myMainTestClient.p_float_array(this.myFloats);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_doubleMethod()
	{
		this.myDouble = this.myMainTestClient.p_double(this.myDouble);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_double_arrayMethod()
	{
		this.myDoubles = this.myMainTestClient.p_double_array(this.myDoubles);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_stringMethod()
	{
		this.myString = this.myMainTestClient.p_string(this.myString);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_string_arrayMethod()
	{
		this.myStrings = this.myMainTestClient.p_string_array(this.myStrings);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_E1Method()
	{
		this.myE1 = this.myMainTestClient.p_E1(this.myE1);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_E1_arrayMethod()
	{
		this.myE1s = this.myMainTestClient.p_E1_array(this.myE1s);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_S1Method()
	{
		 this.myS1 = this.myMainTestClient.p_S1(this.myS1);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_S1_arrayMethod()
	{
		 this.myS1s = this.myMainTestClient.p_S1_array(this.myS1s);

	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_S2Method()
	{
		this.myS2 = this.myMainTestClient.p_S2(this.myS2);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_S2_arrayMethod()
	{
		this.myS2s = this.myMainTestClient.p_S2_array(this.myS2s);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_BlobMethod()
	{
		this.myMap = this.myMainTestClient.p_Blob(this.myMap);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_Blob_ArrayMethod()
	{
		this.myMaps = this.myMainTestClient.p_Blob_array(this.myMaps);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_objectMethod()
	{
		this.myObj = this.myMainTestClient.p_object(this.myObj);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_object_arrayMethod()
	{
		this.myMainTestClient.p_object_array(null);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_object_structMethod()
	{
		this.myS3 = this.myMainTestClient.p_object_struct(this.myS3);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void p_object_struct_arrayMethod()
	{
		this.myS4 = this.myMainTestClient.p_object_struct_array(this.myS4);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void alwaysWordsMethod()
	{
		this.myInt = this.myMainTestClient.alwaysWorks();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void isTrueMethod()
	{
		this.myBool = this.myMainTestClient.isTrue();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void alwaysFailsMethod()
	{
		this.myMainTestClient.alwaysFails();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void isFalseMethod()
	{
		this.myBool = this.myMainTestClient.isFalse();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m1Method()
	{
		this.myMainTestClient.m1();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m1Method()
	{
		this.myBool = this.myMainTestClient.can_m1();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m2Method()
	{
		this.myMainTestClient.m2();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m2Method()
	{
		this.myBool = this.myMainTestClient.can_m2(null);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m3Method()
	{
		this.myMainTestClient.m3();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m3Method()
	{
		this.myBool = this.myMainTestClient.can_m3(null);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m4Method()
	{
		this.myMainTestClient.m4();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m4Method()
	{
		this.myBool = this.myMainTestClient.can_m4(this.myBool);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m5Method()
	{
		this.myMainTestClient.m5();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m5Method()
	{
		this.myBool = this.myMainTestClient.can_m5(this.myByte);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m7Method()
	{
		this.myMainTestClient.m7();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m7Method()
	{
		this.myBool = this.myMainTestClient.can_m7(this.myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m8Method()
	{
		this.myMainTestClient.m8();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m8Method()
	{
		this.myBool = this.myMainTestClient.can_m8(this.myLong);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m9Method()
	{
		this.myMainTestClient.m9();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m9Method()
	{
		this.myBool = this.myMainTestClient.can_m9(this.myFloat);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m10Method()
	{
		this.myMainTestClient.m10();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m10Method()
	{
		this.myBool = this.myMainTestClient.can_m10(this.myDouble);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m11Method()
	{
		this.myMainTestClient.m11();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m11Method()
	{
		this.myBool = this.myMainTestClient.can_m11(this.myString);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m12Method()
	{
		this.myMainTestClient.m12();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m12Method()
	{
		this.myBool = this.myMainTestClient.can_m12(this.myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m13Method()
	{
		this.myMainTestClient.m13(this.myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m13Method()
	{
		this.myBool = this.myMainTestClient.can_m13(this.myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m14Method()
	{
		this.myMainTestClient.m14(this.myS1);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m14Method()
	{
		this.myBool = this.myMainTestClient.can_m14(this.myInt);
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void m15Method()
	{
		this.myMainTestClient.m15();
	}

	/**
	 */
	@Test
	(expected = UnsupportedOperationException.class)
	public void can_m15Method()
	{
		this.myBool = this.myMainTestClient.can_m15(this.myE1);
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
			this.myCount = 0;
			this.mySemaphore = new Semaphore(1);
		}

		/**
		 * Blocks until this lock can be grabbed by the count.
		 * @param c
		 */
		public void getLock(final int c)
		{
			while(c > this.myCount)
			{
				;
			}

			try
			{
				this.mySemaphore.acquire();
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
			if ((c == this.myCount) && (this.mySemaphore.availablePermits() == 0))
			{
				this.mySemaphore.release();
				++this.myCount;
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
			this.ourLock = l;
			this.myHasErrors = false;
		}

		/**
		 * Start client.
		 */
		@Override
		public void run()
		{
			// 0. Wait for the listener to start his engine.

			// 1. Start the client.  Let it start, connect, and die gracefully.
			this.ourLock.getLock(1);
			try
			{
				MainTest1Client.main( new String[] {} );
			}
			catch (final Exception e)
			{
				// SHOULDN'T HAPPEN!
				TestMainTest1Client.printException(e);

				this.myHasErrors = true;
			}
			this.ourLock.releaseLock(1);

			// 2. The listener dies.
		}

		/**
		 * Were any errors encountered?
		 * @return true if an error was encountered, false otherwise.
		 */
		public boolean hasErrors()
		{
			return this.myHasErrors;
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
			this.myHasErrors = false;
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

				this.myHasErrors = true;
			}
		}

		/**
		 * Were any errors encountered?
		 * @return true if an error was encountered, false otherwise.
		 */
		public boolean hasErrors()
		{
			return this.myHasErrors;
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
			this.ourLock = l;
			this.myHasErrors = false;
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
			this.ourLock.getLock(0);
			final SimpleThreadListener listener = new SimpleThreadListener();
			listener.start();
			this.ourLock.releaseLock(0);

			// 1. Meanwhile, back on the ranch... A new Client is born. And
			// killed.  Long live the client!

			// 2. Harshly stop the listener and ignore its whining.
			this.ourLock.getLock(2);
			try
			{
				this.myHasErrors = listener.hasErrors();
				listener.wait(1);
			}
			catch(final Exception e)
			{
				// Do nothing. We beat the listener into submission.
			}
			this.ourLock.releaseLock(2);
		}

		/**
		 * Were any errors encountered?
		 * @return true if an error was encountered, false otherwise.
		 */
		public boolean hasErrors()
		{
			return this.myHasErrors;
		}
	}
}

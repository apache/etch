package etch.util.test;

import static org.junit.Assert.fail;

import org.junit.Test;

/**
 * Help for junit tests.
 */
public class TestHelp
{
	/** @throws Exception */
	@Test
	public void testRelError1() throws Exception
	{
		assertRelError( 10, 9, .11 );
		assertRelError( 10, 11, .14 );
		assertRelError( 20, 19, .07 );
		assertRelError( 19, 23, .22 );
	}
	
	/** @throws Exception */
	@Test( expected = AssertionError.class )
	public void testRelError2() throws Exception
	{
		assertRelError( 9, 8, .1 );
	}
	
	/** @throws Exception */
	@Test( expected = AssertionError.class )
	public void testRelError3() throws Exception
	{
		assertRelError( 9, 10, .1 );
	}
	
	/** @throws Exception */
	@Test
	public void testAbsError1() throws Exception
	{
		assertAbsError( 11, 15, 4 );
		assertAbsError( 15, 10, 5 );
		assertAbsError( 5, 3, 2 );
		assertAbsError( 4, 7, 3 );
	}
	
	/** @throws Exception */
	@Test( expected = AssertionError.class )
	public void testAbsError2() throws Exception
	{
		assertAbsError( 11, 15, 3 );
	}
	
	/** @throws Exception */
	@Test( expected = AssertionError.class )
	public void testAbsError3() throws Exception
	{
		assertAbsError( 19, 15, 3 );
	}
	
	/**
	 * Creates a thread to delay running something.
	 * @param delay
	 * @param runnable
	 * @return the thread which is not started.
	 */
	public static Thread delay( final int delay, final DelayRunnable runnable )
	{
		Thread t = new Thread( new Runnable()
		{
			public void run()
			{
				try
				{
					Thread.sleep( delay );
					runnable.run();
				}
				catch ( Exception e )
				{
					e.printStackTrace();
				}
			}
		} );
		return t;
	}

	/**
	 * Creates a thread to delay running something.
	 * @param delay
	 * @param runnable
	 * @return the thread which is started.
	 */
	public static Thread delayAndStart( final int delay, final DelayRunnable runnable )
	{
		Thread t = delay( delay, runnable );
		t.start();
		return t;
	}
	
	/**
	 * Interface to implement when using delay.
	 */
	public interface DelayRunnable
	{
		/**
		 * @throws Exception
		 */
		public void run() throws Exception;
	}

	public static void assertRelError( double expected, double actual, double error )
	{
		double relError = relError( expected, actual );
		if (relError > error)
			fail( String.format( "expected: <%f> but was: <%f> relative error: <%f>", expected, actual, relError ) );
	}
	
	public static void assertAbsError( double expected, double actual, double error )
	{
		double absError = absError( expected, actual );
		if (absError > error)
			fail( String.format( "expected: <%f> but was: <%f> absolute error: <%f>", expected, actual, absError ) );
	}
	
	public static double relError( double expected, double actual )
	{
		return absError( expected, actual ) / expected;
	}
	
	public static double absError( double expected, double actual )
	{
		return Math.abs( expected - actual );
	}
}

/**
 * 
 */
package etch.bindings.java.support;

/**
 * Interface to a queued or free thread pool.
 */
public interface Pool
{
	/**
	 * @param runnable the thing to run.
	 * @throws Exception if there is a problem scheduling the
	 * runnable to run.
	 */
	void run( PoolRunnable runnable ) throws Exception;
	
	/**
	 * A runnable which can throw an exception.
	 */
	public interface PoolRunnable
	{
		/**
		 * @throws Exception
		 */
		public void run() throws Exception;

		/**
		 * Reports an exception thrown by the run method.
		 * @param e
		 */
		public void exception( Exception e );
	}
}

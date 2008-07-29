package etch.bindings.java.support;

/**
 * A pool which runs the runnable directly.
 */
public class NullPool implements Pool
{
	public void run( PoolRunnable runnable ) throws Exception
	{
		try
		{
			runnable.run();
		}
		catch ( Exception e )
		{
			runnable.exception( e );
		}
	}
}

package etch.bindings.java.support;

import metreos.util.Todo;
import metreos.util.TodoManager;

/**
 * A queued implementation of pool.
 */
public class QueuedPool implements Pool
{
	/**
	 * Constructs the queued pool with a specified todo manager.
	 * @param mgr
	 */
	public QueuedPool( TodoManager mgr )
	{
		this.mgr = mgr;
	}
	
	/**
	 * Constructs the queued pool with a default todo manager.
	 * @throws Exception
	 */
	public QueuedPool() throws Exception
	{
		this( TodoManager.getTodoManager() );
	}
	
	private final TodoManager mgr;
	
	public void run( final PoolRunnable runnable ) throws Exception
    {
        mgr.add( new Todo()
        {
			public void doit( TodoManager mgr ) throws Exception
            {
                runnable.run();
            }

			public void exception( TodoManager mgr, Exception e )
            {
                runnable.exception( e );
            }
        } );
    }
}
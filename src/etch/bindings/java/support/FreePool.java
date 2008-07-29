package etch.bindings.java.support;

/**
 * A free implementation of Pool.
 */
public class FreePool implements Pool
{
	/**
	 * Constructs a FreePool with specified maxSize.
	 * @param maxSize maximum number of free threads at one
	 * time. after that, reject requests.
	 */
	public FreePool( int maxSize )
	{
		this.maxSize = maxSize;
	}
	
	/**
	 * Constructs a FreePool with maxSize 50.
	 */
	public FreePool()
	{
		this( 50 );
	}
	
	private final int maxSize;
	
	private final ThreadGroup tg = new ThreadGroup( "FreePoolThreadGroup" );
	
	private boolean open = true;
	
	/**
	 * Closes the pool. This just marks the pool as being closed, it doesn't
	 * actually do anything to the currently running thread. But no more
	 * threads are allowed to start.
	 */
	public void close()
	{
		open = false;
	}
	
	/**
	 * Joins each of the threads in this pool until there
	 * are none left. The pool will be closed first.
	 * @throws InterruptedException
	 */
	public void join() throws InterruptedException
	{
		close();
		Thread[] list = new Thread[10];
		int n;
		while ((n = tg.enumerate( list )) > 0)
			for (int i = 0; i < n; i++)
				list[i].join();
	}
	
	public synchronized void run( final PoolRunnable runnable )
	{
		if (!open || tg.activeCount() >= maxSize)
			throw new IllegalStateException( "free pool thread count exceeded or pool closed" );
		
		new Thread( tg, new Runnable()
		{
			public void run()
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
		} ).start();
	}
}
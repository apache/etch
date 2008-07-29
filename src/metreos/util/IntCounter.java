package metreos.util;

/**
 * A safe integer counter.
 */
public class IntCounter
{
	/**
	 * Adjusts the value of the counter by k.
	 * @param k the adjustment to the counter.
	 * @return the new value of the counter.
	 */
	public synchronized int adjust( int k )
    {
        value += k;
        return value;
    }

	/**
	 * @return the value of the counter.
	 */
	public int get()
    {
        return value;
    }
	
	private int value;
}
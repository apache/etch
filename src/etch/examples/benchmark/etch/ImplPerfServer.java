package etch.examples.benchmark.etch;

/**
 * Implementation of PerfServer.
 */
public class ImplPerfServer implements PerfServer
{
	public Integer add( Integer x, Integer y )
	{
		return x + y;
	}

	public Integer sum( int[] values )
	{
		int sum = 0;
		for (int value: values)
			sum += value;
		return sum;
	}

	public void report( Integer code, String msg )
	{
		// do nothing.
	}
}

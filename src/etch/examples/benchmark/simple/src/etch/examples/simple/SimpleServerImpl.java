package etch.examples.simple;

public class SimpleServerImpl implements SimpleServer 
{
	private final SimpleClient client;
	
	public SimpleServerImpl(SimpleClient client)
	{
		this.client = client;
	}

	public Integer add(Integer x, Integer y) 
	{
		return (x+y);
	}

	public Integer sum(int[] vals) 
	{
		int sum = 0;		
		for (int v: vals)
			sum += v;
		return sum;
	}

	public void report( Integer code, String msg )
	{
		// ignore.
	}
}

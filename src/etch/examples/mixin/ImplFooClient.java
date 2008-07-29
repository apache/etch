
package etch.examples.mixin;

/**
 * Implementation of methods for FooClient.
 */
public class ImplFooClient implements FooClient
{
	/**
	 * Constructs the ImplFooClient.
	 * @param server
	 */
	public ImplFooClient( RemoteFooServer server )
	{
		this.server = server;
	}
	
	@SuppressWarnings("unused")
	protected FooServer server;

	public Boolean foo1( etch.examples.mixin.Foo.FooData fooData )
	{
		throw new UnsupportedOperationException();
	}

	public Boolean foo3( etch.examples.mixin.Foo.FooData fooData )
	{
		throw new UnsupportedOperationException();
	}

	public Boolean bar1( BarData barData )
	{
		throw new UnsupportedOperationException();
	}

	public Boolean bar3( BarData barData )
	{
		throw new UnsupportedOperationException();
	}

	public Boolean baz1( BazData bazData )
	{
		throw new UnsupportedOperationException();
	}

	public Boolean baz3( BazData bazData )
	{
		throw new UnsupportedOperationException();
	}
}

package etch.examples.mixin;


/**
 * Implementation of methods for FooServer.
 */
public class ImplFooServer extends BaseFooServer
{
	/**
	 * Constructs the ImplFooServer.
	 *
	 * @param client the connection to our client.
	 */
	public ImplFooServer( RemoteFooClient client )
	{
		this.client = client;
	}
	
	private final RemoteFooClient client;

	public Boolean foo1( etch.examples.mixin.Foo.FooData fooData )
	{
		System.out.println( "foo1 called with "+fooData );
		return true;
	}

	public Boolean foo2( etch.examples.mixin.Foo.FooData fooData )
	{
		System.out.println( "foo2 called with "+fooData );
		return false;
	}

	public Boolean bar1( BarData barData )
	{
		System.out.println( "bar1 called with "+barData );
		return true;
	}

	public Boolean bar2( BarData barData )
	{
		System.out.println( "bar2 called with "+barData );
		return false;
	}

	public Boolean baz1( BazData bazData )
	{
		System.out.println( "baz1 called with "+bazData );
		return true;
	}

	public Boolean baz2( BazData bazData )
	{
		System.out.println( "baz2 called with "+bazData );
		return false;
	}

	public void _sessionNotify( Object event ) throws Exception
	{
		System.out.println( event );
	}
}

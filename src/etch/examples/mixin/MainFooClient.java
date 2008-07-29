package etch.examples.mixin;

import etch.bindings.java.util.StrStrHashMap;
import etch.examples.mixin.Foo.FooData;
import etch.examples.mixin.FooHelper.FooClientFactory;
import etch.examples.mixin.bar.Bar.BarData;
import etch.examples.mixin.baz.Baz.BazData;

/**
 * Main implementation for FooClient.
 */
public class MainFooClient implements FooClientFactory
{
	/**
	 * Main for FooClient
	 * @param args
	 * @throws Exception
	 */
	public static void main(String[] args)
		throws Exception
	{
		String uri = "tcp://localhost:4001";

		RemoteFooServer server = FooHelper.newServer( uri, null, new MainFooClient() );
		
		server._startAndWaitUp( 4000 );
		
		StrStrHashMap bazTable = new StrStrHashMap();
		bazTable.put( "def", "baz" );
		
		StrStrHashMap barTable = new StrStrHashMap();
		barTable.put( "def", "bar" );
		
		StrStrHashMap fooTable = new StrStrHashMap();
		fooTable.put( "def", "foo" );
		
		BazData bazData = new BazData( 1, bazTable );
		BarData barData = new BarData( 2, barTable, bazData );
		FooData fooData = new FooData( 3, fooTable, barData, bazData );

		boolean ok;
		
		ok = server.baz1( bazData );
		System.out.println( "baz1 => "+ok );
		
		ok = server.baz2( bazData );
		System.out.println( "baz2 => "+ok );

		ok = server.bar1( barData );
		System.out.println( "bar1 => "+ok );
		
		ok = server.bar2( barData );
		System.out.println( "bar2 => "+ok );
		
		ok = server.foo1( fooData );
		System.out.println( "foo1 => "+ok );
		
		ok = server.foo2( fooData );
		System.out.println( "foo2 => "+ok );
		
		server._stopAndWaitDown( 4000 );
	}

	public FooClient newFooClient( RemoteFooServer server ) throws Exception
	{
		return new ImplFooClient( server );
	}
}

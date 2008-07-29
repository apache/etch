package etch.examples.example;

/**
 * An implementation of ExampleServer to demonstrate the use.
 */
public class ImplExampleServer implements ExampleServer
{
	/**
	 * @param client an ExampleClient instance for server implementation
	 * to use to call back to the client.
	 */
	public ImplExampleServer( ExampleClient client )
	{
		this.client = client;
	}
	
	private final ExampleClient client;

	public void hello( String msg )
	{
		System.out.println( msg );
		client.howdy( "well, hello there client." );
	}
}

package etch.examples.example;

import metreos.util.URL;

/**
 * Tests the RemoteExampleServer interface. Start the TestRemoteExampleListener
 * first.
 */
public class MainExampleClient implements ExampleClient
{
	/**
	 * Program entry point.
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		System.out.println( "MainExampleClient" );
		
		URL uri = new URL( "tcp://localhost:4001" );
		
		MainExampleClient client = new MainExampleClient();
		RemoteExampleServer server = ExampleHelper.newExampleServer( uri, null, client );
		
		System.out.println( "starting..." );
		server._startAndWaitUp( 4000 );
		System.out.println( "started." );

		System.out.println( "greeting..." );
		server.hello( "howdy server!" );
		System.out.println( "greeted." );
		
		System.out.println( "stopping..." );
		server._stopAndWaitDown( 4000 );
		System.out.println( "stopped." );
	}
	
	/**
	 * Constructs the MainExampleClient. 
	 */
	public MainExampleClient()
	{
		// nothing to do.
	}

	public void howdy( String msg )
	{
		System.out.println( msg );
	}
}

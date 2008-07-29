package etch.examples.example;

import metreos.util.URL;
import etch.bindings.java.transport.Transport;
import etch.examples.example.ExampleHelper.ExampleServerFactory;

/**
 * Starts an Example listener.
 */
public class MainExampleListener implements ExampleServerFactory
{
	/**
	 * Program entry point.
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		System.out.println( "MainExampleListener" );
		
		URL uri = new URL( "tcp://0.0.0.0:4001" );
		
		MainExampleListener implFactory = new MainExampleListener();
		
		Transport listener = ExampleHelper.newExampleListener( uri, null, implFactory );
		
		System.out.println( "starting..." );
		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		System.out.println( "started." );
		
		// wait 5 minutes and then shut down the listener.
		Thread.sleep( 5 * 60 * 1000 );
		
		System.out.println( "stopping..." );
		listener.transportControl( Transport.STOP_AND_WAIT_DOWN, 4000 );
		System.out.println( "stopped." );
	}
	
	/**
	 * Constructs the ExampleServer factory.
	 */
	public MainExampleListener()
	{
		// nothing to do.
	}

	public ExampleServer newExampleServer( ExampleClient client )
	{
		System.out.println( "new ImplExampleServer created" );
		return new ImplExampleServer( client );
	}
}

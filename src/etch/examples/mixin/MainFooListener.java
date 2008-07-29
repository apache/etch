package etch.examples.mixin;

import etch.examples.mixin.FooHelper.FooServerFactory;
import etch.util.core.io.Transport;

/**
 * Main implementation for FooListener.
 */
public class MainFooListener implements FooServerFactory
{
	/**
 	 * Main for FooListener.
	 * @param args
	 * @throws Exception
	 */
	public static void main(String[] args)
		throws Exception
	{
		String uri = "tcp://0.0.0.0:4001";

		Transport<?> listener = FooHelper.newListener( uri, null, new MainFooListener() );

		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
	}

	/**
	 * Constructs the MainFooListener.
	 */
	public MainFooListener()
	{
		// Nothing to do.
	}

	/**
	 * Return a new instance of FooServer.
	 * @param client
	 */
	public FooServer newFooServer( RemoteFooClient client )
	{
		return new ImplFooServer( client );
	}
}

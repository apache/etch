package etch.examples.closing;

import java.net.InetSocketAddress;

import etch.examples.closing.ClosingHelper.ClosingServerFactory;
import etch.util.URL;
import etch.util.core.io.TcpListener;
import etch.util.core.io.Transport;

/**
 * Main implementation for ClosingListener.
 */
public class MainClosingListener implements ClosingServerFactory
{
	/**
 	 * Main for ClosingListener.
	 * @param args
	 * @throws Exception
	 */
	public static void main(String[] args)
		throws Exception
	{
		String uri = "tcp://0.0.0.0:4006";
		
		Transport<?> listener = ClosingHelper.newListener( uri, null, new MainClosingListener() );

		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		
		InetSocketAddress localAddr = (InetSocketAddress) listener.transportQuery( TcpListener.LOCAL_ADDRESS );
		URL u = new URL( "tcp:" );
		u.setHost( localAddr.getHostName() );
		u.setPort( localAddr.getPort() );
		localUri = u.toString();
	}

	/**
	 * @return the uri of our listener.
	 */
	public static String getLocalUri()
	{
		return localUri;
	}
	
	private static String localUri;

	/**
	 * Constructs the MainClosingListener.
	 */
	public MainClosingListener()
	{
		// Nothing to do.
	}

	/**
	 * Return a new instance of ClosingServer.
	 * @param client
	 * @return the constructed server.
	 */
	public ClosingServer newClosingServer( RemoteClosingClient client )
	{
		return new ImplClosingServer( client );
	}
}

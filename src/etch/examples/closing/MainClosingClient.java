package etch.examples.closing;

import etch.util.Log;

/**
 * Main implementation for ClosingClient.
 */
public class MainClosingClient implements ClosingHelper.ClosingClientFactory
{
	/**
	 * Main for ClosingClient
	 * @param args
	 * @throws Exception
	 */
	public static void main(String[] args)
		throws Exception
	{
		Log.addSink( null );
		
		Log.report( "MainClosingClient.startingListener" );
		MainClosingListener.main( new String[] { "tcp://localhost:0" } );
		String uri = MainClosingListener.getLocalUri();
		Log.report( "MainClosingClient.listenerUp", "uri", uri );
		
		RemoteClosingServer server = ClosingHelper.newServer( uri, null, new MainClosingClient() );
		
		Log.report( "MainClosingClient.startingServer" );
		server._startAndWaitUp( 4000 );
		Log.report( "MainClosingClient.serverUp" );
		
		Thread.sleep( 100 );
		
		// this sends a message to the server which then closes the connection.
		
		Log.report( "MainClosingClient.sayingCloseToServer" );
		server.close();
		Log.report( "MainClosingClient.saidCloseToServer" );
		
		Thread.sleep( 100 );
		
		// waiting for server to close.
		
		Log.report( "MainClosingClient.waitingServerDown" );
		server._waitDown( 4000 );
		Log.report( "MainClosingClient.serverDown" );
	}

	public ClosingClient newClosingClient( RemoteClosingServer server )
		throws Exception
	{
		return new ImplClosingClient( server );
	}
}

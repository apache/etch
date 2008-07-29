package etch.examples.chat;

import metreos.util.Log;
import metreos.util.URL;
import etch.bindings.java.support.RemoteBase;
import etch.bindings.java.transport.Resources;

/**
 * Tests the RemoteChatServer interface. Start the TestRemoteChatListener
 * first.
 */
public class TestRemoteChatServer
{
	private final static String Fred = "Fred";
	private final static String Mary = "Mary";
	
	/**
	 * Program entry point.
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		Log.addSink( null );
		Log.report( "TestRemoteChatServer" );

		URL uri = new URL( "tcp://localhost:4001" );
		
		// Connect Fred //
		
		Resources resources = ChatHelper.initResources( null );
		
		final ChatClientImpl fred_client = new ChatClientImpl( Fred, false );
		final RemoteChatServer fred_server = ChatHelper.newChatServer( uri,
			resources, fred_client );
		fred_client.setServer( fred_server );
		
		fred_server._startAndWaitUp( 4000 );
		
		// Connect Mary //

		final ChatClientImpl mary_client = new ChatClientImpl( Mary, true );
		final RemoteChatServer mary_server = ChatHelper.newChatServer( uri,
			resources, mary_client );
		mary_client.setServer( mary_server );
		
		mary_server._startAndWaitUp( 4000 );
		
		// Login and chat away... //

		fred_server.login( Fred, "cisco123" );
		Log.report( "logged in", "name", Fred );

		mary_server.login( Mary, "cisco456" );
		Log.report( "logged in", "name", Mary );

		Log.report( "sending", "name", Fred );
		fred_server.send( Mary, "Hey Mary, how's it going?" );
		Log.report( "sent", "name", Fred );
		
		Thread.sleep( 5000 );
		
		Log.report( "shutting down", "name", Fred );
		((RemoteBase<ValueFactoryChat>) fred_server)._stopAndWaitDown( 4000 );
		Log.report( "shut down", "name", Fred );
		
		Thread.sleep( 5000 );
		
		Log.report( "shutting down", "name", Mary );
		((RemoteBase<ValueFactoryChat>) mary_server)._stopAndWaitDown( 4000 );
		Log.report( "shut down", "name", Mary );
	}
}

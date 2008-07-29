package etch.examples.closing;

import etch.util.Log;
import etch.util.core.io.TcpConnection;

/**
 * Call to message translator for ClosingServer.
 */
public class ImplClosingServer extends BaseClosingServer
{
	/**
	 * Constructs the ImplClosingServer.
	 *
	 * @param client
	 */
	public ImplClosingServer( RemoteClosingClient client )
	{
		this.client = client;
	}
	
	private final RemoteClosingClient client;

	public void close()
	{
		try
		{
			Log.report( "ImplClosingServer.sayingStopToClient" );
			client._transportControl( TcpConnection.STOP, null );
			Log.report( "ImplClosingServer.saidStopToClient" );
		}
		catch ( Exception e )
		{
			throw new RuntimeException( "caught exception", e );
		}
	}

	public void _sessionNotify( Object event ) throws Exception
	{
		Log.report( "ImplClosingServer._sessionNotify", "event", event );
	}
}

package etch.examples.closing;

import etch.util.Log;

/**
 * Call to message translator for ClosingClient.
 */
public class ImplClosingClient extends BaseClosingClient
{
	/**
	 * Constructs the ImplClosingClient.
	 * @param server 
 	 */
	public ImplClosingClient( RemoteClosingServer server )
	{
		this.server = server;
	}
	
	@SuppressWarnings( "unused" )
	private final RemoteClosingServer server;

	public void _sessionNotify( Object event ) throws Exception
	{
		Log.report( "ImplClosingClient._sessionNotify", "event", event );
	}
}

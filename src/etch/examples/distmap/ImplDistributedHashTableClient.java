// Generated by:
// @EtchVersion@ (@EtchBuildTag@) / java @EtchVersion@ (@EtchBuildTag@)
// Fri Apr 11 09:02:37 CDT 2008

package etch.examples.distmap;


/**
 * Your custom implementation of BaseDistributedHashTableClient. Add methods here to provide
 * implementations of messages from the server.
 */
public class ImplDistributedHashTableClient extends BaseDistributedHashTableClient
{
	/**
	 * Constructs the ImplDistributedHashTableClient.
	 *
	 * @param server a connection to the server session. Use this to send a
	 * message to the server.
	 */
	public ImplDistributedHashTableClient( RemoteDistributedHashTableServer server )
	{
		this.server = server;
	}
	
	private final RemoteDistributedHashTableServer server;

	// TODO insert methods here to provide implementations of DistributedHashTableClient
	// messages from the server.
}
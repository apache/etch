package etch.examples.chat;

/**
 * Factory for a Listener to use to create new instances of ChatServer.
 */
public interface ChatServerFactory
{
	/**
	 * @param client an ChatClient instance for server implementation
	 * to use to call back to the client.
	 * @return an instance of ChatServer (e.g., ImplChatServer) to
	 * handle messages which arrive from the client.
	 * @throws Exception 
	 */
	public ChatServer newChatServer( ChatClient client ) throws Exception;
}

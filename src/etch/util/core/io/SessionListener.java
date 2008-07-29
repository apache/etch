package etch.util.core.io;

import java.net.Socket;

/**
 * Interface used to deliver sockets to the session from the listener.
 */
public interface SessionListener extends Session
{
	/**
	 * Delivers a socket to the session from the listener.
	 * @param socket
	 * @throws Exception
	 */
	public void sessionAccepted( Socket socket ) throws Exception;
}

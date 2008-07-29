package etch.util.core.io;

import etch.util.FlexBuffer;
import etch.util.core.Who;

/**
 * Interface used to deliver packets to the session from the transport.
 */
public interface SessionData extends Session
{
	/**
	 * Delivers data to the session from the transport.
	 * @param sender the sender of the data, for transports which allow
	 * multiple senders. This is who to return any response to.
	 * @param buf a FlexBuffer positioned at the data.
	 * @throws Exception
	 */
	public void sessionData( Who sender, FlexBuffer buf ) throws Exception;
}

package etch.util.core.io;

import etch.util.FlexBuffer;
import etch.util.core.Who;

/**
 * Interface used to deliver packets to the session from the transport.
 */
public interface SessionPacket extends Session
{
	/**
	 * Delivers a packet to the session from the transport.
	 * @param sender the sender of the packet, for transports which allow
	 * multiple senders. This is who to return any response to.
	 * @param buf a FlexBuffer positioned at the packet.
	 * @throws Exception
	 */
	public void sessionPacket( Who sender, FlexBuffer buf ) throws Exception;
}

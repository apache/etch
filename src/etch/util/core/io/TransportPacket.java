package etch.util.core.io;

import etch.util.FlexBuffer;
import etch.util.core.Who;

/**
 * Interface used to deliver packets to the transport from the session.
 */
public interface TransportPacket extends Transport<SessionPacket>
{
	/**
	 * Returns the size of the packet header for the transport.
	 * @return the size of the packet header for the transport.
	 */
	public int headerSize();
	
	/**
	 * Delivers the packet to the transport after adding the packet header.
	 * @param recipient specifies the recipient when there is the possibility
	 * of more than one.
	 * @param buf the FlexBuffer positioned on the packet, including space for
	 * the header.
	 * @throws Exception
	 */
	public void transportPacket( Who recipient, FlexBuffer buf ) throws Exception;
}

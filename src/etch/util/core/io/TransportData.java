package etch.util.core.io;

import etch.util.FlexBuffer;
import etch.util.core.Who;

/**
 * Interface used to deliver data to the transport from the session.
 */
public interface TransportData extends Transport<SessionData>
{
	/**
	 * Delivers the data to the transport.
	 * @param recipient specifies the recipient when there is the possibility
	 * of more than one.
	 * @param buf the FlexBuffer positioned on the data.
	 * @throws Exception
	 */
	public void transportData( Who recipient, FlexBuffer buf ) throws Exception;
}

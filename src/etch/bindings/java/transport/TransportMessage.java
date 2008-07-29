package etch.bindings.java.transport;

import etch.bindings.java.msg.Message;
import etch.util.core.Who;
import etch.util.core.io.Transport;

/**
 * Interface used to deliver messages to the transport from the session.
 */
public interface TransportMessage extends Transport<SessionMessage>
{
	/**
	 * Delivers the data to the transport.
	 * @param recipient specifies the recipient when there is the possibility
	 * of more than one.
	 * @param msg the Message.
	 * @throws Exception
	 */
	public void transportMessage( Who recipient, Message msg ) throws Exception;
}

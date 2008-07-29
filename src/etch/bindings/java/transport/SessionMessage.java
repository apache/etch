package etch.bindings.java.transport;

import etch.bindings.java.msg.Message;
import etch.util.core.Who;
import etch.util.core.io.Session;

/**
 * Interface used to deliver messages to the session from the transport.
 */
public interface SessionMessage extends Session
{
	/**
	 * Delivers data to the session from the transport.
	 * @param sender the sender of the message, for transports which allow
	 * multiple senders. This is who to return any response to.
	 * @param msg a Message.
	 * @return true if the message was consumed, false otherwise.
	 * @throws Exception
	 */
	public boolean sessionMessage( Who sender, Message msg ) throws Exception;
}

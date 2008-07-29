package etch.bindings.java.transport;

import etch.bindings.java.msg.Message;
import etch.util.core.Who;

/**
 * Event class used with sessionNotify to report unwanted messages.
 */
public class UnwantedMessage
{
	/**
	 * Constructs the UnwantedMessage.
	 * @param sender
	 * @param msg
	 */
	public UnwantedMessage( Who sender, Message msg )
	{
		this.sender = sender;
		this.msg = msg;
	}
	
	/** the sender of this unwanted message. */
	public final Who sender;
	
	/** the unwanted message. */
	public final Message msg;
	
	@Override
	public String toString()
	{
		return String.format( "Unwanted message from %s: %s", sender, msg );
	}
}

package etch.bindings.java.transport;

import etch.bindings.java.msg.Message;
import etch.bindings.java.support.DeliveryService;
import etch.bindings.java.support.Mailbox;
import etch.util.core.Who;

/**
 * Interface to the mailbox manager as needed by {@link DeliveryService} and
 * {@link Mailbox} implementors.
 */
public interface MailboxManager extends SessionMessage, TransportMessage
{
	/**
	 * Sends a message which begins a call after allocating a Mailbox to
	 * receive any responses.
	 * @param recipient specifies the recipient when there is the possibility
	 * of more than one.
	 * @param msg the message which begins the call.
	 * @return the mailbox which will receive responses to this call.
	 * @throws Exception
	 */
	public Mailbox transportCall( Who recipient, Message msg ) throws Exception;
	
	/**
	 * Removes the mailbox from the set of mailboxes receiving responses to
	 * messages.
	 * @param mb a mailbox as returned by {@link #transportCall(Who, Message)}.
	 */
	public void unregister( Mailbox mb );

	/**
	 * Re-delivers dead letter messages from a closed mailbox.
	 * @param sender 
	 * @param msg
	 * @throws Exception 
	 */
	public void redeliver( Who sender, Message msg ) throws Exception;
}

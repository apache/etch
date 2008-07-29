using System;
using System.Text;
using System.Collections.Generic;
using Etch.Msg;
using Etch.Support;

namespace Etch.Transport
{
    /// <summary>
    /// Interface used to send messages back to a message source.
    /// </summary>
    public interface MailboxMessageSource : MessageSource, Source
    {
        /// <summary>
        /// Sends a message to a message source. The message is encoded
        /// in some way (e.g., compressed binary format), and then delivered
        /// to a transport (e.g., packetized data stream).
        /// </summary>
        /// 
        /// <param name="recipient">recipient a transport specific opaque value which identifies
        /// the sender to a message handler and may be passed here to send a
        /// message back. Passing null means "use the default recipient".</param>
        /// <param name="msg">the message to send.</param>
        /// <param name="maxMessages">maximum number of messages which can be queued
        /// to the mailbox before it is full.</param>
        /// <param name="lifetime">time in ms until mailbox expires and is closed.
        /// 0 means don't expire the mailbox.</param>
        /// <returns>a mailbox created to receive replies to this message.</returns>
        /// Exception:
        ///     throws Exception
        Mailbox Message(Who recipient, Message msg, int maxMessages, 
            int lifetime);
    }
}

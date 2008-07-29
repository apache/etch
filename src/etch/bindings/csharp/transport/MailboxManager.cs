using System;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;
using Etch.Support;

namespace Etch.Transport
{
    public interface MailboxManager : TransportMessage,SessionMessage
    {
        /// <summary>
        /// Removes the mailbox from the set of mailbox receiving responses
        /// to messages
        /// </summary>
        /// <param name="mb"></param>
        
        void Unregister(Mailbox mb);

        /// <summary>
        /// Redelivers defunct messages from a closed mailbox.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="msg"></param>

        void Redeliver(Who sender, Message msg);

        Mailbox TransportCall(Who recipient, Message msg);
    }
}

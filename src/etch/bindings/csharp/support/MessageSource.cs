using System;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;

namespace Etch.Support
{
    /// <summary>
    /// A message source is used to model the origin of a message to a
    /// stub so that a reply might be sent.
    /// </summary>
    public interface MessageSource
    {
        /// <summary>
        /// Sends a message to a message source. The message is encoded
        /// in some way (e.g., compressed binary format or xml), and then delivered
        /// to a transport (e.g., packetized data stream or http response).
        /// </summary>
        /// <param name="recipient">a transport specific opaque value which identifies
        /// the sender of a message. Delivered by a message handler to the stub
        /// and may be passed here to send a message back to the original sender.
        /// Passing null means "use the default recipient".</param>
        /// <param name="msg">the message to send</param>
        void Messagex( Who recipient, Message msg );
    }
}

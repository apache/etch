using System;
using System.Text;
using System.Net.Sockets;
using System.Collections.Generic;

namespace Etch.Transport
{
    /// <summary>
    /// Interface used to report listener events
    /// </summary>
    public interface ListenerHandler : SourceHandler<Source>
    {
        /// <summary>
        /// Reports that the listener has accepted a connection.
        /// </summary>
        /// <param name="s">the socket of the connection.</param>
        /// Exception:
        ///     throws Exception

        void Accepted( Socket s );
    }
}

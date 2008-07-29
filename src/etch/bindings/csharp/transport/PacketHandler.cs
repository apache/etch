using System;
using System.Text;
using System.Collections.Generic;
using Etch.Support;

namespace Etch.Transport
{
    /// <summary>
    /// Interface used to deliver packets from a packet source.
    /// </summary>
    public interface PacketHandler : SourceHandler<PacketSource>
    {
        /// <summary>
        /// Delivers a packet from a packet source
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="buf"></param>
        /// Exception:
        ///     throws Exception
        void Packet( Who sender, FlexBuffer buf );
    }
}

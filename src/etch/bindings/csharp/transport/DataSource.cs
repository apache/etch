using System;
using System.Text;
using System.Collections.Generic;
using Etch.Support;

namespace Etch.Transport
{
    /// <summary>
    /// Interface used to model a stream source to a data recipient, which
    /// allows the recipient to send data to the peer.
    /// </summary>
    public interface DataSource : Source 
    {

        /// <summary>
        /// Delivers data to the peer via the data stream.
        /// </summary>
        /// <param name="recipient"></param>
        /// <param name="buf"></param>
        /// Exception:
        ///     throws Exception
        void Data( Who recipient, FlexBuffer buf );
    }
}

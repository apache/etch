using System;
using System.Text;
using System.Net;
using System.Collections.Generic;

namespace Etch.Transport
{
    /// <summary>
    /// Common interface to sources of data, packets, or messages. Models
    /// the various handler's view of a communication channel.
    /// </summary>
    /// <typeparam name="H">The handler type of this source.</typeparam>
    public interface Source : Transport // where H : SourceHandler<Source<Object>>
    {
        /// <summary>
        /// Get the handler of this source
        /// </summary>
        /// <returns></returns>
        Object GetHandler();

        /// <summary>
        /// Sets the handler of this source
        /// </summary>
        /// <param name="handler"></param>
        void SetHandler( Object handler );
    }
}

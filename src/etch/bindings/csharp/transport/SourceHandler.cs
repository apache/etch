using System;
using System.Text;
using System.Collections.Generic;
using Etch.Transport;

namespace Etch.Transport
{
    /// <summary>
    /// SourceHandler receives notification of source events.
    /// </summary>
    /// <typeparam name="S">event originator</typeparam>
    public interface SourceHandler<S> : Session where S : Source
    {
        /// <summary>
        /// 
        /// </summary>
        /// <returns>the source</returns>
        S GetSource();

        /// <summary>
        /// Sets the source
        /// </summary>
        /// <param name="src"></param>
        void SetSource( S src );
    }
}

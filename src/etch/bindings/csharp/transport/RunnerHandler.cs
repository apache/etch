using System;
using System.Text;
using System.Collections.Generic;

namespace Etch.Transport
{
    /// <summary>
    /// RunnerHandler receives notification of runner events.
    /// </summary>
    public interface RunnerHandler
    {
        /// <summary>
        /// Reports that the thread controlling the runner has started.
        /// </summary>
        void Started( );

        /// <summary>
        /// Reports that the thread controlling the runner has stopped.
        /// </summary>
        void Stopped();

        /// <summary>
        /// Reports that the thread controlling the runner has caught an
        /// exception.
        /// </summary>
        /// <param name="what">A short description of the origin</param>
        /// <param name="e">Exception that was caught</param>
        void Exception( String what, Exception e );

    }
}

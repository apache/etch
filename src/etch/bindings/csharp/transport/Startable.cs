using System;
using System.Text;
using System.Collections.Generic;

namespace Etch.Transport
{
    /// <summary>
    /// Description of Startable
    /// </summary>
    public interface Startable
    {
        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        void Start();

        /// Exception:
        ///     throws Exception
        void Stop();

        /// <summary>
        /// 
        /// </summary>
        /// <returns>true if started</returns>
        bool IsStarted();

        /// <summary>
        /// Check the startable is started
        /// </summary>
        void CheckIsStarted();
    
    }
}

using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Support
{
    public enum AsyncMode
    {
        /// <summary>
        /// synchronous mode
        /// </summary>
        NONE,
        /// <summary>
        ///  the operation is queued to a thread pool for execution 
        /// </summary>
        QUEUED,
        /// <summary>
        ///  the operation is executed by a newly allocated thread 
        /// </summary>
        FREE       
    }
}

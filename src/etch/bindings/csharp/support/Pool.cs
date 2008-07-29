using System;
using System.Text;
using System.Collections.Generic;

namespace Etch.Support
{
    /// <summary>
    /// Interface to a queued or free thread pool.
    /// </summary>
    public interface Pool
    {
        void Run( RunDelegate d1, ExceptionDelegate d2 );
    }

    /// <summary>
    /// Delegate for what to run
    /// </summary>
    public delegate void RunDelegate();

    /// <summary>
    /// Delegate that can throw an exception
    /// </summary>
    /// <param name="e"></param>
    public delegate void ExceptionDelegate( Exception e );
}

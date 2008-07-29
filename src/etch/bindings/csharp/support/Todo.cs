using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Support
{
    /// <summary>
    /// A TODO is used to perform a lightweight action
    /// </summary>
    public interface Todo
    {
        ///<summary>
        /// Performs the action
        /// </summary>
        /// <param name="mgr">the todo manager where this todo was queued</param>
        /// Exception:
        ///     throws Exception
        ///     
        void Doit(TodoManager mgr);

        /// <summary>
        /// Reports an exception that occurred while running the todo.
        /// </summary>
        /// <param name="mgr">the todo manager where this todo was queued.</param>
        /// <param name="e">the exception that the todo threw.</param>
        void Exception(TodoManager mgr, Exception e);
    }
}

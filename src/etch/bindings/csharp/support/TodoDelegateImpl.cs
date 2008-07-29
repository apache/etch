using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Support
{
    /// <summary>
    /// This class defines delegate for the methods
    /// defined in Todo Interface. 
    /// </summary>
    public class TodoDelegateImpl : Todo
    {
        private DoitDelegate doIt = null;
        private ExceptionToDoDelegate exc = null;



        public TodoDelegateImpl(DoitDelegate d, ExceptionToDoDelegate e)
        {
            this.doIt = d;
            this.exc = e;
        }

        public virtual void Doit(TodoManager mgr)
        {
            if (doIt == null)
                throw new Exception("Doit method not defined");

            doIt(mgr);
        }

        public virtual void Exception(TodoManager mgr, Exception e)
        {
            if (exc == null)
                throw new Exception("Exception method not defined");

            exc(mgr, e);

        }


        /// <summary>
        /// Performs the action
        /// </summary>
        /// <param name="mgr">the todo manager where this todo was queued</param>

        public delegate void DoitDelegate(TodoManager mgr);

        /// <summary>
        /// Reports an exception that occurred while running the todo.
        /// </summary>
        /// <param name="mgr">the todo manager where this todo was queued.</param>
        /// <param name="e">the exception that the todo threw.</param>
        /// 
        public delegate void ExceptionToDoDelegate(TodoManager mgr, Exception e);

    }
}

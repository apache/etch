using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Support
{
    /// <summary>
    /// A queued implementation of pool
    /// </summary>
    public class QueuedPool : Pool
    {
        /// <summary>
        /// Constructs the queued pool with a specified todo manager.
        /// </summary>
        /// <param name="mgr"></param>
        public QueuedPool( TodoManager mgr )
        {
            this.mgr = mgr;
        }

        /// <summary>
        /// Constructs the queued pool with a default todo manager.
        /// </summary>
        /// Exception: 
        ///     throws Exception
        ///     
        public QueuedPool() : this( TodoManager.GetTodoManager() )
        {}

        private TodoManager mgr;

        public void Run( RunDelegate d1, ExceptionDelegate d2 )
        {
            MyTodo inst = new MyTodo ( d1, d2 );
            mgr.Add( inst );
        }

        /// <summary>
        /// Implementation of Todo
        /// </summary>
        public class MyTodo : Todo
        {
            public MyTodo ( RunDelegate d1, ExceptionDelegate d2 )
            {
                this.d1 = d1;
                this.d2 = d2;
            }

            public void Doit( TodoManager mgr )
            {
                d1();
            }

            public void Exception( TodoManager mgr, Exception e ) 
            {
                d2( e );
            }

            private RunDelegate d1;
            private ExceptionDelegate d2;
        }
    }
}

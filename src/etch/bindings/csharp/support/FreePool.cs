using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Runtime.CompilerServices;

namespace Etch.Support
{
    /// <summary>
    /// A free implementation of pool
    /// </summary>
    public class FreePool : Pool
    {
        /// <summary>
        /// Constructs a FreePool with specified maxSize.
        /// </summary>
        /// <param name="maxSize">maximum number of free threads at one time
        /// after which reject requests. </param>
        public FreePool( int maxSize )
        {
            this.maxSize = maxSize;
            myThreads = new Thread[ maxSize ];
        }

        /// <summary>
        /// Constructs a FreePool with maxSize 50
        /// </summary>
        public FreePool()
            : this( 50 )
        { }

        private int maxSize;
        private int currentThreads = 0;
        Thread[] myThreads;
        
        void Run()
        {
            //donothing
        }

        private Boolean open = true;

        /// <summary>
        /// Closes the pool. This just marks the pool as being closed, it doesn't
        /// actually do anything to the currently running thread. But no more
        /// threads are allowed to start.
        /// </summary>
        public void Close()
        {
            open = false;
        }

        /// <summary>
        /// Joins each of the threads in this pool until there
        /// are none left. The pool will be closed first.
        /// Exception:
        ///     throws ThreadInterruptedException
        /// </summary>
        public void Join()
        {
            Close();
            for ( int i = 0; i < currentThreads; i++ )
            {
                if ( myThreads[ i ].IsAlive )
                    myThreads[ i ].Join();
            }
        }

        /// <summary>
        /// Finds the number of active threads in this pool
        /// </summary>
        /// <returns>the number of active threads</returns>
        public int ActiveCount()
        {
            int count = 0;
            for ( int i = 0; i < currentThreads; i++ )
            {
                if ( myThreads[ i ].IsAlive )
                    count++;
            }
            return count;
        }

        #region Pool Members

        [MethodImpl( MethodImplOptions.Synchronized )]
        public void Run( RunDelegate d1, ExceptionDelegate d2 )
        {
            if ( !open || ActiveCount() >= maxSize )
                throw new Exception( "free pool thread count exceeded or pool closed" );

            myThreads[ currentThreads ] = new Thread(
                new ThreadStart( delegate()
                                {
                                    try
                                    {
                                        d1();
                                    }
                                    catch ( Exception e )
                                    {
                                        d2( e );
                                    }
                                } ) );

            myThreads[ currentThreads ].Start();
            currentThreads++;
        }

        #endregion
    }
}

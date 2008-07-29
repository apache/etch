using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace Etch.Transport
{
    /// <summary>
    /// Generic implementation of Startable
    /// </summary>
    abstract public class AbstractStartable : Startable
    {
        [MethodImpl( MethodImplOptions.Synchronized )]
        public void Start()
        {
            if ( IsStarted() )
                throw new Exception( "Is already started" );
            
            try
            {
                SetStarted();
                Start0();
            }
            catch ( Exception e )
            {
                SetStopped();
                Stop0();
                throw e;
            }
        }

        [MethodImpl( MethodImplOptions.Synchronized )]
        public void Stop()
        {
            if ( !IsStarted() )
                throw new Exception( "!isStarted" );

            started = false;
            
            Stop0();
        }

        private bool started;

        private void SetStarted()
        {
            started = true;
        }

        protected void SetStopped()
        {
            started = false;
        }

        public bool IsStarted()
        {
            return started;
        }

        /// <summary>
        /// Check that the startable is started
        /// Exception:
        ///     ThreadInterruptedException if not started
        /// </summary>
        /// 
        protected void checkIfStarted()
        {
            if ( !IsStarted() )
                throw new System.Threading.ThreadInterruptedException( "is not started" );
        }

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        abstract protected void Start0();

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        abstract protected void Stop0();

    }
}

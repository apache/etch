using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Transport
{
    public class TransportConsts
    {
        #region Well-known queries, controls, and events.

        /// <summary>
        /// Transport control which starts the transport stack. The argument
        /// should be null
        /// </summary>
        public readonly static String START = "START";

        /// <summary>
        /// Transport control which starts the transport stack and waits
        /// for it to come up. The argument should be the integer number of
        /// milliseconds to wait before giving up.
        /// </summary>
        public readonly static String START_AND_WAIT_UP = "START_AND_WAIT_UP";

        /// <summary>
        /// Transport query which gets the current status of the transport
        /// stack. Returns a Boolean, either true or false.
        /// </summary>
        public readonly static String IS_UP = "IS_UP";

        /// <summary>
        /// Transport query which waits for the transport stack to come up.
        /// </summary>
        public class WaitUp
        {
            public readonly int _maxDelay;

            /// <summary>
            /// 
            /// </summary>
            /// <param name="maxDelay">delay in milliseconds</param>
            public WaitUp( int maxDelay )
            {
                _maxDelay = maxDelay;
            }
        }

        /// <summary>
        /// Transport control which stops the transport stack. The argument
        /// should be null, or a Boolean reset (true for instant close,
        /// false for a nicer, gentler close).
        /// </summary>
        public readonly static String STOP = "STOP";

        /// <summary>
        /// Transport control which stops the transport stack and waits
        /// for it to go down. The argument should be the integer number of
        /// milliseconds to wait before giving up.
        /// </summary>
        public readonly static String STOP_AND_WAIT_DOWN = "STOP_AND_WAIT_DOWN";

        /// <summary>
        /// Transport query which waits for the transport stack to go down.
        /// </summary>
        public class WaitDown
        {
            public readonly int _maxDelay;

            /// <summary>
            /// 
            /// </summary>
            /// <param name="maxDelay">delay in milliseconds</param>
            public WaitDown( int maxDelay )
            {
                _maxDelay = maxDelay;
            }
        }

        #endregion
    }
}

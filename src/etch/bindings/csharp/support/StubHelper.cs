using System;
using System.Collections.Generic;
using System.Text;


namespace Etch.Support
{
    public class StubHelper
    {
        public StubHelper(Delegate del, AsyncMode mode)
        {
            this.mode = mode;
            this.del = del;
        }

        ///
        /// The asynchronous receiver mode of this helper.
        ///
        public enum AsyncMode
        {
            NONE,
            QUEUED,
            FREE
        }

        /// <summary>Constructs a StubHelper which uses synchronous mode to dispatch
        /// method from message.</summary>
        public StubHelper() : this(AsyncMode.NONE)
        {
        }


        ///<summary>Constructs a StubHelper which uses specified mode to
        ///dispatch method from message.</summary>
        ///<param name="mode">param mode</param>
        public StubHelper(AsyncMode mode)
        {
            this.mode = mode;
        }

        private AsyncMode mode;
        private Delegate del;

        ///<returns>true if the mode is AsyncMode.QUEUED.</returns>
        public bool IsQueued()
        {
            return mode == AsyncMode.QUEUED;
        }

        ///<returns>true if the mode is AsyncMode.FREE.</returns>
        public bool IsFree()
        {
            return mode == AsyncMode.FREE;
        }

        public Delegate GetDelegate()
        {
            return del;
        }
    }
}

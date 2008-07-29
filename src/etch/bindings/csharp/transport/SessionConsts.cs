using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Transport
{
    public class SessionConsts
    {

        #region WELL KNOWN EVENTS

        /// <summary>
        /// Session event reporting the transport is up.
        /// </summary>
        public readonly static String UP = "UP";

        /// <summary>
        /// Session event reporting the transport is down.
        /// </summary>
        public readonly static String DOWN = "DOWN";

        #endregion
    }
}

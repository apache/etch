using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.CompilerServices;

namespace Etch.Support
{
    /// <summary>
    ///  A safe integer counter
    /// </summary>
    public class IntCounter
    {
        /// <summary>
        /// Adjusts the value of the counter by k
        /// </summary>
        /// <param name="k">the adjustment to the counter.</param>
        /// <returns>the new value of the counter.</returns>
        /// 
        [MethodImpl( MethodImplOptions.Synchronized )]
        public int Adjust( int k )
        {
            value += k;
            return value;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the value of the counter</returns>
        public int Get()
        {
            return value;
        }

        private int value;
    }
}

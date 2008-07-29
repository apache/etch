using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using Etch.Transport;

namespace Etch.Util
{
    /// <summary>
    /// Synchronously generates long id numbers. 
    /// </summary>
    public sealed class IdGenerator 
    {
        /// <summary>
        /// Constructs the IdGenerator with the default starting point of 1
        /// and the default stride of 1.
        /// </summary>
        public IdGenerator() : this( 1, 1 )
        {
            
        }

        /// <summary>
        /// Constructs the IdGenerator with the specified starting point
        /// and the default stride of 1.
        /// </summary>
        /// <param name="nextId"></param>
        public IdGenerator( long nextId ) : this( nextId, 1 )
        {
            
        }

        /// <summary>
        /// Constructs the IdGenerator with the specified starting point
        /// and the specified stride.
        /// </summary>
        /// <param name="nextId"></param>
        /// <param name="stride"></param>
        public IdGenerator( long nextId, int stride )
        {
            if ( stride <= 0 )
                throw new ArgumentException( "stride <= 0" );

            this.nextId = nextId;
            this.stride = stride;
        }

        private long nextId;
	
	    private readonly int stride;

        /// <summary>
        /// 
        /// </summary>
        /// <returns>The next Id in sequence</returns>
        [MethodImpl(MethodImplOptions.Synchronized)]
        public long Next()
        {
            long id = nextId;
            nextId += stride;
            return id;
        }

    }
}

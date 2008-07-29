using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validate values put into messages
    /// </summary>
    abstract public class Validator
    {
        /// <summary>
        /// The maximum number of dimensions for arrays
        /// </summary>
        public readonly static int MAX_NDIMS = 9;

        /// <summary>
        /// The maximum number of validators to cache per type.
        /// </summary>
        protected static readonly int MAX_CACHED = 4;

        /// <summary>
        /// 
        /// </summary>
        /// <returns>for an array type, return a validator for an element of
        /// the array (which might be a scalar or another array element).</returns>
        abstract public Validator ElementValidator();

        /// <summary>
        /// Checks the value for being valid. If the value is valid
        /// by this validator, return true.
        /// </summary>
        /// <param name="value">the value to be validated.</param>
        /// <returns>true if the value is valid by any validator in the
        /// chain, or false if all reject it.</returns>
        abstract public bool Validate( Object value );

        /// <summary>
        /// 
        /// </summary>
        /// <param name="value"></param>
        /// <returns>the appropriate type code for this value or null
        /// if this validator does not recognise the value.</returns>
        abstract public sbyte? CheckValue( Object value );
    }
}

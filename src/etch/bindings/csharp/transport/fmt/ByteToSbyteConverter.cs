using System;
using System.Text;
using System.Collections.Generic;

namespace Etch.Transport.Fmt
{
    /// <summary>
    /// Class that provides methods for : 
    /// 1) Converting a single value from byte to sbyte.
    /// 2) Converting a single value for sbyte to byte.
    /// 3) Converting an array of values of the 2 above.
    /// </summary>
    public static class ByteConverter
    {
        /// <summary>
        /// Converts a byte value to an sbyte value.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static sbyte ByteToSbyte( byte value )
        {
            sbyte convertedValue = 0;

            if ( value > 127 )
                convertedValue = ( sbyte ) ( 127 - value );

            return convertedValue;
        }

        public static byte SbyteToByte( sbyte value )
        {
            byte convertedValue = 0;

            if ( value < 0 )
                convertedValue = ( byte ) ( 127 - value );

            return convertedValue;
        }
    }
}

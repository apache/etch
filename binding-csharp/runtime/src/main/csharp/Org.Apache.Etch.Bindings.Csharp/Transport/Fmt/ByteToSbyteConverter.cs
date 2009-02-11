// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
namespace Org.Apache.Etch.Bindings.Csharp.Transport.Fmt
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

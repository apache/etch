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
using System;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// Miscellaneous string utility functions
    /// </summary>
    /// 
    public class StringUtil
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="s"></param>
        /// <param name="c"></param>
        /// <returns>an array x of two elements containing s split in two by the
        /// leftmost c. x[0] is the chars before c, x[1] is the chars after c.
        /// If c is not in s, returns null.</returns>
        /// 
        public static String[] LeftSplit( String s, char c )
        {
            if ( s == null )
                throw new ArgumentNullException( "Passed string is null" );

            int i = s.IndexOf( c );
            if ( i < 0 )
                return null;

            return new String[] { s.Substring( 0, i ), s.Substring( i+1 ) };
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="s"></param>
        /// <param name="c"></param>
        /// <returns>an array x of two elements containing s split in two by the
        /// rightmost c. x[0] is the chars before c, x[1] is the chars after c.
        /// If c is not in s, returns null.</returns>
        public static String[] RightSplit( String s, Char c )
        {
            if ( s == null )
                throw new ArgumentNullException( "Passed string is null" );

            int i = s.LastIndexOf( c );
            if ( i < 0 )
                return null;

            return new String[] { s.Substring( 0, i ), s.Substring( i+1 ) };
        }

        /// <summary>
        /// Translates an int into a hex char.
        /// </summary>
        /// <param name="i">i an int (0-15)</param>
        /// <returns>a hex char (0-9, a-f).</returns>
        /// 
        public static char ToHex( int i )
        {
            if ( i >= 0 && i <= 9 )
                return ( char ) ( '0' + i );
            if ( i >= 10 && i <= 15 )
                return ( char ) ( 'a' + i - 10 );
            throw new ArgumentException( "bad hex digit selector "+i );
        }

        /// <summary>
        /// Translates a hex char into an int.
        /// </summary>
        /// <param name="c">a hex char (0-9, A-F, a-f).</param>
        /// <returns>an int (0-15).</returns>
        /// 
        public static int FromHex( char c )
        {
            if ( c >= '0' && c <= '9' )
                return c - '0';
            if ( c >= 'A' && c <= 'F' )
                return c - 'a' + 10;
            if ( c >= 'a' && c <= 'f' )
                return c - 'a' + 10;
            throw new ArgumentException( "bad hex char "+c );
        }

        public static bool Eq(string a, string b)
        {
            if (ReferenceEquals(a, b))
                return true;
            if (a == null || b == null)
                return false;
            return a.Equals(b);
        }

        public static bool EqIgnoreCase(string a, string b)
        {
            if (ReferenceEquals(a, b))
                return true;
            if (a == null || b == null)
                return false;
            return a.Equals(b, StringComparison.CurrentCultureIgnoreCase);
        }
    }
}

using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Util
{
    /// <summary>
    /// A hashmap which is from string to integer
    /// </summary>
    public class StrStrHashMap : Dictionary<string, string>
    {
        public StrStrHashMap()
        {
            // nothing to do .
        }

        /// <summary>
        /// Constructs a StrStrHashMap initialized from another
        /// </summary>
        /// <param name="other"></param>
        public StrStrHashMap( StrStrHashMap other )
            : base( other )
        { }

    }
}

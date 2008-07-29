using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Util
{
    /// <summary>
    /// A hashmap which is from string to integer
    /// </summary>
    public class StrIntHashMap : Dictionary<string, int?>
    {
        public StrIntHashMap()
        {
            // nothing to do .
        }

        /// <summary>
        /// Constructs a StrIntHashMap initialized from another
        /// </summary>
        /// <param name="other"></param>
        public StrIntHashMap( StrIntHashMap other )
            : base( other )
        { }

    }
}

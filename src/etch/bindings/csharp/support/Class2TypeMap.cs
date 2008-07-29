using System;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;

namespace Etch.Support
{
    public class Class2TypeMap
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="c"></param>
        /// <returns>the Type for the specified class, or null</returns>
        public XType Get( Type c )
        {
            try
            {
                return c2t[ c ];
            }
            catch ( KeyNotFoundException )
            {
                return null;
            }
        }

        /// <summary>
        /// Adds a map entry from c to t
        /// </summary>
        /// <param name="c"></param>
        /// <param name="t"></param>
        public void Add( Type c, XType t )
        {
            if ( locked )
                throw new Exception( "locked" );

            XType x;
            try
            {
                x = c2t[ c ];
            }
            catch ( KeyNotFoundException )
            {
                x = null;
            }

            if ( x != null )
            {
                if ( !x.Equals( t ) )
                    throw new ArgumentException( String.Format( "type {0} : class {1} is already mapped to type {2}", t, c, x ) );
                return;
            }
            c2t.Add( c, t );
        }

        /// <summary>
        /// Adds all the mappings from other to this
        /// </summary>
        /// <param name="other"></param>
        public void AddAll( Class2TypeMap other )
        {
            if ( locked )
                throw new Exception( "locked" );

            foreach ( KeyValuePair<Type, XType> me in other.c2t )
            {
                Add( me.Key, me.Value );
            }
        }

        /// <summary>
        /// Locks the map, disallowing any more changes
        /// </summary>
        public void Lock()
        {
            locked = true;
        }

        private readonly Dictionary<Type, XType> c2t = new Dictionary<Type, XType>();

        private Boolean locked;
    }
}

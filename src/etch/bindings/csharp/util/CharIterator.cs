using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Util
{
    public class CharIterator 
    {
        private readonly String _s;
        private readonly int _n;
        private int _i = -1;

        public CharIterator( String s )
        {
            if ( s == null )
                throw new ArgumentNullException( "String to iterate over shouldn't be null" );

            _s = s;
            _n = s.Length;
        }

        public Boolean MoveNext()
        {
            if ( _i < (_n-1) )
            {
                _i++;
                return true;
            }
            else
                return false;
        }

        public Char Current
        {
            get
            {
                return _s[ _i ];
            }
        }
    }
}

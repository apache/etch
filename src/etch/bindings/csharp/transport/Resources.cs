using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Transport
{
    public class Resources : Dictionary<String, Object>
    {
        public Resources()
        {
            // ignore
        }

        public Resources( Resources r )
            : base( r )
        { }

        public Object Get( String key )
        {
            try
            {
                return this[ key ];
            }
            catch ( KeyNotFoundException )
            {
                return null;
            }
        }
    }
}

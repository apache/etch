using System;
using System.Text;
using System.Collections.Generic;

namespace Etch.Support
{
    public class _Etch_AuthException : _Etch_RuntimeException
    {
        public _Etch_AuthException()
        {
            // dont initialize anything
        }

        public _Etch_AuthException( String msg ) : base( msg )
        {
        }
    }
}

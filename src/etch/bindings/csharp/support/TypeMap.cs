using System;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;

namespace Etch.Support
{
    /// <summary>
    /// TypeMap is an IdNameMap for Type
    /// </summary>
    public class TypeMap : IdNameMap<XType>
    {
        public override XType MakeNew( string name )
        {
            return new XType( name );
        }
    }
}

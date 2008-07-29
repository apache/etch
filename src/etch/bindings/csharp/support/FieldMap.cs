using System;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;

namespace Etch.Support
{
    /// <summary>
    /// FieldMap is an IdNameMap for Field
    /// </summary>
    public class FieldMap : IdNameMap<Field>
    {
        public override Field MakeNew( string name )
        {
            return new Field( name );
        }
    }
}

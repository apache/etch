using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Transport.Fmt.Binary
 
{
    public class BinaryTaggedDataFormatFactory : FormatFactory
    {
        
        public BinaryTaggedDataFormatFactory() {}
        
        public override TaggedDataInput NewTaggedDataInput( Etch.Msg.ValueFactory vf, string uri )
        {
            return new Etch.Transport.Fmt.Binary.BinaryTaggedDataInput( vf, uri );
        }

        public override TaggedDataOutput NewTaggedDataOutput( Etch.Msg.ValueFactory vf, string uri )
        {
            return new Etch.Transport.Fmt.Binary.BinaryTaggedDataOutput( vf, uri );
        }
    }
}

using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Transport
{
    public class BinaryTaggedDataFormatFactory : FormatFactory
    {
        public override TaggedDataInput NewTaggedDataInput( Etch.Msg.ValueFactory vf, Etch.Util.URL uri )
        {
            return new Etch.Transport.Fmt.Binary.BinaryTaggedDataInput( vf, uri );
        }

        public override TaggedDataOutput NewTaggedDataOutput( Etch.Msg.ValueFactory vf, Etch.Util.URL uri )
        {
            return new Etch.Transport.Fmt.Binary.BinaryTaggedDataOutput( vf, uri );
        }
    }
}

using System;
using System.Collections.Generic;
using System.Text;
using Etch.Support;
using Etch.Msg;

namespace Etch.Util
{
    /// <summary>
    /// Serialize a DateTime into our network standard format, which
    /// is the unix standard of milliseconds since jan 1, 1970 utc.
    /// </summary>
    public class DateSerializer : ImportExportHelper
    {
        private readonly static String FIELD_NAME = "dateTime";

        private readonly XType type;
        private readonly Field field;

        public DateSerializer( XType type, Field field )
        {
            this.type = type;
            this.field = field;
        }
        
        /// <summary>
        /// Defines custom fields in the value factory so that the importer
        /// can find them
        /// </summary>
        /// <param name="type"></param>
        /// <param name="class2type"></param>
        public static void Init( XType type, Class2TypeMap class2type )
        {
            Field field = type.GetField( FIELD_NAME );
            class2type.Add( typeof( DateTime ), type );
            type.SetComponentType( typeof( DateTime ) );
            type.SetImportExportHelper( new DateSerializer( type, field ) );
            type.PutValidator( field, Validator_long.Get( 0 ) );
            type.Lock();
        }

        public override Object ImportValue( StructValue sv )
        {
            long ms = (long)sv.Get(field);
            return new DateTime(epoch + ms * TICKS_PER_MS, DateTimeKind.Utc).ToLocalTime();
        }

        public override StructValue ExportValue( Object value )
        {
            long ms = (((DateTime)value).ToUniversalTime().Ticks - epoch) / TICKS_PER_MS;
            StructValue sv = new StructValue(type);
            sv.Add(field, ms);
            return sv;
        }

        private const long TICKS_PER_MS = 10000;

        private static readonly long epoch =
            new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc).Ticks;
    }
}

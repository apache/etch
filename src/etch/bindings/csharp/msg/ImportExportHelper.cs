using System;
using System.Collections.Generic;
using System.Text;


namespace Etch.Msg
{
    /// <summary>
    /// class used to implement ValueFactory based import / export code
    /// for service defined types.
    /// </summary>
    public class ImportExportHelper
    {
        ExportValueDelegate _ev;
        ImportValueDelegate _iv;

        public ImportExportHelper()
        {
            _ev = null;
            _iv = null;
        }

        public ImportExportHelper( ExportValueDelegate ev, ImportValueDelegate iv )
        {
            _ev = ev;
            _iv = iv;
        }

        /// <summary>
        /// Exports a value by creating an equivalent StructValue.
        /// </summary>
        /// <param name="value">a service defined type to export.</param>
        /// <returns>a StructValue representing the exported value.</returns>
        public virtual StructValue ExportValue( Object value )
        {
            if ( _ev == null )
                throw new Exception( "method not defined" );

            return _ev( value );
        }

        /// <summary>
        /// Imports a value from an equivalent StructValue.
        /// </summary>
        /// <param name="sv">a StructValue representing the exported value.</param>
        /// <returns>a service defined type.</returns>
        public virtual Object ImportValue( StructValue sv )
        {
            if ( _iv == null )
                throw new Exception( "method not defined" );

            return _iv( sv );
        }

        /// <summary>
        /// Exports a value by creating an equivalent StructValue.
        /// </summary>
        /// <param name="value">a service defined type to export.</param>
        /// <returns>a StructValue representing the exported value.</returns>
        public delegate StructValue ExportValueDelegate( Object value );

        /// <summary>
        /// Imports a value from an equivalent StructValue.
        /// </summary>
        /// <param name="sv">a StructValue representing the exported value.</param>
        /// <returns>a service defined type.</returns>
        public delegate Object ImportValueDelegate( StructValue sv );

    }
}

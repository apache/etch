using System;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;
using Etch.Util;

namespace Etch.Transport
{
    public abstract class FormatFactory
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="vf"></param>
        /// <param name="uri"></param>
        /// <returns>the tagged data input with the specified value factory and
        /// initialized by terms from the uri.</returns>
        abstract public TaggedDataInput NewTaggedDataInput( ValueFactory vf, URL uri );

        /// <summary>
        /// 
        /// </summary>
        /// <param name="vf"></param>
        /// <param name="uri"></param>
        /// <returns>the tagged data output with the specified value factory and
        /// initialized by terms from the uri.</returns>
        abstract public TaggedDataOutput NewTaggedDataOutput( ValueFactory vf, URL uri );

        private readonly static Dictionary<String, FormatFactory> formatFactories = new Dictionary<String, FormatFactory>();

        /// <summary>
        /// Gets the named format factory.
        /// </summary>
        /// <param name="name">the name of a configured format factory.</param>
        /// <returns>the named format factory, or null if it isn't defined.</returns>
        public static FormatFactory Get( String name )
        {
            return formatFactories[ name ];
        }

        /// <summary>
        /// Puts a named format factory.
        /// </summary>
        /// <param name="name">the uri scheme of this format factory.</param>
        /// <param name="ff">the format factory to associate with the name.</param>
        public static void Put( String name, FormatFactory ff )
        {
            formatFactories.Add( name, ff );
        }

        static FormatFactory()
        {
            Put( "binary", new BinaryTaggedDataFormatFactory() );
        }
    }
}

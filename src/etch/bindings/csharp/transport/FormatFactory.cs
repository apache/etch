using System;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;
using Etch.Transport.Fmt.Binary;
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
        abstract public TaggedDataInput NewTaggedDataInput( ValueFactory vf, string uri );

        /// <summary>
        /// 
        /// </summary>
        /// <param name="vf"></param>
        /// <param name="uri"></param>
        /// <returns>the tagged data output with the specified value factory and
        /// initialized by terms from the uri.</returns>
        abstract public TaggedDataOutput NewTaggedDataOutput( ValueFactory vf, string uri );

        private readonly static Dictionary<String, Object> formatFactories = new Dictionary<String, Object>();

        /// <summary>
        /// Gets the named format factory.
        /// </summary>
        /// <param name="name">the name of a configured format factory.</param>
        /// <returns>the named format factory, or null if it isn't defined.</returns>
        public static FormatFactory Get( String name )
        {
            Object factory = formatFactories[name];

            if (factory == null)
                return null;
            if (factory is string)
            {
                try
                {
                
                    Type c = Type.GetType((string) factory);
                    factory = Activator.CreateInstance(c);
                    if (!(factory is FormatFactory))
                        throw new ArgumentException(
                            String.Format(" Instance for format name {0} does not implement FormatFactory {1}", name, factory));
                    
                 //   formatFactories.Add(name,factory);
                   
                }
                catch(Exception e)
                {
                    throw new ArgumentException(" Could not create instance of format name " + name,e);
                }
            }
           return (FormatFactory) factory;
        }

        /// <summary>
        /// Puts a named format factory.
        /// </summary>
        /// <param name="name">the uri scheme of this format factory.</param>
        /// <param name="factory">thefully qualified class name or instance
        /// of the FormatFactory to associate with the name</param>
        public static void Put( String name, Object factory )
        {
            formatFactories.Add( name, factory );
        }

        static FormatFactory()
        {
            Put("binary", "Etch.Transport.Fmt.Binary.BinaryTaggedDataFormatFactory");
        }
    }
}

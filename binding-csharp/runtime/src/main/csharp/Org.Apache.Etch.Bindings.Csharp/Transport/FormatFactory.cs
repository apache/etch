// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using System.Collections.Generic;
using Org.Apache.Etch.Bindings.Csharp.Msg;

namespace Org.Apache.Etch.Bindings.Csharp.Transport
{
    public abstract class FormatFactory
    {
	    public const String BINARY = "binary";
    	
	    public const String XML = "xml";
	
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
            Put(BINARY, "Org.Apache.Etch.Bindings.Csharp.Transport.Fmt.Binary.BinaryTaggedDataFormatFactory");
        }
    }
}

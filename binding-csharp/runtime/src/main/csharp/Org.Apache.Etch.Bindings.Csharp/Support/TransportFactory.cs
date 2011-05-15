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
using System.IO;
using System.Reflection;
using Org.Apache.Etch.Bindings.Csharp.Transport;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    /// <summary>
    /// Interface to transport factory
    /// </summary>
	public abstract class TransportFactory
	{
        /// <summary>
        /// Constructs a new Transport stack topped by a TransportMessage
        /// which is used by the remote service implementations to send
        /// messages.
        /// </summary>
        /// <param name="uri">transport configuration parameters.</param>
        /// <param name="resources">additional resources needed by the stack.</param>
        /// <returns>the TransportMessage topping the transport stack.</returns>
        protected abstract TransportMessage NewTransport(string uri, Resources resources);

        /// <summary>
        /// Constructs a new Transport stack topped by a TransportMessage
        /// which is used by the remote service implementations to send
        /// messages.
        /// </summary>
        /// <param name="uri">transport configuration parameters.</param>
        /// <param name="resources">additional resources needed by the stack.</param>
        /// <returns>the TransportMessage topping the transport stack.</returns>
        public static TransportMessage GetTransport(string uri, Resources resources) 
	    {
            URL u = new URL(uri);
            TransportFactory f = GetTransportFactory(u.Scheme);
            return f.NewTransport(uri, resources);
	    }

        /// <summary>
        /// Constructs a new Transport Listener which is used to construct
        /// server sessions.
        /// </summary>
        /// <param name="uri">listener configuration parameters.</param>
        /// <param name="resources">additional resources needed by the listener.</param>
        /// <returns>an out-of-band source which may be used to control the listener.</returns>
        protected abstract Transport<ServerFactory> NewListener(string uri, Resources resources);

        /// <summary>
        /// Constructs a new Transport Listener which is used to construct
        /// server sessions.
        /// </summary>
        /// <param name="uri">listener configuration parameters.</param>
        /// <param name="resources">additional resources needed by the listener.</param>
        /// <returns>an out-of-band source which may be used to control the listener.</returns>
        public static Transport<ServerFactory> GetListener(string uri, Resources resources) 
	    {
            URL u = new URL(uri);
            TransportFactory f = GetTransportFactory(u.Scheme);
            return f.NewListener(uri, resources);
	    }

        /// <summary>
        /// Adds any message filters specified on the uri. They are added in order
        /// from transport to session. The first filter is the session for Messagizer,
        /// the second is the session for the first, etc. The last filter added is
        /// returned, and becomes the TransportMessage for what follows.
        /// </summary>
        /// <param name="transport"></param>
        /// <param name="uri"></param>
        /// <param name="resources"></param>
        /// <returns>the most recently added filter</returns>
        protected TransportMessage AddFilters(TransportMessage transport, URL uri, Resources resources)
        {
            string s = uri.GetTerm(FILTER);
            if (s == null || s.Length == 0)
                return transport;

            StringTokenizer st = new StringTokenizer(s, ",:;| \t\r\n");
            while (st.HasMoreTokens())
            {
                string t = st.NextToken();
                transport = AddFilter(transport, t, uri, resources);
            }

            return transport;
        }

        /// <summary>
        /// Query term on the transport uri which defines a set of filters which
        /// process messages as they move up and down the transport stack. Filter
        /// names are separated by one or more of these characters: ",:;| \t\r\n".
        /// 
        /// Usage example: tcp://host:port?filter=KeepAlive,PwAuth
        /// </summary>
        public const string FILTER = "filter";

        private TransportMessage AddFilter(TransportMessage transport, string name,
            URL uri, Resources resources)
        {
            string typeName = GetFilter(name);

            if (typeName == null)
                throw new ArgumentException(
                    string.Format( "Class '{0}' for filter name '{1}' not defined", typeName, name ) );

            Type type = Type.GetType(typeName);

            ConstructorInfo c = type.GetConstructor(FILTER_PARAMS);

            return (TransportMessage) c.Invoke(new object[] { transport, uri, resources });
        }

        private readonly static Type[] FILTER_PARAMS = { typeof(TransportMessage), typeof(URL), typeof(Resources) };

        /// <summary>
        /// Gets the fully qualified type name of the filter.
        /// </summary>
        /// <param name="name"></param>
        /// <returns>the fully qualified type name of the filter.</returns>
        public static string GetFilter(string name)
        {
            lock (filters)
            {
                string value;
                return filters.TryGetValue(name, out value) ? value : null;
            }
        }

        /// <summary>
        /// Sets the fully qualified type name of the filter.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="typeName"></param>
        public static void DefineFilter(string name, string typeName)
        {
            lock (filters)
            {
                filters.Add(name, typeName);
            }
        }

        private static readonly Dictionary<string, string> filters =
            new Dictionary<string, string>();

        /// <summary>
        /// Gets the named transport factory.
        /// </summary>
        /// <param name="name">the name of a configured transport factory.</param>
        /// <returns>the named transport factory.</returns>
        private static TransportFactory GetTransportFactory( string name )
        {
            object tfspec = Get(name);

            if (tfspec == null)
                throw new IOException("transport factory scheme '" + name + "' unknown");

            if (tfspec is string)
                tfspec = Type.GetType((string)tfspec);

            if (tfspec is Type)
            {
                ConstructorInfo c = ((Type)tfspec).GetConstructor(FACTORY_PARAMS);
                tfspec = c.Invoke(FACTORY_ARGS);
            }

            if (tfspec is TransportFactory)
                return (TransportFactory)tfspec;

            throw new ArgumentException(String.Format(
                "Cannot create an instance of TransportFactory from transport factory name '{0}'", name));
        }

        private readonly static Type[] FACTORY_PARAMS = { };

        private readonly static object[] FACTORY_ARGS = { };

        public static object Get(string name)
        {
            lock (transportFactories)
            {
                object value;
                return transportFactories.TryGetValue(name, out value) ? value : null;
            }
        }


        /// <summary>
        /// Puts a named transport factory.
        /// </summary>
        /// <param name="name">the uri scheme of this transport factory.</param>
        /// <param name="tfspec">the transport factory spec to associate with the
        /// name. This can be an instance of TransportFactory or the name of a subclass
        /// of TransportFactory to instantiate.</param>
        public static void Define( string name, object tfspec )
        {
            lock (transportFactories)
            {
                transportFactories.Add(name, tfspec);
            }
        }

        private readonly static Dictionary<string, object> transportFactories =
            new Dictionary<string, object>();

        static TransportFactory()
        {
            Define("tcp", new TcpTransportFactory(false));
            Define("tls", new TcpTransportFactory(true));
            Define("udp", new UdpTransportFactory());

            DefineFilter("KeepAlive", "Org.Apache.Etch.Bindings.Csharp.Transport.Filter.KeepAlive");
            DefineFilter("PwAuth", "Org.Apache.Etch.Bindings.Csharp.Transport.Filter.PwAuth");
            DefineFilter("Logger", "Org.Apache.Etch.Bindings.Csharp.Transport.Filter.Logger");
        }
	}
}

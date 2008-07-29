using System;
using System.Collections.Generic;
using System.Text;
using Etch.Util;
using Etch.Support;

namespace Etch.Transport
{
    /// <summary>
    /// Interface to transport factory
    /// </summary>
	public abstract class TransportFactory
	{
        /// <summary>
        /// Constructs a new Transport stack topped by a DeliveryService
        /// which is used by the remote service implementations to send
        /// messages and make calls.
        /// </summary>
        /// <param name="uri">transport configuration parameters.</param>
        /// <param name="resources">additional resources needed by the stack.</param>
        /// <returns>the DeliveryService topping the transport stack.</returns>
        public abstract DefaultDeliveryService NewTransport( URL uri, Resources resources );

        /// <summary>
        /// Constructs a new Transport Listener which is used to construct
        /// server sessions.
        /// </summary>
        /// <param name="uri">listener configuration parameters.</param>
        /// <param name="resources">additional resources needed by the listener.</param>
        /// <param name="factory">constructs the actual service class instances.</param>
        /// <returns>an out-of-band source which may be used to control the listener.</returns>
        public abstract Transport NewListener( URL uri, Resources resources, ServerFactory factory );

        private readonly static Dictionary<String, TransportFactory> transportFactories = 
            new Dictionary<string, TransportFactory>();

        /// <summary>
        /// Gets the named transport factory.
        /// </summary>
        /// <param name="name">the name of a configured transport factory.</param>
        /// <returns>the named transport factory.</returns>
        public static TransportFactory Get( String name )
        {
            TransportFactory tf;
            lock ( ((System.Collections.ICollection)transportFactories).SyncRoot )
            {
                tf = transportFactories[ name ];
            }
            if ( tf == null )
                throw new Exception( "transport factory scheme '"+name+"' unknown" );
            return tf;
        }

        /// <summary>
        /// Puts a named transport factory.
        /// </summary>
        /// <param name="name">the uri scheme of this transport factory.</param>
        /// <param name="tf">the transport factory to associate with the name.</param>
        public static void Add( String name, TransportFactory tf )
        {
            lock ( ( ( System.Collections.ICollection ) transportFactories ).SyncRoot )
            {
                transportFactories.Add( name, tf );
            }
        }

        static TransportFactory()
        {
            Add( "tcp", new TcpTransportFactory() );
        }
	}
}

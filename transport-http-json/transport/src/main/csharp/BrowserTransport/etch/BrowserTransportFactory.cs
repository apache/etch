// $Id$
// 
// Copyright 2007-2008 Cisco Systems Inc.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.


using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using Etch.Support;
using Etch.Util;
using Etch.Msg;

using BasicHttpListener;

namespace Etch.Transport
{
    public class BrowserTransportFactory : TransportFactory
    {
        
        /// <summary>
        ///     Invoked everytime a new client arrives.
        /// </summary>
        /// <param name="uri"></param>
        /// <param name="resources"></param>
        /// <returns></returns>
        protected override DeliveryService NewTransport(string uri, Resources resources)
        {
            HttpSession session = (HttpSession) resources.Get( "HTTPSESSION" );
            
            // Create an adapter which acts as a conduit from native Etch message formats and the HTTP server implementation
            BrowserTransporter transporter = new BrowserTransporter( session, uri, resources );

            PlainMailboxManager manager = new PlainMailboxManager( transporter, uri, resources);

            DefaultDeliveryService deliveryService = new DefaultDeliveryService(manager, uri, resources);

            return deliveryService;
        }

        /// <summary>
        ///     Invoked when the server creates a new listener.
        /// </summary>
        /// <param name="uri">The full URI associated with the server, i.e., browser://0.0.0.0:82/base</param>
        /// <param name="resources"></param>
        /// <param name="factory"></param>
        /// <returns>An instance of the HTTP transport</returns>
        protected override Transport<ServerFactory> NewListener( string uri, Resources resources, ServerFactory factory )
        {
            Uri parsedUri = new Uri(uri);

            HttpServerImpl server = new HttpServerImpl( false, (short) parsedUri.Port );

            BrowserListenerHandler clientHandler = new BrowserListenerHandler( this, server, uri, resources, factory );

            server.SetSession( clientHandler );

            return clientHandler;
        }

        public class BrowserListenerHandler : Transport<ServerFactory>, HttpServerHandler
        {
            private Transport<HttpServerHandler> transport;
            private BrowserTransportFactory transportFactory;
            private Resources resources;
            private string uri;
            private ServerFactory factory;

            public BrowserListenerHandler( BrowserTransportFactory transportFactory, Transport<HttpServerHandler> transport, string uri, Resources resources, ServerFactory factory )
            {
                this.transport = transport;
                this.transportFactory = transportFactory;
                this.resources = resources;
                this.uri = uri;
                this.factory = factory;

                this.transport.SetSession( this );
            }

            #region HttpServerHandler Members

            public void NewClient(HttpSession session)
            {
                Console.WriteLine( "Constructing resources" );
                Resources newClientResources = new Resources( this.resources );

                Console.WriteLine( "Adding HTTPSESSION" );

                newClientResources.Add( "HTTPSESSION", session );

                Console.WriteLine( "Getting new value factory" );
                ValueFactory vf = factory.NewValueFactory();

                Console.WriteLine( "Adding new value factory to session" );
                newClientResources.Add(TransportConsts.VALUE_FACTORY, vf);

                Console.WriteLine( "New transport" );
                DeliveryService deliveryService = transportFactory.NewTransport( this.uri, newClientResources );

                Console.WriteLine( "New Server" );
                this.factory.NewServer( deliveryService, vf );

                deliveryService.TransportControl( TransportConsts.START, null );
            }

            #endregion

            #region SourceHandler<DataSource> Members

            #endregion

            #region Session Members

            public object SessionQuery(object query)
            {
                if(this.factory is Session)
                    return ((Session) this.factory).SessionQuery(query);
                else
                    throw new Exception("unknown query " + query);
            }

            public void SessionControl(object control, object value)
            {
                if(this.factory is Session)
                    ((Session) this.factory).SessionControl(control, value);
                else
                    throw new Exception("unknown control " + control);
            }

            public void SessionNotify(object eventObj)
            {
                if(this.factory is Session)
                    ((Session) this.factory).SessionNotify(eventObj);
            }

            #region Transport<ServerFactory> Members

            public object TransportQuery( object query )
            {
                return transport.TransportQuery( query );
            }

            public void TransportControl( object control, object value )
            {
                transport.TransportControl( control, value );
            }

            public void TransportNotify( object eventObj )
            {
                transport.TransportNotify( eventObj );
            }

            public ServerFactory GetSession()
            {
                return session;
            }

            public void SetSession( ServerFactory session )
            {
                this.session = session;
            }

            private ServerFactory session;

            #endregion

            #endregion
        }

    }


}

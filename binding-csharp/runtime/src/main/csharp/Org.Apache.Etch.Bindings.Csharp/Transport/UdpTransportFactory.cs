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
using System.Net;
using System.Net.Sockets;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace Org.Apache.Etch.Bindings.Csharp.Transport
{
    public class UdpTransportFactory : TransportFactory
    {
        private const String UDP_LISTENER = "UdpTransportFactory.udpListener";
        private const String SOCKET_ADDRESS = "UdpTransportFactory.socketAddress";

        public UdpTransportFactory()
        {
        }

        protected override TransportMessage NewTransport( string uri, Resources resources )
        {
            UdpListener udpListener = resources.Get( UDP_LISTENER ) as UdpListener;
            IPEndPoint ipEndPoint = resources.Get(SOCKET_ADDRESS) as IPEndPoint;

            URL url = new URL( uri );
            TransportPacket transportPacket = null;

            if (udpListener != null)
                transportPacket = new UdpConnection( ipEndPoint, udpListener );
            else
                transportPacket = new UdpConnection( url );

            TransportMessage transportMessage = new Messagizer( transportPacket, url, resources );

            transportMessage = AddFilters( transportMessage, url, resources );

            ValueFactory vf = (ValueFactory)resources.Get( TransportConsts.VALUE_FACTORY );
            vf.LockDynamicTypes();

            return transportMessage;
        }

        protected override Transport<ServerFactory> NewListener( string uri, Resources resources )
        {
            UdpListener transportListener = new UdpListener( uri, resources );
            return new MySessionListener( this, transportListener, uri, resources );
        }

        public class MySessionListener : Transport<ServerFactory>, SessionListener<IPEndPoint>
        {
            private readonly UdpTransportFactory transportFactory;
            private readonly UdpListener listener;
            private readonly string uri;
            private readonly Resources resources;

            public MySessionListener( UdpTransportFactory transportFactory, UdpListener listener, String uri, Resources resources )
            {
                this.transportFactory = transportFactory;
                this.listener = listener;
                this.uri = uri;
                this.resources = resources;

                listener.SetSession( this );
            }

            public override string ToString()
            {
                return "UdpTransportFactory.MySessionListener/" + listener;
            }

            public void SessionAccepted( IPEndPoint ipEndPoint )
            {
                ValueFactory vf = session.NewValueFactory( uri );
                Resources r = new Resources( resources );

                r.Add( UDP_LISTENER, listener );
                r.Add( SOCKET_ADDRESS, ipEndPoint);
                r.Add( TransportConsts.VALUE_FACTORY, vf );

                TransportMessage t = transportFactory.NewTransport( uri, r );

                session.NewServer( t, uri, r );
            }

            #region Transport<ServerFactory> Members
            public object TransportQuery( object query )
            {
                return listener.TransportQuery( query );
            }

            public void TransportControl( object control, object value )
            {
                listener.TransportControl( control, value );
            }

            public void TransportNotify( object eventObj )
            {
                listener.TransportNotify( eventObj );
            }


            private ServerFactory session;

            public ServerFactory GetSession()
            {
                return session;
            }

            public void SetSession( ServerFactory session )
            {
                this.session = session;
            }
            #endregion

            #region Session Members
            public object SessionQuery( object query )
            {
                return session.SessionQuery( query );
            }

            public void SessionControl( object control, object value )
            {
                session.SessionControl( control, value );
            }

            public void SessionNotify( object eventObj )
            {
                session.SessionNotify( eventObj );
            }
            #endregion
        }

    }
}

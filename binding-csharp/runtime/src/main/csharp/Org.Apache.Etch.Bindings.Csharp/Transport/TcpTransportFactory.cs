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
using System.Net.Sockets;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;
using TcpListener=Org.Apache.Etch.Bindings.Csharp.Util.TcpListener;

namespace Org.Apache.Etch.Bindings.Csharp.Transport
{
    /// <summary>
    /// TransportFactory for tcp connections and listeners.
    /// </summary>
    public class TcpTransportFactory : TransportFactory
    {
        /// <summary>
        /// Constructs a TcpTransportFactory which delivers TcpConnection or a
        /// TlsConnection depending upon the isSecure parameter.
        /// </summary>
        /// <param name="isSecure">true if TlsConnection is desired, false otherwise</param>
        public TcpTransportFactory(bool isSecure)
        {
            this.isSecure = isSecure;
        }

        private readonly bool isSecure;
        
        private const String SOCKET = "TcpTransportFactory.socket";

        protected override TransportMessage NewTransport(string uri, Resources resources)
        {
            URL u = new URL(uri);

            Object socket = resources.Get(SOCKET);

            TransportData c;

            if (isSecure)
                c = new TlsConnection((Socket)socket, u, resources);
            else
                c = new TcpConnection((Socket)socket, u, resources);

            TransportPacket p = new Packetizer(c, u, resources);

            TransportMessage m = new Messagizer(p, u, resources);

            m = AddFilters(m, u, resources);

            //MailboxManager r = new PlainMailboxManager(m, u, resources);

            //DeliveryService d = new DefaultDeliveryService(r, u, resources);

            ValueFactory vf = (ValueFactory) resources.Get(TransportConsts.VALUE_FACTORY);
            vf.LockDynamicTypes();

            return m;
        }

        protected override Transport<ServerFactory> NewListener( string uri, Resources resources )
        {
            Transport<SessionListener<Socket>> l = new TcpListener(uri, resources);
            return new MySessionListener(this, l, uri, resources);
        }

        public class MySessionListener : Transport<ServerFactory>, SessionListener<Socket>
        {
            public MySessionListener(TcpTransportFactory ttf, Transport<SessionListener<Socket>> transport,
                String uri, Resources resources )
            {
                this.ttf = ttf;
                this.transport = transport;
                this.uri = uri;
                this.resources = resources;

                transport.SetSession(this);
            }

            private readonly TcpTransportFactory ttf;
            private readonly Transport<SessionListener<Socket>> transport;
            private readonly string uri;
            private readonly Resources resources;

            public override string ToString()
            {
                return "TcpTransportFactory.MySessionListener/" + transport;
            }

            public void SessionAccepted(Socket socket)
            {
                Resources r = new Resources(resources);
                r.Add(SOCKET, socket);

                ValueFactory vf = session.NewValueFactory(uri);
                r.Add(TransportConsts.VALUE_FACTORY, vf);

                TransportMessage t = ttf.NewTransport(uri, r);

                session.NewServer(t, uri, r);
            }

            #region Transport<ServerFactory> Members

            public object TransportQuery(object query)
            {
                return transport.TransportQuery(query);
            }

            public void TransportControl(object control, object value)
            {
                transport.TransportControl(control, value);
            }

            public void TransportNotify(object eventObj)
            {
                transport.TransportNotify(eventObj);
            }

            public ServerFactory GetSession()
            {
                return session;
            }

            public void SetSession(ServerFactory session)
            {
                this.session = session;
            }

            private ServerFactory session;

            #endregion

            #region Session Members

            public object SessionQuery(object query)
            {
                return session.SessionQuery(query);
            }

            public void SessionControl(object control, object value)
            {
                session.SessionControl(control, value);
            }

            public void SessionNotify(object eventObj)
            {
                session.SessionNotify(eventObj);
            }

            #endregion
        }
    }
}

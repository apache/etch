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

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// Implementation of connection which handles a tcp connection.
    /// </summary>
    public class TcpConnection : TcpTransport
    {
        public TcpConnection(Socket socket, string uri, Resources resources)
            : this(socket, new URL(uri), resources)
        {
            // nothing to do.
        }

        public TcpConnection(Socket socket, URL uri, Resources resources)
            : base(uri, resources)
        {
            if (socket == null)
            {
                if (uri.Host == null)
                    throw new ArgumentNullException("host");
                host = uri.Host;

                if (uri.Port == null)
                    throw new ArgumentNullException("port");
                port = (int)uri.Port;

                if (port <= 0 || port >= 65536)
                    throw new ArgumentException("port <= 0 || port >= 65536");

                this.socket = null;
                //this.host = host;
                //this.port = (int)port;
            }
            else
            {
                this.socket = socket;
                host = null;
                port = 0;
            }
        }

        private readonly String host;

        private readonly int port;

        public override string ToString()
        {
            Socket s = socket;

            if (s != null)
                return String.Format("TcpConnection(up, {0}, {1})",
                    s.LocalEndPoint, s.RemoteEndPoint);

            return String.Format("TcpConnection(down, {0}, {1})", host, port);
        }

        protected override bool IsServer()
        {
            return host == null;
        }

        protected override Socket NewSocket()
        {
            IPAddress addr;

            if (host != null)
            {
                IPAddress[] addrs = Dns.GetHostAddresses(host);
                if (addrs == null || addrs.Length == 0)
                    throw new ArgumentException("host is invalid");
                addr = addrs[0];
            }
            else
            {
                addr = IPAddress.Any;
            }

            IPEndPoint ipe = new IPEndPoint(addr, port);
            Socket socket = new Socket(ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            socket.Connect(ipe);
            return socket;
        }
    }
}

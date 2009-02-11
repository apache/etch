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
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Runtime.CompilerServices;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// Implementation of a connection which handles a socket listener.
    /// </summary>
    public class TcpListener : Connection<SessionListener<Socket>>
    {
        /// <summary>
        /// Constructs the TcpListener.
        /// </summary>
        /// <param name="backlog">max number of pending connection requests.</param>
        /// <param name="host">address to listen to. Null means any local address.</param>
        /// <param name="port">port to listen on. Port must be >= 0. Port of 0
        /// means allocate an available port.</param>
        /// <param name="delay">how long in milliseconds to wait before retrying a
        /// failure. Delay must be >= 0. Delay of 0 means do not retry.</param>

        private TcpListener( int backlog, String host, int port, int delay ) 
        {
            if ( backlog < 0 )
                throw new ArgumentException( "backlog < 0" );

            if ( port < 0 || port > 65535 )
                throw new ArgumentException( "port < 0 || port > 65535" );

            if ( delay < 0 )
                throw new ArgumentException( "delay < 0" );

            this.backlog = backlog;
            this.host = host;
            this.port = port;
            this.delay = delay;
        }

        /// <summary>
        /// Query term from URI to specify backlog value to ServerSocket. The
        /// value is "TcpListener.backlog".
        /// </summary>
        public const String BACKLOG = "TcpListener.backlog";


        public TcpListener(URL uri, Resources resources)
            : this((int)uri.GetIntegerTerm(BACKLOG, 0), TranslateHost(uri.Host), uri.Port != null ? uri.Port.Value : 0, 0)
        {
            // nothing else.
        }

        /// <summary>
        /// Constructs a TcpListener, initialized from the URI. Listens on the host
        /// and port specified in the URI. To listen on all interfaces, specify
        /// host as ALL_INTFS ("0.0.0.0"). If port is specified or defaulted to 0,
        /// an unused port will be selected.
        /// </summary>
        /// <param name="uri"></param>

        public TcpListener(string uri, Resources resources)
            : this(new URL(uri), resources)
        {
            // nothing else.
        }

        private int backlog;
	
	    private String host;
    	
	    private int port;
    	
	    private int delay;

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        
        protected override void Stop0()
        {
            Close( true );
            base.Stop0();
        }
        
        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        /// <returns></returns>
        private Socket CheckSocket()
        {
            Socket ss = serverSocket;
            if ( ss == null )
            {
                //Console.WriteLine( "----socket was null----" );
                throw new IOException("server socket is closed");
            }
            return ss;
        }

        private Socket serverSocket;

        public override string ToString()
        {
            Socket ss = serverSocket;

            if ( ss == null )
                return String.Format("TcpListener (down, {0}:{1})", host, port);

            return String.Format("TcpListener (up, {0})", ss.LocalEndPoint);
        }

        [MethodImpl (MethodImplOptions.Synchronized)]
        protected override bool OpenSocket( bool reconnect )
        {
            bool first = true;
            while ( IsStarted() )
            {
                if ( reconnect || !first )
                {
                    if ( delay == 0 )
                        return false;

                    System.Threading.Monitor.Wait(this, delay);

                    if ( !IsStarted() )
                        break;
                }

                try
                {
                    //IPHostEntry ipEntry = Dns.GetHostEntry( host );

                    IPAddress addr;
                    if ( host != null )
                    {
                        IPAddress[] addrs = Dns.GetHostAddresses( host );
                        if ( addrs == null || addrs.Length == 0 )
                            throw new ArgumentException( "host is invalid" );
                        addr = addrs[ 0 ];
                    }
                    else 
                    {
                        addr = IPAddress.Any;
                    }
                    
                    //IPAddress[] addr = ipEntry.AddressList;
                    //string sIPAddress = addr[ 0 ].ToString();

                    IPEndPoint ipe = new IPEndPoint( addr, port );
                    
                    serverSocket = new Socket( ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp );

                    try
                    {
                        serverSocket.Bind(ipe);
                    }
                    catch (SocketException e)
                    {
                        throw new IOException("Could not bind address "+host+":"+port, e);
                    }

                    serverSocket.Listen( backlog );

                    return true;
                }
                catch (Exception e)
                {
                    if ( first )
                    {
                        first = false;
                        FireException( "open", e );
                    }
                }
            }
            return false;
        }

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        protected override void SetUpSocket()
        {
            // nothing to do.
        }

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        protected override void ReadSocket()
        {
            try
            {
                Socket ss = CheckSocket();
                while (IsStarted())
                {
                    Socket s = ss.Accept();

                    try
                    {
                        session.SessionAccepted(s);
                    }
                    catch (Exception e)
                    {
                        s.Close();
                        FireException("accepted", e);
                    }
                }
            }
            catch (SocketException e)
            {
                if (e.Message != null && e.Message.Contains("interrupted"))
                    return;
                throw;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="reset"></param>
        /// Exception:
        ///     throws Exception
        public override void Close( bool reset )
        {
            Socket ss = serverSocket;
            if ( ss != null )
            {
                serverSocket = null;
                ss.Close();
            }
        }

        public override EndPoint LocalAddress()
        {
            return CheckSocket().LocalEndPoint;
        }

        public override EndPoint RemoteAddress()
        {
            // ignore
            return null;
        }
    }
}

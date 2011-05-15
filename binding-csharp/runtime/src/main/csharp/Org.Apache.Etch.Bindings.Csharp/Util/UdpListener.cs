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
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Runtime.CompilerServices;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// Implementation of a connection which handles a socket listener.
    /// </summary>
    public class UdpListener : Connection<SessionListener<IPEndPoint>>
    {
        /// <summary>
        /// Query term from URI to specify queueSize value to UdpConnection. The
        /// value is "UdpListener.queueSize".
        /// </summary>
        public const String QUEUE_SIZE = "UdpListener.queueSize";
        public const string REUSE_PORT = "UdpListener.reusePort";

        private readonly string host;
        private readonly int port;
        private readonly bool reusePort;
        private readonly int delay;
        private readonly int queueSize;

        private UdpClient udpClient;
        private IDictionary<IPEndPoint, BlockingCollection<UdpPacket>> readQueues;

        public EndPoint LocalEndPoint { get { return udpClient.Client.LocalEndPoint; } }

        /// <summary>
        /// Constructs the UdpListener.
        /// </summary>
        /// <param name="host">address to listen to. Null means any local address.</param>
        /// <param name="port">port to listen on. Port must be >= 0. Port of 0
        /// means allocate an available port.</param>
        /// <param name="reusePort">pass reuse port flag to the socket.</param>
        /// <param name="delay">how long in milliseconds to wait before retrying a
        /// failure. Delay must be >= 0. Delay of 0 means do not retry.</param>
        /// <param name="queueSize">udp connection packet queue size.</param>
        public UdpListener( string host, int port, bool reusePort, int delay, int queueSize )
        {
            if ( port < 0 || port > 65535 )
                throw new ArgumentException( "port < 0 || port > 65535" );

            if ( delay < 0 )
                throw new ArgumentException( "delay < 0" );

            if ( queueSize <= 0 )
                throw new ArgumentException( "queueSize < 0" );

            this.host = host;
            this.port = port;
            this.reusePort = reusePort;
            this.delay = delay;
            this.queueSize = queueSize;

            readQueues = new Dictionary<IPEndPoint, BlockingCollection<UdpPacket>>();
        }

        public UdpListener( URL uri, Resources resources )
            : this( TranslateHost( uri.Host ),
            uri.Port != null ? uri.Port.Value : 0,
            (bool)uri.GetBooleanTerm( REUSE_PORT, false ),
            0,
            (int)uri.GetIntegerTerm( QUEUE_SIZE, 15 ) )
        {
            // nothing else.
        }

        /// <summary>
        /// Constructs a UdpListener, initialized from the URI. Listens on the host
        /// and port specified in the URI. To listen on all interfaces, specify
        /// host as ALL_INTFS ("0.0.0.0"). If port is specified or defaulted to 0,
        /// an unused port will be selected.
        /// </summary>
        /// <param name="uri"></param>
        public UdpListener( string uri, Resources resources )
            : this( new URL( uri ), resources )
        {
            // nothing else.
        }

        [MethodImpl( MethodImplOptions.Synchronized )]
        public int Send( byte[] dgram, int bytes, IPEndPoint endPoint )
        {
            if ( udpClient != null )
                return udpClient.Send( dgram, bytes, endPoint );

            return -1;
        }

        public BlockingCollection<UdpPacket> allocReadQueue( IPEndPoint endPoint )
        {
            lock ( readQueues )
            {
                if ( !readQueues.ContainsKey( endPoint ) )
                    readQueues.Add( endPoint, new BlockingCollection<UdpPacket>( queueSize ) );

                return readQueues[ endPoint ];
            }
        }

        private BlockingCollection<UdpPacket> getReadQueue( IPEndPoint endPoint )
        {
            lock ( readQueues )
            {
                if ( readQueues.ContainsKey( endPoint ) )
                    return readQueues[ endPoint ];
            }

            return null;
        }

        public void releaseReadQueue( IPEndPoint endPoint )
        {
            BlockingCollection<UdpPacket> readQueue = null;

            lock ( readQueues )
            {
                if ( readQueues.ContainsKey( endPoint ) )
                {
                    readQueue = readQueues[ endPoint ];
                    readQueues.Remove( endPoint );
                }
            }

            if ( readQueue != null )
            {
                readQueue.CompleteAdding();
                readQueue.Dispose();
            }
        }

        #region Connection<SessionListener<IPEndPoint>> Member
        [MethodImpl( MethodImplOptions.Synchronized )]
        protected override bool OpenSocket( bool reconnect )
        {
            bool first = true;

            while ( IsStarted() )
            {
                if ( reconnect || !first )
                {
                    if ( delay == 0 )
                        return false;

                    System.Threading.Monitor.Wait( this, delay );

                    if ( !IsStarted() )
                        break;
                }

                try
                {
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

                    IPEndPoint ipe = new IPEndPoint( addr, port );
                    udpClient = new UdpClient();
                    udpClient.Client.SetSocketOption( SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, reusePort );
                    try
                    {
                        udpClient.Client.Bind( ipe );
                    }
                    catch ( SocketException e )
                    {
                        throw new IOException( "Could not bind address " + host + ":" + port, e );
                    }

                    return true;
                }
                catch ( Exception e )
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

        protected override void SetUpSocket()
        {
            udpClient.DontFragment = true;
        }

        protected override void ReadSocket()
        {
            while ( IsStarted() )
            {
                IPEndPoint remoteEP = null;
                byte[] receiveBytes;

                try
                {
                    receiveBytes = udpClient.Receive(ref remoteEP);

                    BlockingCollection<UdpPacket> readQueue = getReadQueue( remoteEP );

                    if ( readQueue == null )
                        session.SessionAccepted( remoteEP );

                    readQueue = getReadQueue( remoteEP );
                    if ( readQueue != null )
                        readQueue.TryAdd( new UdpPacket( remoteEP, receiveBytes ) );
                }
                catch ( SocketException ex )
                {
                    if ( ex.SocketErrorCode != SocketError.ConnectionReset )
                        throw;
                }
                catch ( Exception e )
                {
                    FireException( "accepted", e );
                }
            }
        }

        protected override void Stop0()
        {
            try
            {
                Close( false );
            }
            catch
            {
                // ignore
            }
            base.Stop0();
        }

        public override void Close( bool reset )
        {
            lock ( readQueues )
            {
                foreach ( IPEndPoint ipEndPoint in readQueues.Keys.ToArray() )
                    releaseReadQueue( ipEndPoint );
            }

            if ( udpClient != null )
            {
                udpClient.Close();
                udpClient = null;
            }
        }

        public override EndPoint LocalAddress()
        {
            if ( udpClient != null )
                return udpClient.Client.LocalEndPoint;

            return null;
        }

        public override EndPoint RemoteAddress()
        {
            // ignore
            return null;
        }
        #endregion
    }
}

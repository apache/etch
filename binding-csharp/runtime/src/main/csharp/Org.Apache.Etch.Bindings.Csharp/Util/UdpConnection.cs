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
using System.Net;
using System.Net.Sockets;
using System.Runtime.CompilerServices;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    public class UdpConnection : Connection<SessionPacket>, TransportPacket
    {
        public const string BROADCAST = "UdpConnection.broadcast";

        private IPEndPoint remoteIpEndPoint;
        private readonly UdpListener listener;
        private BlockingCollection<UdpPacket> readQueue;

        private readonly int delay;
        private readonly bool enableBroadcast;
        private readonly string host;
        private readonly int port;

        private UdpClient udpClient;

        public UdpConnection( IPEndPoint remoteIpEndPoint, UdpListener listener )
        {
            this.remoteIpEndPoint = remoteIpEndPoint;
            this.listener = listener;

            readQueue = listener.allocReadQueue( remoteIpEndPoint );
        }

        public UdpConnection( string host, int? port, bool enableBroadcast, int delay )
        {
            if ( host == null )
                throw new ArgumentNullException( "host is missing" );
            else if ( host == "255.255.255.255" )
                enableBroadcast = true;

            if ( port == null )
                throw new ArgumentNullException( "port" );
            else if ( port <= 0 || port >= 65536 )
                throw new ArgumentException( "port <= 0 || port >= 65536" );

            this.host = host;
            this.port = (int)port;
            this.enableBroadcast = enableBroadcast;
            this.delay = delay;

            listener = null;
        }

        public UdpConnection( URL uri )
            : this(uri.Host, uri.Port, uri.GetBooleanTerm( BROADCAST, false ), 0)
        {
            // nothing else.
        }

        public UdpConnection( string uri )
            : this( new URL( uri ) )
        {
            // nothing else.
        }

        public override string ToString()
        {
            if (listener != null || udpClient != null)
                return String.Format( "UdpConnection(up, {0}, {1})", LocalAddress(), RemoteAddress() );

            return String.Format( "UdpConnection(down, {0}, {1})", host, port );
        }

        #region Connection<SessionData> Member
        [MethodImpl( MethodImplOptions.Synchronized )]
        protected override bool OpenSocket( bool reconnect )
        {
            // if a one time connection from a server socket listener, just
            // return the existing socket. Bail if this is a reconnect.
            if ( listener != null )
            {
                if ( !reconnect && readQueue == null )
                    readQueue = listener.allocReadQueue( remoteIpEndPoint );

                return !reconnect;
            }

            // we don't have an existing socket, and this is either the first
            // connection attempt or a reconnect with delay > 0.
            bool first = true;

            while ( IsStarted() )
            {
                // if reconnect is false and first is true, this is our
                // very first attempt to connect. otherwise, we are trying
                // to reconnect a broken link or establish a link where we
                // have already failed at least once.
                if ( reconnect || !first )
                {
                    if ( delay == 0 )
                        return false;

                    System.Threading.Monitor.Wait( this, delay );

                    if ( !IsStarted() )
                        break;
                }

                // try to open a socket.
                try
                {
                    udpClient = new UdpClient( host, port );
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
            if ( udpClient != null )
            {
                udpClient.EnableBroadcast = enableBroadcast;
                udpClient.DontFragment = true;
            }
        }

        protected override void ReadSocket()
        {
            try
            {
                while ( IsStarted() )
                {
                    IPEndPoint senderEndPoint = remoteIpEndPoint;
                    byte[] receiveBytes = null;

                    if ( readQueue != null )
                    {
                        UdpPacket packet = readQueue.Take();

                        senderEndPoint = packet.IPEndPoint;
                        receiveBytes = packet.Bytes;
                    }
                    else
                        receiveBytes = udpClient.Receive( ref senderEndPoint );

                    WhoSender sender = new WhoSender( senderEndPoint );
                    FlexBuffer receiveBuf = new FlexBuffer( receiveBytes );

                    session.SessionPacket( sender, receiveBuf );
                }
            }
            catch ( ArgumentNullException )
            {
                // ignore
            }
            catch ( SocketException ex )
            {
                if ( ex.SocketErrorCode != SocketError.Interrupted )
                    throw ex;
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
            if ( listener != null )
            {
                listener.releaseReadQueue( remoteIpEndPoint );
                readQueue = null;
            }
            else if ( udpClient != null )
            {
                udpClient.Close();
                udpClient = null;
            }
        }

        public override EndPoint LocalAddress()
        {
            if ( listener != null )
                return listener.LocalEndPoint;

            return udpClient.Client.LocalEndPoint;
        }

        public override EndPoint RemoteAddress()
        {
            if (listener != null)
                return remoteIpEndPoint;

            return udpClient.Client.RemoteEndPoint;
        }
        #endregion

        #region TransportPacket Member
        public int HeaderSize()
        {
            return 0;
        }

        public void TransportPacket( Who recipient, FlexBuffer buf )
        {
            byte[] sendBytes = buf.GetAvailBytes();

            if ( listener != null )
            {
                IPEndPoint ipe = remoteIpEndPoint;

                if ( recipient != null )
                {
                    if (!(recipient is WhoSender))
                        throw new Exception( "unknown recipient" );
                    ipe = ( recipient as WhoSender ).sender;
                }

                if (ipe == null)
                    throw new Exception( "unknown receiver" );

                listener.Send( sendBytes, sendBytes.Length, ipe );
            }
            else
                udpClient.Send( sendBytes, sendBytes.Length );
        }

        public override object TransportQuery( object query )
        {
            if ( query.Equals( TransportConsts.IS_SERVER ) )
                return listener != null;

            return base.TransportQuery( query );
        }
        #endregion

        private class WhoSender : Who
        {
            public IPEndPoint sender;

            public WhoSender( IPEndPoint sender )
            {
                this.sender = sender;
            }

            public override int GetHashCode()
            {
                return sender.GetHashCode();
            }

            public override bool Equals( object obj )
            {
                WhoSender whoObj = obj as WhoSender;

                if ( whoObj != null )
                    return sender.Equals( whoObj.sender );

                return base.Equals( obj );
            }

            public override string ToString()
            {
                return "WhoSender(" + sender + ")";
            }
        }

    }
}

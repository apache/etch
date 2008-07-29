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
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Runtime.CompilerServices;

namespace Etch.Util
{
    /// <summary>
    /// Constants and methods related to tcp transport.
    /// </summary>
    abstract public class TcpTransport : Connection<SessionData>, TransportData
    {
        /// <summary>
        /// Term on the uri which specifies the auto flush flag. The term string is
        /// "TcpTransport.autoFlush". The value is "true" or "false". The default
        /// is "false".
        /// </summary>
        public const String AUTO_FLUSH = "TcpTransport.autoFlush";

        /// <summary>
        /// Term on the uri which specifies the buffer size in bytes. The term
        /// string is "TcpTransport.bufferSize". The value is an integer between
	    /// 0 and 65536. The default is 0.
        /// </summary>
        public const String BUFFER_SIZE = "TcpTransport.bufferSize";

        /// <summary>
        /// Term on the uri which specifies the keep alive flag. The term string is
	    /// "TcpTransport.keepAlive". The value is "true" or "false". The default is
	    /// "false".
        /// </summary>
        public const String KEEP_ALIVE = "TcpTransport.keepAlive";

        /// <summary>
        /// Term on the uri which specifies the linger time in seconds. The term
	    /// string is "TcpTransport.lingerTime". The value is an integer between -1
	    /// and 240. The default is 30. -1 means disable.
        /// </summary>
        public const String LINGER_TIME = "TcpTransport.lingerTime";

        /// <summary>
        /// Term on the uri which specifies the no delay flag. The term string is
	    /// "TcpTransport.noDelay". The value is "true" or "false". The default is
	    /// "true".
        /// </summary>
        public const String NO_DELAY = "TcpTransport.noDelay";

        /// <summary>
        /// Term on the uri which specifies the reconnect delay in milliseconds. The
	    /// term string is "TcpTransport.reconnectDelay". The value is an integer >=
	    /// 0. The default is 0.
        /// </summary>
        public const String RECONNECT_DELAY = "TcpTransport.reconnectDelay";

        /// <summary>
        /// Term on the uri which specifies the traffic class. The term string is
	    /// "TcpTransport.trafficClass". The value is an integer between 0 and 255.
	    /// The default is 0.
        /// </summary>
        public const String TRAFFIC_CLASS = "TcpTransport.trafficClass";

        /// <summary>
        /// Constructs the TcpTransport. Pulls common parameters off the uri.
        /// </summary>
        /// <param name="uri"></param>
        /// <param name="resources"></param>
        protected TcpTransport(URL uri, Resources resources)
        {
            SetDefaultAutoFlush(uri.GetBooleanTerm(AUTO_FLUSH, false));
            SetDefaultBufferSize((int) uri.GetIntegerTerm(BUFFER_SIZE, 0));
            SetDefaultKeepAlive(uri.GetBooleanTerm(KEEP_ALIVE, false));
            SetDefaultLingerTime((int) uri.GetIntegerTerm(LINGER_TIME, 30));
            SetDefaultNoDelay(uri.GetBooleanTerm(NO_DELAY, true));
            SetDefaultReconnectDelay((int) uri.GetIntegerTerm(RECONNECT_DELAY, 0));
            SetDefaultTrafficClass((int) uri.GetIntegerTerm(TRAFFIC_CLASS, 0));
        }

        private void SetDefaultAutoFlush(bool autoFlush)
        {
            this.autoFlush = autoFlush;
        }

        /// <summary>
        /// The auto flush setting for this connection. If true, each call to send
        /// must automatically call flush.
        /// </summary>
        protected bool autoFlush;

        private void SetDefaultBufferSize(int bufferSize)
        {
            if (bufferSize < 0 || bufferSize > 65536)
                throw new ArgumentException(
                    "bufferSize < 0 || bufferSize > 65536");
            this.bufferSize = bufferSize;
        }

        /// <summary>
        /// The output buffer size to use for this connection. Bytes, 0 means
        /// unbuffered output. If using buffered output, you'll want to disable
        /// auto flush and call flush manually only when needed.
        /// </summary>
        protected int bufferSize;

        private void SetDefaultKeepAlive(bool keepAlive)
        {
            this.keepAlive = keepAlive;
        }

        /// <summary>
        /// The tcp keep alive setting for this connection.
        /// </summary>
        protected bool keepAlive;

        private void SetDefaultLingerTime(int lingerTime)
        {
            if (lingerTime < -1 || lingerTime > 240)
                throw new ArgumentException(
                    "lingerTime < -1 || lingerTime > 240");
            this.lingerTime = lingerTime;
        }

        /// <summary>
        /// The tcp linger time setting for this connection. Time in seconds, -1
        /// means disable.
        /// </summary>
        protected int lingerTime;

        private void SetDefaultNoDelay(bool noDelay)
        {
            this.noDelay = noDelay;
        }

        /// <summary>
        /// The tcp no delay setting for this connection. True disables nagle's
        /// algorithm and causes all sends to be made asap.
        /// </summary>
        protected bool noDelay;

        private void SetDefaultReconnectDelay(int reconnectDelay)
        {
            if (reconnectDelay < 0)
                throw new ArgumentException(
                    "reconnectDelay < 0");
            this.reconnectDelay = reconnectDelay;
        }

        /// <summary>
        /// The reconnect delay for this connection. Time in milliseconds, 0 means
        /// do not reconnect.
        /// </summary>
        protected int reconnectDelay;

        private void SetDefaultTrafficClass(int trafficClass)
        {
            if (trafficClass < 0 || trafficClass > 255)
                throw new ArgumentException(
                    "trafficClass < 0 || trafficClass > 255");
            this.trafficClass = trafficClass;
        }

        /// <summary>
        /// The traffic class for this connection. 0-255, 0 means normal handling.
        /// Also called type of service or dscp.
        /// </summary>
        protected int trafficClass;

        protected override void Stop0()
        {
            try
            {
                Close(false);
            }
            catch (Exception)
            {
                // ignore
            }
            base.Stop0();
        }

        protected Socket socket;

        /// <summary>
        /// Checks the connection socket for being open.
        /// </summary>
        /// <returns>the connection socket</returns>
        protected Socket CheckSocket()
        {
            Socket s = socket;

            if (s == null)
                throw new IOException("socket closed");

            return s;
        }

        public override void Close(bool reset)
        {
            Socket s = socket;
            if (s != null)
            {
                try
                {
                    try
                    {
                        if (reset)
                            s.LingerState = new LingerOption(false, 0);
                        else
                        {
                            Flush();
                            ShutdownOutput();
                        }
                    }
                    finally
                    {
                        s.Close();
                    }
                }
                catch (Exception)
                {
                    // ignore.
                }
                finally
                {
                    stream = null;
                    socket = null;
                }
            }
        }

        /// <summary>
        /// Sends some data to the remote end. The output data is buffered
        /// until the buffer is full or the buffer is flushed.
        /// </summary>
        /// <param name="buf">the bytes to be sent</param>
        /// Exception:
        ///     throws Exception if there is a problem transmitting the
        ///     data. Such a problem causes the current connection to be
        ///     reset.
        /// <seealso cref="Flush()"/>
        /// <seealso cref="SetDefaultAutoFlush(bool)"/>
        public void Send(byte[] buf)
        {
            Send(buf, 0, buf.Length);
        }

        /// <summary>
        /// Sends some data to the remote end. The output data is buffered
        /// until the buffer is full or the buffer is flushed.
        /// </summary>
        /// <param name="buf">the bytes to be sent</param>
        /// <param name="off">the offset into buf of the first byte to send</param>
        /// <param name="len">the number of bytes to send</param>
        public void Send(byte[] buf, int off, int len)
        {
            try
            {
                Stream s = checkStream();
                s.Write(buf, off, len);
                if (autoFlush)
                {
                    s.Flush();
                }
            }
            catch (Exception e)
            {
                Close(true);
                throw e;
            }
        }

        public void Flush()
        {
            try
            {
                checkStream().Flush();
            }
            catch (Exception e)
            {
                Close(true);
                throw e;
            }
        }

        protected Stream checkStream()
        {
            Stream ns = stream;
            if (ns == null)
                throw new IOException("net stream closed");
            return ns;
        }

        private void FireData(FlexBuffer buf)
        {
            session.SessionData(null, buf);
        }

        protected Stream stream;

        public override EndPoint LocalAddress()
        {
            return CheckSocket().LocalEndPoint;
        }

        public override EndPoint RemoteAddress()
        {
            return CheckSocket().RemoteEndPoint;
        }

        public void TransportData(Who recipient, FlexBuffer buf)
        {
            Send(buf.GetBuf(), buf.Index(), buf.Avail());
        }

        protected override void SetUpSocket()
        {
            Socket s = CheckSocket();

            s.SetSocketOption(SocketOptionLevel.Tcp, SocketOptionName.KeepAlive, keepAlive);
            s.LingerState = new LingerOption(lingerTime >= 0, lingerTime >= 0 ? lingerTime : 0);
            s.NoDelay = noDelay;
            //s.SetSocketOption(SocketOptionLevel.Tcp, SocketOptionName.TypeOfService, trafficClass);

            stream = new NetworkStream(socket);
            // TODO do something about buffering this stream.
        }

        public void ShutdownInput()
        {
            CheckSocket().Shutdown(SocketShutdown.Receive);
        }

        public void ShutdownOutput()
        {
            CheckSocket().Shutdown(SocketShutdown.Send);
        }

        abstract protected Socket NewSocket();

        abstract protected bool IsServer();

        public override object TransportQuery(object query)
        {
            if (query.Equals(TransportConsts.IS_SERVER))
                return IsServer();

            return base.TransportQuery(query);
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        protected override bool OpenSocket(bool reconnect)
        {
            // if a one time connection from a server socket listener, just
            // return the existing socket.
            if (!reconnect && socket != null)
                return true;

            // if a one time connection from a server socket listener, and
            // this is a reconnect, then bail.
            if (reconnect && IsServer())
                return false;

            // if a reconnect but no retries allowed, then bail.
            if (reconnect && reconnectDelay == 0)
                return false;

            // ok, we don't have an existing socket, and this is either the first
            // connection attempt or a reconnect with delay > 0.

            bool first = true;

            while (IsStarted())
            {
                // if reconnect is false and first is true, this is our
                // very first attempt to connect. otherwise, we are trying
                // to reconnect a broken link or establish a link where we
                // have already failed at least once.

                if (reconnect || !first)
                {
                    if (reconnectDelay == 0)
                        return false;

                    System.Threading.Monitor.Wait(this, reconnectDelay);

                    if (!IsStarted())
                        break;
                }

                // try to open a socket.

                try
                {
                    socket = NewSocket();
                    return true;
                }
                catch (Exception e)
                {
                    if (first)
                    {
                        first = false;
                        FireException("open", e);
                    }
                }
            }

            return false;
        }

        protected override void ReadSocket()
        {
            Stream ns = checkStream();
            FlexBuffer buf = new FlexBuffer(new byte[8192]);

            try
            {
                while (IsStarted())
                {
                    int n = ns.Read(buf.GetBuf(), 0, buf.Length());

                    if (n <= 0)
                        break;

                    buf.SetLength(n);
                    buf.SetIndex(0);
                    FireData(buf);
                }
            }
            catch (Exception e)
            {
                if (e.Message == null)
                    throw e;
                if (e.Message.Contains("connection was aborted"))
                    return;
                if (e.Message.Contains("blocking operation"))
                    return;
                if (e.Message.Contains("socket closed"))
                    return;
                if (e.Message.Contains("read operation failed"))
                    return;
                throw e;
            }
        }
    }
}

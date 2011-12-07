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
using System.Threading;
using System.Globalization;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// Constants and methods related to tcp transport.
    /// </summary>
    abstract public class TcpTransport : Connection<SessionData>, TransportData
    {
        /// <summary>
        /// Constructs the TcpTransport. Pulls common parameters off the uri.
        /// </summary>
        /// <param name="uri"></param>
        /// <param name="resources"></param>
        protected TcpTransport(URL uri, Resources resources)
        {
            options = new TcpOptions(uri, resources);
        }

        private readonly TcpOptions options;

        protected override void Stop0()
        {
            try
            {
                Close(false);
            }
            catch
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
                catch
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
        /// <seealso cref="TcpOptions.autoFlush"/>
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
                if (options.autoFlush)
                {
                    s.Flush();
                }
            }
            catch (Exception)
            {
                Close(true);
                throw;
            }
        }

        public void Flush()
        {
            try
            {
                checkStream().Flush();
            }
            catch (Exception)
            {
                Close(true);
                throw;
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

            s.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.KeepAlive, options.keepAlive);
            s.LingerState = new LingerOption(options.lingerTime >= 0, options.lingerTime >= 0 ? options.lingerTime : 0);
            s.NoDelay = options.noDelay;

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
            if (reconnect && options.reconnectDelay == 0)
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
                    if (options.reconnectDelay == 0)
                        return false;

                    System.Threading.Monitor.Wait(this, options.reconnectDelay);

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

            //set the language to English in order to make sure the following
            //string comparisons work also with an installed .Net Framework Language Pack

            //save users current CI
            CultureInfo userCI = Thread.CurrentThread.CurrentCulture;

            //set CI to English
            Thread.CurrentThread.CurrentCulture = CultureInfo.CreateSpecificCulture("en-US");
            Thread.CurrentThread.CurrentUICulture = Thread.CurrentThread.CurrentCulture;

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
                string errorMessage = e.Message;

                if (errorMessage == null)
                  throw;
                if (errorMessage.Contains("connection was aborted"))
                  return;
                if (errorMessage.Contains("blocking operation"))
                  return;
                if (errorMessage.Contains("socket closed"))
                  return;
                if (errorMessage.Contains("read operation failed"))
                  return;
                throw;

            }
            finally
            {
                //set CI back to the users CI
                Thread.CurrentThread.CurrentCulture = userCI;
                Thread.CurrentThread.CurrentUICulture = Thread.CurrentThread.CurrentCulture;
            }
        }
    }
}

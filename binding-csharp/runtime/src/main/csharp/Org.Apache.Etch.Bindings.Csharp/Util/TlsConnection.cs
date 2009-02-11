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
using System.Net.Security;
using System.Net.Sockets;
using System.Security.Authentication;
using System.Security.Cryptography.X509Certificates;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    public class TlsConnection : TcpTransport
    {
        /// <summary>
        /// Term on uri which specifies whether server certificate should be authenticated. 
        /// </summary>
        public const string AUTH_REQD = "TlsConnection.authReqd";

        /// <summary>
        /// Term on uri which specifies the certificate name of the server.
        /// </summary>
        public const string CERT_NAME = "TlsConnection.certName";

        public TlsConnection(Socket socket, string uri, Resources resources)
            : this(socket, new URL(uri), resources)
        {
            // nothing to do.
        }

        public TlsConnection(Socket socket, URL uri, Resources resources)
            : base(uri, resources)
        {
            SetCertificateName(uri.GetTerm(CERT_NAME, "default"));
            SetAuthReqd(uri.GetBooleanTerm(AUTH_REQD, true));

            if (socket == null)
            {
                String host = uri.Host;
                if (host == null)
                    throw new ArgumentNullException("host == null");

                int? port = uri.Port;
                if (port == null)
                    throw new ArgumentNullException("port == null");

                if (port <= 0 || port >= 65536)
                    throw new ArgumentOutOfRangeException("port <= 0 || port >= 65536");

                this.socket = null;
                this.host = host;
                this.port = (int)port;
            }
            else
            {
                this.socket = socket;
                this.host = null;
                this.port = 0;
            }
        }

        private readonly String host;

        private readonly int port;

        public override string ToString()
        {
            Socket s = socket;

            if (s != null)
                return String.Format("TlsConnection(up, {0}, {1})",
                    s.LocalEndPoint, s.RemoteEndPoint);

            return String.Format("TlsConnection(down, {0}, {1})", host, port);
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

        protected override void SetUpSocket()
        {
            base.SetUpSocket();

            if (IsServer())
            {
                stream = new SslStream(stream, false);
                try
                {
                    ((SslStream)stream).AuthenticateAsServer(GetServerCert(certName));
                }
                catch (AuthenticationException e)
                {
                    //Console.WriteLine(" Exception in authenticating certificate ");
                    FireException("Certificate Authentication", e);
                }
            }
            else
            {
                stream = new SslStream(stream, false,
                     new RemoteCertificateValidationCallback(ValidateServerCertificate),
                     null);

                // The server name must match the name on the server certificate.

                try
                {
                    ((SslStream)stream).AuthenticateAsClient(certName);
                }
                catch (AuthenticationException e)
                {
                    AuthenticationException e1 = new AuthenticationException(
                        "Remote Certificate Mismatch Error. Problem in setting up"
                        + " SSL Connection. Either disable server authentication by using"
                        + " term TlsConnection.authReqd=false on uri or provide a valid"
                        + " certificate name using term TlsConnection.certName=name on uri.", e);
                    throw e1;
                }
            }
        }

        private void SetCertificateName(string certName)
        {
            this.certName = certName;
        }

        private string certName;

        private void SetAuthReqd(bool authReqd)
        {
            this.authReqd = authReqd;
        }

        private bool authReqd;

        public  bool ValidateServerCertificate(
           object sender,
           X509Certificate certificate,
           X509Chain chain,
           SslPolicyErrors sslPolicyErrors)
        {
            if (!authReqd)
                return true;

            if (sslPolicyErrors == SslPolicyErrors.None)
                return true;

            //Console.WriteLine("Certificate error: {0}", sslPolicyErrors);

            // Do not allow this client to communicate with unauthenticated servers.
            return false;
        }

        private  X509Certificate GetServerCert(string certName)
        {
            X509Store store = new X509Store(StoreName.My, StoreLocation.LocalMachine);
            store.Open(OpenFlags.ReadOnly);
            X509CertificateCollection coll = store.Certificates.Find(X509FindType.FindBySubjectName, certName, true);
            return coll[0];
        }
    }
}

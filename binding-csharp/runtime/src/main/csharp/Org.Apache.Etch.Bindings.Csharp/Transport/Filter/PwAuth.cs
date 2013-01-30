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
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace Org.Apache.Etch.Bindings.Csharp.Transport.Filter
{
    /// <summary>
    /// PwAuth is a message filter which watches for Session.UP and attempts to
    /// authenticate the user by sending a message to the server with the name and
    /// password in it. If the server responds, the session is notified with the
    /// user name. If the server responds negatively, the session is notified of the
    /// failure (using a PwAuthFail event). A transport control may also be sent to
    /// force a login or logout or to set the name or password. The name and password
    /// are initialized from the uri, or prompted for if not specified.
    /// </summary>
    public class PwAuth : AbstractMessageFilter
    {



        /// <summary>
        /// client sessionQuery: return user name to use for authentication.
        /// transportQuery: return the current user name (on server it has been validated).
        /// client transportControl: set user name to use for authentication.
        /// </summary>

        public const String USER = "PwAuth.user";



        /// <summary>
        /// client sessionQuery: return password to use for authentication.
        /// transportQuery: return the current password (on server it has been validated).
        /// client transportControl: set password to use for authentication.
        /// </summary>
        /// 
        public const String PASSWORD = "PwAuth.password";

        /// <summary>
        /// client transportControl: send authentication request
        /// </summary>
        private const String AUTHENTICATE = "PwAuth.authenticate";

        public PwAuth(TransportMessage transport, URL uri, Resources resources)
            : base(transport, uri, resources)
        {

            server = (Boolean)transport.TransportQuery(TransportConsts.IS_SERVER);

            if (!server)
            {
                user = uri.User;
                password = uri.Password;
            }

            //		Log.report( "KeepAliveInstalled",
            //			"delay", delay, "count", count, "server", server );

            vf = (ValueFactory)resources.Get(TransportConsts.VALUE_FACTORY);

            mt_request = new XType("_Etch_PwAuthReq");
            mt_response = new XType("_Etch_PwAuthResp");

            vf.AddType(mt_request);
            vf.AddType(mt_response);

            mf_user = new Field("user");
            mf_password = new Field("password");
            mf_ok = new Field("ok");
            mf_status = new Field("status");

            mt_request.PutValidator(mf_user, Validator_string.Get(0));
            mt_request.PutValidator(mf_password, Validator_string.Get(0));
            mt_request.SetResult(mt_response);
            mt_request.Lock();

            mt_response.PutValidator(mf_ok, Validator_boolean.Get(0));
            mt_response.PutValidator(mf_status, Validator_string.Get(0));
            mt_response.Lock();
        }

        private String user;

        private String password;

        private readonly bool server;

        private readonly ValueFactory vf;

        private readonly Field mf_user;

        private readonly Field mf_password;

        private readonly Field mf_ok;

        private readonly Field mf_status;

        private readonly XType mt_request;

        private readonly XType mt_response;

        public bool GetServer()
        {
            return server;
        }

        public override bool SessionMessage(Who sender, Message msg)
        {
            if (msg.IsType(mt_request))
            {
                HandlePwAuthReq(sender, msg);
                return true;
            }

            if (msg.IsType(mt_response))
            {
                HandlePwAuthResp(sender, msg);
                return true;
            }

            return base.SessionMessage(sender, msg);
        }


        protected override bool SessionUp()
        {
            if (!server)
            {
                // client: send authentication request.
                SendPwAuthReq();
            }
            return true;
        }


        protected override bool SessionDown()
        {
            if (server)
            {
                // make sure that server session's idea of user is cleared.
                SessionNotifyUser(null, null);
            }

            return true;
        }

        private void SendPwAuthReq()
        {
            if (user == null)
            {
                user = (String)session.SessionQuery(USER);
                if (user == null)
                    throw new Exception("PwAuth: user == null");
            }

            if (password == null)
            {
                password = (String)session.SessionQuery(PASSWORD);
                if (password == null)
                    throw new Exception("PwAuth: password == null");
            }

            Message msg = new Message(mt_request, vf);
            msg.Add(mf_user, user);
            msg.Add(mf_password, password);
            transport.TransportMessage(null, msg);
        }

        private void HandlePwAuthReq(Who sender, Message msg)
        {
            if (!server)
            {
                // we're a client that got a request, likely we're talking to a
                // server that is confused.
                return;
            }

            Console.WriteLine(" Sender: " + sender);
            Console.WriteLine(" Message: " + msg);

            String u = (String)msg.Get(mf_user);
            String p = (String)msg.Get(mf_password);

            try
            {
                OkStatus as1 = (OkStatus)SessionQuery(new UserPassword(u, p));

                if (as1.ok)
                    SessionNotifyUser(u, p);
                else
                    SessionNotifyUser(null, null);

                sendPwAuthResp(sender, msg, as1.ok, as1.status);
            }
            catch (Exception e)
            {
                SessionNotifyUser(null, null);
                sendPwAuthResp(sender, msg, false, e.ToString());
                throw e;
            }
        }

        private void sendPwAuthResp(Who recipient, Message msg, bool result, String status)
        {
            Message rmsg = msg.Reply();
            rmsg.Add(mf_ok, result);
            rmsg.Add(mf_status, status);
            transport.TransportMessage(recipient, rmsg);
        }

        private void HandlePwAuthResp(Who sender, Message msg)
        {
            if (server)
            {
                // we're a server that got a response, likely we're talking to a
                // client that is confused.
                return;
            }

            // Log.report( "GotPwAuthResp", "sender", sender, "msg", msg );

            bool result = (Boolean)msg.Get(mf_ok);
            String status = (String)msg.Get(mf_status);

            session.SessionNotify(new OkStatus(result, status));
        }


        public override object TransportQuery(Object query)
        {
            if ((string)query == USER)
                return user;

            if ((string)query == PASSWORD)
                return password;

            return base.TransportQuery(query);
        }


        public override void TransportControl(Object control, Object value)
        {
            if ((string)control == USER)
            {
                CheckNotServer();
                user = (String)value;
                return;
            }

            if ((string)control == PASSWORD)
            {
                CheckNotServer();
                password = (String)value;
                return;
            }

            if ((string)control == AUTHENTICATE)
            {
                CheckNotServer();
                SendPwAuthReq();
                return;
            }

            base.TransportControl(control, value);
        }

        private void CheckNotServer()
        {
            if (server)
                throw new Exception("control not permitted by server");
        }


        private void SessionNotifyUser(String user, String password)
        {
            this.user = user;
            this.password = password;
            session.SessionNotify(new UserPassword(user, password));
        }

        /// <summary>
        /// Server session query response to AuthUserPassword.
        /// </summary>
        public class OkStatus
        {
            /// <summary>Constructs the AuthOkStatus.</summary>
            /// <param name="ok">true if user / password valid.</param>
            /// <param name="status">message related to ok.</param>
            public OkStatus(bool ok, String status)
            {
                this.ok = ok;
                this.status = status;
            }


            public override String ToString()
            {
                return String.Format("AuthOkStatus( %s, %s )", ok, status);
            }

            // true if the user / password matched 
            public readonly bool ok;

            // message related to ok
            public readonly String status;
        }

        /// <summary>
        /// Server session query to get AuthOkStatus, also server session event
        /// to report changes in authentication status.
        /// </summary>
        public class UserPassword
        {

            /// <summary>Constructs the CheckAuth.</summary>
            /// <param name="user">name to check</param>
            /// <param name="password">password of the user</param>
            public UserPassword(String user, String password)
            {
                this.user = user;
                this.password = password;
            }


            public override String ToString()
            {
                return String.Format("AuthUserPassword( %s )", user);
            }

            // The user name to check
            public readonly String user;

            // The password of the user
            public readonly String password;
        }


        public override string ToString()
        {
            return "PwAuth/" + transport;
        }
    }


}

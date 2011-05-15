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
using System.Threading;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Transport.Filter
{
    [TestFixture]
    public class TestKeepAlive
    {
        private MyTransport transport;
        private MySession session;
        private MyValueFactory vf;
        private Resources resources;

        [SetUp]
        public void Initialize()
        {
            transport = new MyTransport();
            session = new MySession();
            vf = new MyValueFactory("tcp:");
            resources = new Resources();
            resources.Add(TransportConsts.VALUE_FACTORY, vf);
        }

        private KeepAlive newKeepAlive(string uri)
        {
            return new KeepAlive(transport, new URL(uri), resources);
        }

        [Test]
        public void Constructor1()
        {
            transport.is_server = true;
            string uri = "tcp://localhost:4008?filter=KeepAlive";
            KeepAlive filter = newKeepAlive(uri);

            filter.SetSession(session);

            Assert.AreSame(transport.GetSession(), filter);
            Assert.AreSame(filter.GetSession(), session);
            Assert.AreEqual(filter.GetServer(), true);
            Assert.AreEqual(filter.GetCount(), 4);
            Assert.AreEqual(filter.GetDelay(), 15);
        }

        [Test]
        public void constructor2()
        {
            transport.is_server = true;
            String uri = "tcp://localhost:4008?filter=KeepAlive&KeepAlive.delay=20&KeepAlive.count=5";
            KeepAlive filter = newKeepAlive(uri);
            filter.SetSession(session);

            Assert.AreSame(transport.GetSession(), filter);
            Assert.AreSame(filter.GetSession(), session);
            Assert.AreEqual(filter.GetServer(), true);
            Assert.AreEqual(filter.GetCount(), 5);
            Assert.AreEqual(filter.GetDelay(), 20);
        }

        [Test]
        public void constructor3()
        {
            transport.is_server = false;
            String uri = "tcp://localhost:4008?filter=KeepAlive";
            KeepAlive filter = newKeepAlive(uri);
            filter.SetSession(session);

            Assert.AreSame(transport.GetSession(), filter);
            Assert.AreSame(filter.GetSession(), session);
            Assert.AreEqual(filter.GetServer(), false);
            Assert.AreEqual(filter.GetCount(), 4);
            Assert.AreEqual(filter.GetDelay(), 15);
        }

        [Test]
        public void clientup()
        {
            transport.is_server = false;
            string uri = "tcp://localhost:4008?filter=KeepAlive&KeepAlive.delay=3";
            KeepAlive filter = newKeepAlive(uri);
            filter.SetSession(session);
            filter.SessionNotify(SessionConsts.UP);
            Console.WriteLine("Sleeping for 5 seconds");
            Thread.Sleep(5000);
            Console.WriteLine("Out of sleep");
            Assert.AreEqual(What.TRANSPORT_MESSAGE, transport.what);
            Message msg = transport.msg;
            Assert.AreEqual("_Etch_KeepAliveReq", msg.GetXType.Name);
            Assert.AreEqual(4, msg.Get(new Field("count")));
            Assert.AreEqual(3, msg.Get(new Field("delay")));
            filter.SessionNotify(SessionConsts.DOWN);
            Assert.IsFalse(session.up);
}

        [Test]
        public void clientup1()
        {
            transport.is_server = false;
            string uri = "tcp://localhost:4008?filter=KeepAlive&KeepAlive.delay=2";
            KeepAlive filter = newKeepAlive(uri);
            filter.SetSession(session);
            filter.SessionNotify(SessionConsts.UP);
            Console.WriteLine("Sleeping for 4 seconds");
            Thread.Sleep(4000);
            Console.WriteLine("Out of sleep");
            Assert.AreEqual(What.TRANSPORT_MESSAGE, transport.what);
            Message msg = transport.msg;
            Assert.AreEqual("_Etch_KeepAliveReq", msg.GetXType.Name);
            Assert.AreEqual(4, msg.Get(new Field("count")));
            Assert.AreEqual(2, msg.Get(new Field("delay")));
            filter.SessionNotify(SessionConsts.DOWN);
            Assert.IsFalse(session.up);
        }

        [Test]
        public void clientdown()
        {
            transport.is_server = false;
            string uri = "tcp://localhost:4008?filter=KeepAlive";
            KeepAlive filter = newKeepAlive(uri);
            filter.SetSession(session);
            filter.SessionNotify(SessionConsts.DOWN);
            Assert.IsFalse(session.up);
        }

        [Test]
        public void serverUp()
        {
            transport.is_server = true;
            string uri = "tcp://localhost:4008?filter=KeepAlive";
            KeepAlive filter = newKeepAlive(uri);
            filter.SetSession(session);
            filter.SessionNotify(SessionConsts.UP);
            Message req = ConstructMessage();

            filter.SessionMessage(null, req);

            Assert.AreEqual(req.Reply().ToString(), transport.msg.ToString());

            filter.SessionNotify(SessionConsts.DOWN);
            Assert.IsFalse(session.up);
        }

        [Test]
        public void serverdown()
        {
            transport.is_server = true;
            string uri = "tcp://localhost:4008?filter=KeepAlive";
            KeepAlive filter = newKeepAlive(uri);
            filter.SetSession(session);
            filter.SessionNotify(SessionConsts.DOWN);
            Assert.IsFalse(session.up);
        }

        private Message ConstructMessage()
        {
            int delay = 15;
            int count = 4;

            MyValueFactory vf1 = new MyValueFactory("tcp:");

            Field mf_delay = new Field("delay");
            Field mf_count = new Field("count");

            XType mt_request = new XType("_Etch_KeepAliveReq");
            mt_request.PutValidator(mf_delay, Validator_int.Get(0));
            mt_request.PutValidator(mf_count, Validator_int.Get(0));
            vf1.AddType(mt_request);

            XType mt_response = new XType("_Etch_KeepAliveResp");
            vf1.AddType(mt_response);

            mt_request.SetResult(mt_response);

            Message msg = new Message(mt_request, vf1);
            msg.Add(mf_delay, delay);
            msg.Add(mf_count, count);

            return msg;
        }

        public enum What
        {
            TRANSPORT_MESSAGE,
            SESSION_MESSAGE
        }

        static Object cook(Object o)
        {
            if (o is Exception)
            {
                Exception e = (Exception)o;
                e.GetBaseException();
                throw e;
            }
            return o;
        }

        public class MyTransport : TransportMessage
        {

            public What what;

            public Who recipient;

            public Message msg;

            public Object is_server;

            private SessionMessage session;


            #region TransportMessage Members

            public void TransportMessage(Who recipient, Org.Apache.Etch.Bindings.Csharp.Msg.Message msg)
            {
                this.what = What.TRANSPORT_MESSAGE;
                this.recipient = recipient;
                this.msg = msg;
            }

            #endregion

            #region Transport<SessionMessage> Members

            public object TransportQuery(object query)
            {
                if ((string)query == TransportConsts.IS_SERVER)
                    return cook(is_server);
                throw new Exception("The method or operation is not implemented.");
            }

            public void TransportControl(object control, object value)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void TransportNotify(object eventObj)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void SetSession(SessionMessage session)
            {
                this.session = session;
            }

            public SessionMessage GetSession()
            {
                return session;
            }

            #endregion
        }

        public class MySession : SessionMessage
        {


            public What what;

            public Who sender;

            public Message msg;

            public bool handled;

            public Object user;

            public Object password;

            public Boolean up;



            #region SessionMessage Members

            public bool SessionMessage(Who sender, Org.Apache.Etch.Bindings.Csharp.Msg.Message msg)
            {
                this.what = What.SESSION_MESSAGE;
                this.sender = sender;
                this.msg = msg;
                return handled;
            }

            #endregion

            #region Session Members

            public object SessionQuery(object query)
            {
                return query;
            }

            public void SessionControl(object control, object value)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void SessionNotify(object eventObj)
            {
                if (eventObj.Equals(SessionConsts.UP))
                {
                    up = true;
                    return;
                }

                if (eventObj.Equals(SessionConsts.DOWN))
                {
                    up = false;
                    return;
                }



                
            }

            #endregion
        }

        public class MyValueFactory : DefaultValueFactory
        {

            public MyValueFactory(String uri)
                : base(uri, types, class2type)
            {
            }

            private readonly static TypeMap types = new TypeMap();

            private readonly static Class2TypeMap class2type = new Class2TypeMap();

            static MyValueFactory()
            {
                DefaultValueFactory.Init(types, class2type);
            }

        }
    }
}

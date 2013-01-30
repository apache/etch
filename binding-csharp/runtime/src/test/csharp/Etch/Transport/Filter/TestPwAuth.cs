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
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Transport.Filter
{
    [TestFixture]
    public class TestPwAuth
    {
        private MyTransport transport;
        private  MySession session;
        private MyValueFactory vf;
        private  Resources resources;

        [SetUp]
        public void Initialize()
        {
            transport = new MyTransport();
            session = new MySession();
            vf = new MyValueFactory("tcp:");
            resources = new Resources();
            resources.Add(TransportConsts.VALUE_FACTORY, vf);
        }

        private PwAuth newPwAuth( String uri ) 
	    {
		    return new PwAuth( transport, new URL( uri ), resources );
	    }

        [Test]
        public void Constructor1()
        {
            transport.is_server = true;
            PwAuth pwAuth = newPwAuth("tcp://fred:1234@host:9999");
            pwAuth.SetSession(session);
            Assert.AreSame(pwAuth,transport.GetSession());
            Assert.AreSame(session, pwAuth.GetSession());
            Assert.AreEqual(true,pwAuth.GetServer());
            Assert.IsNull(pwAuth.TransportQuery(PwAuth.USER));
            Assert.IsNull(pwAuth.TransportQuery(PwAuth.PASSWORD));
        }

        [Test]
        public void Constructor2()
        {
            transport.is_server = false;
            PwAuth pwAuth = newPwAuth("tcp://fred:1234@host:9999");
            pwAuth.SetSession(session);
            Assert.AreSame(pwAuth, transport.GetSession());
            Assert.AreSame(session, pwAuth.GetSession());
            Assert.AreEqual(false, pwAuth.GetServer());
            Assert.AreEqual("fred",pwAuth.TransportQuery(PwAuth.USER));
            Assert.AreEqual("1234", pwAuth.TransportQuery(PwAuth.PASSWORD));
        }

        [Test]
        public void Constructor3()
        {
            transport.is_server = false;
            PwAuth pwAuth = newPwAuth("tcp://mary:2345@host:9999");
            pwAuth.SetSession(session);
            Assert.AreSame(pwAuth, transport.GetSession());
            Assert.AreSame(session, pwAuth.GetSession());
            Assert.AreEqual(false, pwAuth.GetServer());
            Assert.AreEqual("mary", pwAuth.TransportQuery(PwAuth.USER));
            Assert.AreEqual("2345", pwAuth.TransportQuery(PwAuth.PASSWORD));
        }

        [Test]
        public void Constructor4()
        {
            transport.is_server = false;
            PwAuth pwAuth = newPwAuth("tcp://alice@host:9999");
            pwAuth.SetSession(session);
            Assert.AreSame(pwAuth, transport.GetSession());
            Assert.AreSame(session, pwAuth.GetSession());
            Assert.AreEqual(false, pwAuth.GetServer());
            Assert.AreEqual("alice", pwAuth.TransportQuery(PwAuth.USER));
            Assert.IsNull(pwAuth.TransportQuery(PwAuth.PASSWORD));
        }

        [Test]
        public void Constructor5()
        {
            transport.is_server = false;
            PwAuth pwAuth = newPwAuth("tcp://host:9999");
            pwAuth.SetSession(session);
            Assert.AreSame(pwAuth, transport.GetSession());
            Assert.AreSame(session, pwAuth.GetSession());
            Assert.AreEqual(false, pwAuth.GetServer());
            Assert.IsNull(pwAuth.TransportQuery(PwAuth.USER));
            Assert.IsNull(pwAuth.TransportQuery(PwAuth.PASSWORD));
        }

        [Test]
        [ExpectedException(typeof(Exception))]
        public void Constructor6()
        {
            transport.is_server = new Exception("is_server failed");
            PwAuth pwAuth = newPwAuth("tcp://fred:1234@host:9999");
            
        }

        [Test]
        public void Clientup1()
        {
            transport.is_server = false;
            PwAuth pwAuth = newPwAuth("tcp://fred:1234@host:9999");
            pwAuth.SetSession(session);
            pwAuth.SessionNotify(SessionConsts.UP);
            Assert.IsTrue(session.up);
            Assert.AreEqual(What.TRANSPORT_MESSAGE, transport.what);
            Message msg1 = transport.msg;
            Assert.AreEqual("_Etch_PwAuthReq", msg1.GetXType.Name);
            Assert.AreEqual("fred",msg1.Get(new Field("user")));
            Assert.AreEqual("1234", msg1.Get(new Field("password")));
        }

        [Test]
        public void Clientup2()
        {
            transport.is_server = false;
            session.password = "2345";
            PwAuth pwAuth = newPwAuth("tcp://fred@host:9999");
            pwAuth.SetSession(session);
            pwAuth.SessionNotify(SessionConsts.UP);
            Assert.IsTrue(session.up);
            Assert.AreEqual(What.TRANSPORT_MESSAGE, transport.what);
            Message msg1 = transport.msg;
            Assert.AreEqual("_Etch_PwAuthReq", msg1.GetXType.Name);
            Assert.AreEqual("fred", msg1.Get(new Field("user")));
            Assert.AreEqual("2345", msg1.Get(new Field("password")));
        }

        [Test]
        public void Clientup3()
        {
            transport.is_server = false;
            session.user = "mary";
            session.password = "3456";
            PwAuth pwAuth = newPwAuth("tcp://host:9999");
            pwAuth.SetSession(session);
            pwAuth.SessionNotify(SessionConsts.UP);
            Assert.IsTrue(session.up);
            Assert.AreEqual(What.TRANSPORT_MESSAGE, transport.what);
            Message msg1 = transport.msg;
            Assert.AreEqual("_Etch_PwAuthReq", msg1.GetXType.Name);
            Assert.AreEqual("mary", msg1.Get(new Field("user")));
            Assert.AreEqual("3456", msg1.Get(new Field("password")));
        }

        [Test]
        [ExpectedException(typeof(Exception))]
        public void Clientup4()
        {
            transport.is_server = false;
            session.user = "mary";
            PwAuth pwAuth = newPwAuth("tcp://host:9999");
            pwAuth.SetSession(session);
            pwAuth.SessionNotify(SessionConsts.UP);         
        }

        [Test]
        [ExpectedException(typeof(Exception))]
        public void Clientup5()
        {
            transport.is_server = false;
            session.password = "1234";
            PwAuth pwAuth = newPwAuth("tcp://host:9999");
            pwAuth.SetSession(session);
            pwAuth.SessionNotify(SessionConsts.UP);         
        }

        [Test]
        [ExpectedException(typeof(Exception))]
        public void Clientup6()
        {
            transport.is_server = false;
            session.user = "fred";
            session.password = new Exception("password failed");
            PwAuth pwAuth = newPwAuth("tcp://host:9999");
            pwAuth.SetSession(session);
            pwAuth.SessionNotify(SessionConsts.UP);
        }

        [Test]
        [ExpectedException(typeof(Exception))]
        public void Clientup7()
        {
            transport.is_server = false;
            session.user = new Exception("user failed");
            session.password = "1234";
            PwAuth pwAuth = newPwAuth("tcp://host:9999");
            pwAuth.SetSession(session);
            pwAuth.SessionNotify(SessionConsts.UP);
        }

        [Test]
        public void Clientdown1()
        {
            transport.is_server = false;
            
            PwAuth pwAuth = newPwAuth("tcp://host:9999");
            pwAuth.SetSession(session);
            pwAuth.SessionNotify(SessionConsts.DOWN);

            Assert.IsFalse(session.up);
        }


        [Test]
        public void Serverup1()
        {
            transport.is_server = true;

            PwAuth pwAuth = newPwAuth("tcp://host:9999");
            pwAuth.SetSession(session);
            pwAuth.SessionNotify(SessionConsts.UP);

            Assert.IsTrue(session.up);
        }

        [Test]
        public void Serverdown1()
        {
            transport.is_server = true;

            PwAuth pwAuth = newPwAuth("tcp://host:9999");
            pwAuth.SetSession(session);
            pwAuth.SessionNotify(SessionConsts.DOWN);

            Assert.IsFalse(session.up);
            Assert.IsNull(session.userPassword.user);
            Assert.IsNull(session.userPassword.password);
        }



        public enum What
        {
            TRANSPORT_MESSAGE,
            SESSION_MESSAGE
        }

        static Object cook( Object o ) 
	    {
		if (o is Exception)
		{
			Exception e = (Exception) o;
		    e.GetBaseException();
			throw e;
		}
		return o;
	    }

        public class MyTransport : TransportMessage
        {

            /** */
            public What what;
            /** */
            public Who recipient;
            /** */
            public Message msg;
            /** */
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

            /** */
            public What what;
            /** */
            public Who sender;
            /** */
            public Message msg;
            /** */
            public bool handled;
            /** */
            public Object user;
            /** */
            public Object password;
            /** */
            public Boolean up;
            /** */
            public PwAuth.UserPassword 
                userPassword = new PwAuth.UserPassword(null,null);

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
                if ((string)query == PwAuth.USER)
                    return cook(user);

                if ((string)query == PwAuth.PASSWORD)
                    return cook(password);
			
                throw new Exception("The method or operation is not implemented.");
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
    			
			    if (eventObj is PwAuth.UserPassword)
			    {
				    userPassword = (PwAuth.UserPassword) eventObj;
				    return;
			    }

                throw new Exception("The method or operation is not implemented.");
            }

            #endregion
        }

        public class MyValueFactory : DefaultValueFactory
        {

            public MyValueFactory( String uri ) : base(uri, types, class2type)
		    {
			}
		
		    private readonly static TypeMap types = new TypeMap();
		
		    private readonly static Class2TypeMap class2type = new Class2TypeMap();

		    static MyValueFactory()
		    {
			    DefaultValueFactory.Init( types, class2type );
		    }
            
        }


    }
}

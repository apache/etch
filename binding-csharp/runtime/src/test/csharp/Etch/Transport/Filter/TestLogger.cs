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
    public class TestLogger
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

        private Logger newLogger(string uri) 
	    {
		    return new Logger(transport,new URL(uri),null);
	    }

        [Test]
	    public void constructor1() {
		    String uri = "tcp://localhost:4008?filter=Logger";
		    Logger.testFlag = true;
		    Logger logger = newLogger(uri);
		    Assert.AreEqual(logger.GetFileName(),"Log.txt");
            Assert.AreNotSame(logger.GetFileName(),logger.GetTestRenamedFileName());
	    }

        [Test]
        public void constructor2()
        {
            String uri = "tcp://localhost:4008?filter=Logger&Logger.filePath=Test.txt";
            Logger.testFlag = true;
            Logger logger = newLogger(uri);
            Assert.AreEqual(logger.GetFileName(), "Test.txt");
            Assert.AreNotSame(logger.GetFileName(), logger.GetTestRenamedFileName());
        }

        [Test]
	    public void logger() {
		    String uri = "tcp://localhost:4008?filter=Logger";
		    Logger.testFlag = true;
		    Logger logger = newLogger(uri);
		    logger.SetSession( session );
		    Assert.AreSame(transport.GetSession(),logger);
		    Assert.AreSame(logger.GetSession(),session);
	    }

        [Test]
        public void logger1()
        {
            String uri = "tcp://localhost:4008?filter=Logger";
            Logger.testFlag = true;
            Logger logger = newLogger(uri);
            logger.SetSession(session);
            Assert.AreSame(transport.GetSession(), logger);
            Assert.AreSame(logger.GetSession(), session);
            Message m = constructMessage();
            logger.TransportMessage(null,m);
            Assert.AreEqual(m.ToString(),transport.msg.ToString());
        }

        [Test]
        public void logger2()
        {
            String uri = "tcp://localhost:4008?filter=Logger";
            Logger.testFlag = true;
            Logger logger = newLogger(uri);
            logger.SetSession(session);
            Assert.AreSame(transport.GetSession(), logger);
            Assert.AreSame(logger.GetSession(), session);
            Message m = constructMessage();
            logger.SessionMessage(null, m);
            Assert.AreEqual(m.ToString(), session.msg.ToString());
        }

        private Message constructMessage()
	    {	
		    XType mt_request = new XType( "_Etch_Logger_Test" );
		
		
		    XType mt_response = new XType( "_Etch_Logger_Test_Resp" );
			
		    mt_request.SetResult( mt_response );
		
		    vf.AddType( mt_request);
		
		    Message msg = new Message( mt_request, vf );
	
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



                throw new Exception("The method or operation is not implemented.");
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

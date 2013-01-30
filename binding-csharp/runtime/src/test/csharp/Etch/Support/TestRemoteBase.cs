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
using Org.Apache.Etch.Bindings.Csharp.Transport;
using Org.Apache.Etch.Bindings.Csharp.Util;
using NUnit.Framework;

//using Etch.Support;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{

    [TestFixture]
    public class TestRemoteBase : RemoteBase
    {
        
        public TestRemoteBase() : this(new MyDeliveryService(),new MyDefaultValueFactory())
        {
            
        }

        public TestRemoteBase(MyDeliveryService svc, MyDefaultValueFactory vf) : base(svc,vf)
        {
            this.svc = svc;
        }
        
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestRemoteBase" );
        }

        [SetUp]
        public void Init()
        {
            svc.what = null;
            svc.xmsg = null;
            svc.xresponseType = null;
            svc.xresponseField = null;
            svc.xtimeout = 0;
        }

        private readonly MyDeliveryService svc;
     /*   private static MyDeliveryService svc = new MyDeliveryService(); //declared static
        //private static DefaultValueFactory vf = new DefaultValueFactory();//declared static
        private static MyDefaultValueFactory vf = new MyDefaultValueFactory(); 
        private RemoteBase rb = new RemoteBase(svc, vf); */
    	private XType mt = new XType( "foo" );
    	private XType rmt = new XType( "bar" );
    	private Field rmf = new Field( "baz" );
	
	    [Test]
	    public void NewMessage()
	    {
		    Message msg = _NewMessage( mt );
		    msg.CheckType( mt );
		   
		    Assert.IsNull( svc.what );
		    Assert.IsNull( svc.xmsg );
		    Assert.IsNull( svc.xresponseType );
		    Assert.IsNull( svc.xresponseField );
		    Assert.AreEqual( 0, svc.xtimeout );
	    }
    	
	    [Test]
	    public void Send() 
	    {

		    Message msg = _NewMessage( mt );
    		
		    _Send( msg );
		    Assert.IsTrue( svc.what.Equals(What.SEND));
		    Assert.AreSame( svc.xmsg, msg );
		    Assert.IsNull( svc.xresponseType );
		    Assert.IsNull( svc.xresponseField );
		    Assert.AreEqual( 0, svc.xtimeout );
	    }
    	
        [Test]
	    public void Call() 
	    {
		    Message msg = _NewMessage( mt );

            rmt.ResponseField = rmf;
            rmt.Timeout = 98;

            Object result = _EndCall( _BeginCall( msg ), rmt );

		    Assert.IsTrue( svc.what.Equals(What.CALL));
		    Assert.AreSame( svc.xmsg, msg );
		    Assert.AreSame( svc.xresponseType, rmt );
		    Assert.AreSame( svc.xresponseField, rmf );
		    Assert.AreEqual( svc.xtimeout, 98 );
		    Assert.AreEqual( result, 23 );
	    }
    	
	    public enum What { SEND, CALL, SHUTDOWN, BEGINCALL };
    	
	    public class MyDeliveryService : DeliveryService, MailboxManager
	    {

            public Enum what;
            
            public void Init()
            {
                what = null;
            }

		    public Message xmsg;
		    public XType xresponseType;
		    public Field xresponseField;
		    public int xtimeout;
            public Mailbox xmb;
    		
		    public void Send( Message msg ) 
		    {
			    Assert.IsNull( what );
			    what = What.SEND;
			    this.xmsg = msg;
		    }

            public Mailbox BeginCall( Message msg )
            {
                Init();
                Assert.IsNull( what );
                what = What.BEGINCALL;
                this.xmsg = msg;
                this.xmb = new PlainMailbox(this, 0L);
                return this.xmb;
            }

            public Object EndCall( Mailbox mb, XType responseType ) 
		    {
                Assert.AreEqual( What.BEGINCALL, what );
                Assert.AreEqual( xmb, mb );
			    what = What.CALL;
                xmb = null;
			    this.xresponseType = responseType;
                this.xresponseField = responseType.ResponseField;
			    this.xtimeout = responseType.Timeout;
			    return 23;
		    }

            public void TransportNotify( Object eventObj )
            {
                // ignore
            }

            public Object TransportQuery( Object query )
            {
                // ignore
                return null;
            }

            public void TransportControl( Object control, Object value )
            {
                // ignore
            }

            public void Redeliver(Who sender, Message msg)
            {
                // ignore
            }

            public void Unregister(Mailbox mb)
            {
                // ignore
            }


            #region TransportMessage Members

            public void TransportMessage(Who recipient, Message msg)
            {
                Assert.IsNull(what);
                what = What.SEND;
                this.xmsg = msg;
               
            }

            #endregion

            #region Transport<SessionMessage> Members


            public void SetSession(SessionMessage session)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public SessionMessage GetSession()
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion

            #region SessionMessage Members

            public bool SessionMessage(Who sender, Message msg)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion

            #region Session Members

            public object SessionQuery(object query)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void SessionControl(object control, object value)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void SessionNotify(object eventObj)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion



            #region MailboxManager Members


            public Mailbox TransportCall(Who recipient, Message msg)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion
        }

        /// <summary>
        /// A fake implementation of DefaultValueFactory for testing
        /// </summary>
        /// 
	    public class MyDefaultValueFactory : DefaultValueFactory
	    {
		    private readonly static TypeMap types = new TypeMap();

		    

            private readonly static Class2TypeMap class2type = 
                new Class2TypeMap();

		    static MyDefaultValueFactory()
		    {
                DefaultValueFactory.Init( types, class2type );
		    }

		    /**
		     * Constructs the ValueFactoryFake.
		     */
            public MyDefaultValueFactory() : base("none:",types, class2type)
		    {
		    
		    }

		   

           
	    }
  }
}
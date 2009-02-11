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

namespace Org.Apache.Etch.Bindings.Csharp.Transport
{
    [TestFixture]
    public class TestPlainMailboxManager
    {

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestMailboxManager" );
        }

        private PlainMailboxManager mmgr;
        private Message msg;
        private MyMessageHandler mh;
        private MyMessagizer m;

        private bool duplexFlag = true;
        private bool registerFlag = false;

        private readonly ValueFactory vf = new MyValueFactory();

        public Message ConstructSimpleMessage()
        {
            Message msg = new Message( MyValueFactory.mt_function, vf );
            return msg;
        }

        public Message ConstructAddMessage() 
	    {
		    msg = new Message( MyValueFactory.mt_add, vf );
		    msg.Add( MyValueFactory.mf_x, 4 );
		    msg.Add( MyValueFactory.mf_y, 5 );
    		
		    return msg;
	    }

        [SetUp]
        public void SetUp()
        {
            duplexFlag = true;
          
            m = new MyMessagizer(this);
            mmgr = new PlainMailboxManager(m, "none:", null);
            mh = new MyMessageHandler(mmgr);
       
        }
    	
	    [Test]
	    public void SimplexMessage() 
	    {
            Assert.IsNull(m.what);
    		
		    Message message = ConstructSimpleMessage();
		    mmgr.TransportMessage( null, message );
    		
		    Assert.AreEqual(m.what, What.MYMESSAGIZERMESSAGE);
	    }
    	
	    [Test]
	    public void DuplexMessage() 
	    {
            Assert.IsNull(m.what);
		    Assert.IsNull(mh.what);

            mmgr.SessionNotify(SessionConsts.UP);
            Message message = ConstructAddMessage();
            mmgr.TransportCall( null, message);
    		
		    Assert.AreEqual(m.what, What.MYMESSAGIZERMESSAGE);
    		
		    /* 
		     * Because PlainMailbox will intercept messages ...
		     * The Default messagehandler won't be called.
		     */
            Assert.AreEqual(What.OOB_NOTIFY_HANDLER, mh.what);
	    }
    	
	    [Test]
	    public void DuplicateRegistration() 
	    {
            mmgr.SessionNotify(SessionConsts.UP);

            Message message = ConstructAddMessage();
		    Mailbox mb = 
                ( Mailbox ) mmgr.TransportCall( null, message);

		    /*
		     * Check for duplicate registration
		     */
		    try 
		    {
			    mmgr.Register(mb);
		    }
		    catch (Exception)
		    {
			    registerFlag = true;
		    }
		    Assert.IsTrue(registerFlag);
	    }
    	
	    [Test]
	    public void TestUnregistration() 
	    {
            mmgr.SessionNotify(SessionConsts.UP);
            Message message = ConstructAddMessage();
		    Mailbox mb =  mmgr.TransportCall( null, message );

		    /*
		     * Unregister and then register again ... 
		     */
		    mmgr.Unregister(mb);
            registerFlag = false;
		    try 
		    {
                mmgr.Register( ( Mailbox ) mb );
		    }
		    catch (Exception)
		    {
                registerFlag = true;
		    }
		    Assert.IsFalse(registerFlag);

	    }
    	
	    [Test]
	    public void NoMailboxOrReplyToId() 
	    {
            Assert.IsNull(mh.what);
		    Assert.IsNull(m.what);
    		
		    duplexFlag = false;
		    Message message = ConstructAddMessage();
    		
		    m.TransportMessage(null, message);
    		
		    Assert.AreEqual(m.what, What.MYMESSAGIZERMESSAGE);
		    /*
		     * Since no mailbox, default message handler receives
		     * messages.
		     */
		    Assert.AreEqual(mh.what, What.MMGRMESSAGEHANDLERMESSAGE);
	    }

        public enum What
        {
            MMGRMESSAGEHANDLERSTARTED,
            MMGRMESSAGEHANDLERUP,
            MMGRMESSAGEHANDLERDown,
            MMGRMESSAGEHANDLERSTOPPED,
            MYMESSAGIZERClose,
            MYMESSAGIZERLocalAddress,
            MYMESSAGIZERRemoteAddress,
            MYMESSAGIZERShutdownInput,
            MYMESSAGIZERShutdownOutput,
            MYMESSAGIZERSTOP,
            MYMESSAGIZERMESSAGE,
            MMGRMESSAGEHANDLERMESSAGE, 
            OOB_CONTROL_HANDLER, 
            OOB_NOTIFY_HANDLER, 
            OOB_QUERY_HANDLER
        };

        public class MyMessageHandler : SessionMessage
        {
            public Enum what;

            public Who xsender;

            public Message xmsg;

            public Object xcontrol;

            public Object xvalue;

            public Object xquery;

            public Object xevent;

            public Object xquery_result;

            public TransportMessage _tmm;

            public MyMessageHandler()
            {
                // nothing to do.
            }

            public MyMessageHandler( TransportMessage tmm )
            {
                _tmm = tmm;
                _tmm.SetSession(this);
            }
    		
            //public bool Message( Who sender, Message msg ) 
            //{
            //    what = What.MMGRMESSAGEHANDLERMESSAGE;
            //    xsender = sender;
            //    xmsg = msg;
            //    return true;
            //}

           

            #region Session Members

            public object SessionQuery( object query )
            {
                what = What.OOB_QUERY_HANDLER;
                xquery = query;
                return xquery_result;
            }

            public void SessionControl( object control, object value )
            {
                what = What.OOB_CONTROL_HANDLER;
                xcontrol = control;
                xvalue = value;
            }

            public void SessionNotify( object eventObj )
            {
                what = What.OOB_NOTIFY_HANDLER;
                xevent = eventObj;
            }

            #endregion

            #region SessionMessage Members

            public bool SessionMessage(Who sender, Message msg)
            {
                what = What.MMGRMESSAGEHANDLERMESSAGE;
                xsender = sender;
                xmsg = msg;
                return true;
            }

            #endregion
        }

        public class MyMessagizer : TransportMessage
        {
            private SessionMessage handler;

            public Enum what;

            public TestPlainMailboxManager _tmm;

            public MyMessagizer( TestPlainMailboxManager tmm )
            {
                _tmm = tmm;
            }

      

            public override string ToString()
            {
                return "My Messagizer";
            }

          
            //public void Messagex( Who recipient, Message msg ) 
            //{
            //    // message from mailbox manager
            //    what = What.MYMESSAGIZERMESSAGE;
    			
            //    // send message back ... 
            //    if ( _tmm.duplexFlag == true )
            //        msg.InReplyTo = msg.MessageId;
				    
            //    handler.Message(null, msg);
            //}

           
           

            #region Transport Members

            public object TransportQuery( object query )
            {
                return null;
            }

            public void TransportControl( object control, object value )
            {
                // ignore
            }

            public void TransportNotify( object eventObj )
            {
                // ignore
            }

            #endregion

            #region TransportMessage Members

            public void TransportMessage(Who recipient, Message msg)
            {
                // message from mailbox manager
                what = What.MYMESSAGIZERMESSAGE;

                // send message back ... 
                if (_tmm.duplexFlag == true)
                    msg.InReplyTo = msg.MessageId;

                handler.SessionMessage(null, msg);
            }

            #endregion

            #region Transport<SessionMessage> Members


            public void SetSession(SessionMessage session)
            {
                this.handler = session;
            }

            #endregion

            #region Transport<SessionMessage> Members


            public SessionMessage GetSession()
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion
        }

        public class MyValueFactory : DefaultValueFactory
        {
            private readonly static TypeMap types = new TypeMap();

            

            private readonly static Class2TypeMap class2type = 
                new Class2TypeMap();

            public readonly static XType mt_function = types.Get( "function" );

            public readonly static XType mt_add = types.Get( "add" );

            public readonly static XType mt_add_result = types.Get( "add_result" );

            public readonly static Field mf_x = new Field( "x" );

            public readonly static Field mf_y = new Field( "y" );

            public readonly static Field mf_result = new Field( "xresult" );

            static MyValueFactory()
            {
                DefaultValueFactory.Init( types, class2type );

                mt_function.PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );
                mt_function.PutValidator( DefaultValueFactory._mf__inReplyTo, Validator_long.Get( 0 ) );

                mt_add.PutValidator( mf_x, Validator_int.Get( 0 ) );
                mt_add.PutValidator( mf_y, Validator_int.Get( 0 ) );
                mt_add.PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );
                mt_add.PutValidator( DefaultValueFactory._mf__inReplyTo, Validator_long.Get( 0 ) );

                mt_add_result.PutValidator( mf_result, Validator_int.Get( 0 ) );
                mt_add_result.PutValidator( DefaultValueFactory._mf__messageId, Validator_long.Get( 0 ) );
                mt_add_result.PutValidator( DefaultValueFactory._mf__inReplyTo, Validator_long.Get( 0 ) );
            }

            public MyValueFactory()
                : base("none:", types, class2type)
            {
             
            }
        }
    }
}

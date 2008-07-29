///$Id$
///
///Created by Champakesan, Badri Narayanan on Jul 02, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

using System;
using NUnit.Framework;
using Etch.Msg;
using Etch.Support;
using Etch.Transport;

namespace Etch.Support.Test
{
    [TestFixture]
    public class TestStubBase
    {
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestStubBase" );
        }

        public enum What 
        { 
            /** wanted message delivered */ UNWANTED 
        };

        MyUnwanted obj = new MyUnwanted();

        [Test]
        public void Nothing()
        { }
        
        //[Test]
        public void Unwanted2()
        {
            MyUnwanted obj = new MyUnwanted();

            DeliveryService src = new MyMessageSource();

            StubBase sb = new StubBase( src, obj, null, null );

            Who sender = new MyWho(); 
            Message msg = new Message(null, null);

            sb.SessionMessage(sender, msg);


            Assert.AreEqual( obj.what, What.UNWANTED );
            Assert.AreEqual( sender, obj.xsender );
            Assert.AreEqual( msg, obj.xmsg );
        }

        //[Test] 
        [ExpectedException("System.InvalidOperationException")]
        public void Unwanted3()
        {
            Object obj = new Object();

            DeliveryService src = new MyMessageSource();
            StubBase sb = new StubBase( src, obj, null, null );

            Who sender = new MyWho(); 
            Message msg = new Message(null, null);

            sb.SessionMessage(sender, msg);
           Assert.Fail("delivery of unwanted message should fail");
        }

        //[Test]
        [ExpectedException("System.InvalidOperationException")]
        public void Unwanted4()
        {
            Object obj = new Object();

            DeliveryService src = new MyMessageSource();
            StubBase sb = new StubBase( src, obj, null, null );

            Who sender = new MyWho(); 
            Message msg = new Message(null, null);

            sb.SessionMessage(sender, msg);
            Assert.Fail("delivery of unwanted message should fail");
        }

        public class MyMessageSource : DeliveryService
        {

           

            #region TransportMessage Members

            public void TransportMessage(Who recipient, Message msg)
            {
                Assert.Fail("not needed");
            }

            #endregion

            #region Transport<SessionMessage> Members

            public object TransportQuery(object query)
            {
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
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion

            #region DeliveryService Members

            public Mailbox BeginCall(Message msg)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public object EndCall(Mailbox mb, XType responseType)
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

            #region Transport<SessionMessage> Members


            public SessionMessage GetSession()
            {
                throw new Exception("The method or operation is not implemented.");
            }

            #endregion
        }

        public class MyUnwanted : Unwanted
        {
            public void Init()
            {
                what = null;
            }

            public void _Unwanted( Who sender, Message msg)
            {
                Init();
                Assert.IsNull(what);
                what = What.UNWANTED;
                this.xsender = sender;
                this.xmsg = msg;
            }

            public Enum what;

            public Who xsender;

            public Message xmsg;
        }
    }

    public class MyWho : Who
    {
    }
}
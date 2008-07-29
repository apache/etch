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

            MessageSource src = new MyMessageSource();

            StubBase sb = new StubBase( src, obj, null, null );

            Who sender = new MyWho(); 
            Message msg = new Message(null, null);

            sb.Message( sender, msg);


            Assert.AreEqual( obj.what, What.UNWANTED );
            Assert.AreEqual( sender, obj.xsender );
            Assert.AreEqual( msg, obj.xmsg );
        }

        //[Test] 
        [ExpectedException("System.InvalidOperationException")]
        public void Unwanted3()
        {
            Object obj = new Object();

            MessageSource src = new MyMessageSource();
            StubBase sb = new StubBase( src, obj, null, null );

            Who sender = new MyWho(); 
            Message msg = new Message(null, null);

            sb.Message(sender, msg);
           Assert.Fail("delivery of unwanted message should fail");
        }

        //[Test]
        [ExpectedException("System.InvalidOperationException")]
        public void Unwanted4()
        {
            Object obj = new Object();

            MessageSource src = new MyMessageSource();
            StubBase sb = new StubBase( src, obj, null, null );

            Who sender = new MyWho(); 
            Message msg = new Message(null, null);

            sb.Message(sender, msg);
            Assert.Fail("delivery of unwanted message should fail");
        }

        public class MyMessageSource : MessageSource
        {

            #region MessageSource Members

            public void Messagex( Who recipient, Message msg )
            {
                Assert.Fail( "not needed" );
            }

            #endregion
        }

        public class MyUnwanted : Unwanted
        {
            public void Init()
            {
                what = null;
            }

            public void _Unwanted(MessageSource src, Who sender, Message msg)
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
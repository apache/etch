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
using System.Collections.Generic;
using System.Text;
using System.Threading;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Transport
{
    [TestFixture]
    public class TestPlainMailbox : MailboxManager
    {
        [SetUp]
        public void Init()
        {
            unregistered = false;

            //     AlarmManager.SetAlarmManager(null);

            redelivered = new List<Element>();

            vf = new MyValueFactory();

            mt_foo = new XType("foo");

            foo_msg = new Message(mt_foo, vf);

            fred_who = new WhoAmI();

            mt_bar = new XType("bar");

            bar_msg = new Message(mt_bar, vf);

            alice_who = new WhoAmI();

            notify = new MyNotify();

            notify1x = new MyNotify();
        }
        
        [Test]
        public void construct1()
        {
            testConstruct(1L);
        }


        [Test]
        [ExpectedException(typeof(NullReferenceException))]
        public void construct2()
        {
            new PlainMailbox(null, 1L);
        }


        [Test]
        public void closeDelivery1()
        {
            // open mailbox and close it for delivery while empty.

            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkCloseDelivery(mb, true);
            checkMailbox(mb, true, false, true, true, 0);
        }


        [Test]
        public void closeDelivery2()
        {
            // open mailbox and close it for delivery while not empty.

            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, alice_who, foo_msg);
            checkMailbox(mb, false, true, false, false, 0);

            checkCloseDelivery(mb, true);
            checkMailbox(mb, false, true, true, true, 0);
        }


        [Test]
        public void closeDelivery3()
        {
            // open mailbox and close it for delivery twice.

            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkCloseDelivery(mb, true);
            checkMailbox(mb, true, false, true, true, 0);

            checkCloseDelivery(mb, false);
            checkMailbox(mb, true, false, true, true, 0);
        }


        [Test]
        public void closeRead1()
        {
            // open mailbox and close it for reading while empty.

            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkCloseRead(mb, true);
            checkMailbox(mb, true, false, true, true, 0);
        }


        [Test]
        public void closeRead2()
        {
            // open mailbox and close it for reading while not empty.

            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, alice_who, foo_msg);
            checkMailbox(mb, false, true, false, false, 0);

            checkCloseRead(mb, true);
            checkMailbox(mb, true, false, true, true, 1);
            checkRedelivered(0, alice_who, foo_msg);
        }


        [Test]
        public void closeRead3()
        {
            // open mailbox and close it for reading twice while empty.

            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkCloseRead(mb, true);
            checkMailbox(mb, true, false, true, true, 0);

            checkCloseRead(mb, false);
            checkMailbox(mb, true, false, true, true, 0);
        }


        [Test]
        public void closeRead4()
        {
            // open mailbox and close it for reading twice while not empty.

            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, alice_who, foo_msg);
            checkMailbox(mb, false, true, false, false, 0);

            checkCloseRead(mb, true);
            checkMailbox(mb, true, false, true, true, 1);
            checkRedelivered(0, alice_who, foo_msg);

            checkCloseRead(mb, false);
            checkMailbox(mb, true, false, true, true, 1);
            checkRedelivered(0, alice_who, foo_msg);
        }


        [Test]
        public void full1()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, fred_who, foo_msg);
            checkMailbox(mb, false, true, false, false, 0);

            checkDeliver(mb, false, alice_who, bar_msg);
            checkMailbox(mb, false, true, false, false, 0);
        }


        [Test]
        public void full2()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, fred_who, foo_msg);
            checkMailbox(mb, false, true, false, false, 0);

            checkDeliver(mb, false, alice_who, bar_msg);
            checkMailbox(mb, false, true, false, false, 0);

            checkDeliver(mb, false, fred_who, bar_msg);
            checkMailbox(mb, false, true, false, false, 0);
        }


        [Test]
        public void read1()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, alice_who, bar_msg);
            checkMailbox(mb, false, true, false, false, 0);

            checkRead(mb, true, alice_who, bar_msg);
            checkMailbox(mb, true, false, false, false, 0);

            checkCloseDelivery(mb, true);
            checkMailbox(mb, true, false, true, true, 0);
        }


        [Test]
        public void read2()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, alice_who, bar_msg);
            checkMailbox(mb, false, true, false, false, 0);

            checkCloseDelivery(mb, true);
            checkMailbox(mb, false, true, true, true, 0);

            checkRead(mb, true, alice_who, bar_msg);
            checkMailbox(mb, true, false, true, true, 0);
        }


        [Test]
        public void read3()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, alice_who, bar_msg);
            checkMailbox(mb, false, true, false, false, 0);

            checkCloseDelivery(mb, true);
            checkMailbox(mb, false, true, true, true, 0);

            checkRead(mb, true, alice_who, bar_msg);
            checkMailbox(mb, true, false, true, true, 0);
        } 


        [Test]
        public void read4()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkCloseDelivery(mb, true);
            checkMailbox(mb, true, false, true, true, 0);

            checkRead(mb, false, null, null);
            checkMailbox(mb, true, false, true, true, 0);
        } 

        
        [Test]
        [Ignore]
        public void read5()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            Thread.Sleep(1000);

            checkRead(mb, false, null, null);
            checkMailbox(mb, true, false, true, true, 0);
        }


       
        [Test]
        [Ignore]
        public void read6()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkRead(mb, false, null, null);
            checkMailbox(mb, true, false, true, true, 0);
        }

        
        [Test]
        public void read7()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkRead(mb, -1, false, null, null);
            checkMailbox(mb, true, false, false, false, 0);
        }

        
        [Test]
        public void read8()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkRead(mb, 1, false, null, null);
            checkMailbox(mb, true, false, false, false, 0);
        }


        [Test]
        public void deliver1()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            checkMailbox(mb, true, false, false, false, 0);

            checkCloseDelivery(mb, true);
            checkMailbox(mb, true, false, true, true, 0);

            checkDeliver(mb, false, fred_who, bar_msg);
            checkMailbox(mb, true, false, true, true, 0);
        }


        [Test]
        public void notify1()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            notify.checkMailboxStatus(false, null, null, false);

            Object state = new Object();

            mb.RegisterNotify(notify, state, 0);
            notify.checkMailboxStatus(false, null, null, false);

            checkCloseDelivery(mb, true);
            notify.checkMailboxStatus(true, mb, state, true);
        }


        [Test]
        public void notify2()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            notify.checkMailboxStatus(false, null, null, false);

            Object state = new Object();

            mb.RegisterNotify(notify, state, 1000);
            notify.checkMailboxStatus(false, null, null, false);

            Thread.Sleep(2000);
            notify.checkMailboxStatus(true, mb, state, true);
        }


        [Test]
        public void notify3()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            notify.checkMailboxStatus(false, null, null, false);

            Object state = new Object();

            mb.RegisterNotify(notify, state, 0);
            notify.checkMailboxStatus(false, null, null, false);

            Thread.Sleep(2000);
            notify.checkMailboxStatus(false, null, null, false);
        }


        [Test]
        public void notify4()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            notify.checkMailboxStatus(false, null, null, false);

            Object state = new Object();

            mb.RegisterNotify(notify, state, 0);
            notify.checkMailboxStatus(false, null, null, false);

            mb.Message(alice_who, foo_msg);
            notify.checkMailboxStatus(true, mb, state, false);
        }

        [Test]
        [ExpectedException(typeof(ArgumentNullException))]
        public void reg1()
        {
            // notify == null
            PlainMailbox mb = new PlainMailbox(this, 1L);
            mb.RegisterNotify(null, null, 0);
        }

        [Test]
        [ExpectedException(typeof(ArgumentException))]
        public void reg2()
        {
            // maxDelay < 0
            PlainMailbox mb = new PlainMailbox(this, 1L);
            mb.RegisterNotify(notify, null, -1);
        }

        [Test]
        public void reg3()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            mb.RegisterNotify(notify, null, 0);
        }

        [Test]
        [ExpectedException(typeof(Exception))]
        public void reg4()
        {
            // this.notify != null
            PlainMailbox mb = new PlainMailbox(this, 1L);
            mb.RegisterNotify(notify, null, 0);
            mb.RegisterNotify(notify, null, 0);
        }

        [Test]
        [ExpectedException(typeof(Exception))]
        public void reg5()
        {
            // this.notify != null
            PlainMailbox mb = new PlainMailbox(this, 1L);
            mb.RegisterNotify(notify, null, 0);
            mb.RegisterNotify(notify1x, null, 0);
        }

        [Test]
        public void unreg1()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            mb.UnregisterNotify(notify);
        }

        [Test]
        public void unreg2()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            mb.RegisterNotify(notify, null, 0);
            mb.UnregisterNotify(notify);
        }

        [Test]
        public void unreg3()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            mb.RegisterNotify(notify, null, 0);
            mb.UnregisterNotify(notify);
            mb.UnregisterNotify(notify);
            mb.UnregisterNotify(notify1x);
        }

        [Test]
        public void unreg4()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            mb.RegisterNotify(notify, null, 0);
            mb.UnregisterNotify(notify);
            mb.RegisterNotify(notify, null, 0);
            mb.UnregisterNotify(notify);
            mb.RegisterNotify(notify1x, null, 0);
            mb.UnregisterNotify(notify1x);
        }

        [Test]
        [ExpectedException(typeof(ArgumentException))]
        public void unreg5()
        {
            // notify != this.notify
            PlainMailbox mb = new PlainMailbox(this, 1L);
            mb.RegisterNotify(notify, null, 0);
            mb.UnregisterNotify(notify1x);
        }

        [Test]
        public void unreg6()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L);
            mb.RegisterNotify(notify, null, 30);
            mb.UnregisterNotify(notify);
        }

        ///////////////////
        // HELPFUL STUFF //
        ///////////////////

        private static ValueFactory vf;

        private static XType mt_foo;

        private Message foo_msg;

        private Who fred_who;

        private static XType mt_bar;

        private Message bar_msg;

        private Who alice_who;

        private MyNotify notify;

        private MyNotify notify1x;

        private void testConstruct(long messageId)
        {
            PlainMailbox mb = new PlainMailbox(this, messageId);
            Assert.AreEqual(this, mb.GetMailboxManager());
            Assert.AreEqual(messageId, mb.GetMessageId());
        }

        private void checkDeliver(PlainMailbox mb, bool handled, Who who,
            Message msg)
        {
            Assert.AreEqual(handled, mb.Message(who, msg));
        }

        private void checkRead(PlainMailbox mb, bool present, Who who, Message msg)
        {
            Element e = mb.Read();
            if (present)
                checkElement(e, who, msg);
            else
                Assert.IsNull(e);
        }

        private void checkRead(PlainMailbox mb, int maxDelay, bool present,
            Who who, Message msg)
        {
            Element e = mb.Read(maxDelay);
            if (present)
                checkElement(e, who, msg);
            else
                Assert.IsNull(e);
        }

        private void checkRedelivered(int index, Who who, Message msg)
        {
            Element e = redelivered[index];
            checkElement(e, who, msg);
        }

        private void checkElement(Element e, Who who, Message msg)
        {
            Assert.IsNotNull(e);
            Assert.AreSame(who, e.sender);
            Assert.AreSame(msg, e.msg);
        }

        private void checkMailbox(PlainMailbox mb, bool empty, bool full,
            bool closed, bool unreg, int size)
        {
            Assert.AreEqual(empty, mb.IsEmpty());
            Assert.AreEqual(full, mb.IsFull());
            Assert.AreEqual(closed, mb.IsClosed());
            Assert.AreEqual(unreg, unregistered);
            Assert.AreEqual(size, redelivered.Count);
        }

        private void checkCloseRead(PlainMailbox mb, bool closed)
        {
            Assert.AreEqual(closed, mb.CloseRead());
        }

        private void checkCloseDelivery(PlainMailbox mb, bool closed)
        {
            Assert.AreEqual(closed, mb.CloseDelivery());
        }

        ////////////////////////////////
        // MailboxManagerIntf methods //
        ////////////////////////////////

        public void Redeliver(Who sender, Message msg)
        {
            redelivered.Add(new Element(sender, msg));
        }

        private  List<Element> redelivered;

        public void Unregister(Mailbox mb)
        {
            unregistered = true;
        }

        private bool unregistered;

        ////////////////////
        // MyValueFactory //
        ////////////////////

        public class MyValueFactory : ValueFactory
        {
            public StructValue ExportCustomValue(Object value)
            {
                throw new NotSupportedException();
            }

            public XType GetCustomStructType(Type c)
            {
                throw new NotSupportedException();

            }

            public long? GetInReplyTo(Message msg)
            {
                throw new NotSupportedException();

            }

            public long? GetMessageId(Message msg)
            {
                throw new NotSupportedException();

            }

            public Encoding GetStringEncoding()
            {
                throw new NotSupportedException();

            }

            public XType GetType(int id)
            {
                throw new NotSupportedException();

            }

            public XType GetType(String name)
            {
                throw new NotSupportedException();

            }

            public ICollection<XType> GetTypes()
            {
                throw new NotSupportedException(); ;
            }

            public XType Get_mt__Etch_AuthException()
            {
                throw new NotSupportedException();

            }

            public XType Get_mt__Etch_RuntimeException()
            {
                throw new NotSupportedException(); ;
            }

            public XType get_mt__exception()
            {
                throw new NotSupportedException();
            }

            public Object ImportCustomValue(StructValue sv)
            {
                throw new NotSupportedException();
            }

            public void SetInReplyTo(Message msg, long? msgid)
            {
                throw new NotSupportedException(); ;
            }

            public void SetMessageId(Message msg, long? msgid)
            {
                throw new NotSupportedException(); ;
            }


            public void AddType(XType type)
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void LockDynamicTypes()
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public void UnlockDynamicTypes()
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public Field Get_mf__messageId()
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public Field Get_mf__inReplyTo()
            {
                throw new Exception("The method or operation is not implemented.");
            }

            public Validator.Level GetLevel()
            {
                return Validator.Level.FULL;
            }

            public Validator.Level SetLevel(Validator.Level level)
            {
                throw new Exception("The method or operation is not implemented.");
            }
        }

        public class WhoAmI : Who
        {
            
        }

        public class MyNotify : Notify
        {
            public void mailboxStatus(Mailbox mb, Object state, bool closed)
            {
                mailboxStatus1 = true;
                this.mailbox = mb;
                this.state = state;
                this.closed = closed;
            }

            private bool mailboxStatus1;

            private Mailbox mailbox;

            private Object state;

            private bool closed;

            public void checkMailboxStatus(bool mailboxStatus, Mailbox mailbox,
                Object state, bool closed)
            {
                Assert.AreEqual(mailboxStatus, this.mailboxStatus1);
                Assert.AreSame(mailbox, this.mailbox);
                Assert.AreSame(state, this.state);
                Assert.AreEqual(closed, this.closed);
            }
        }

        #region MailboxManager Members


        public Mailbox TransportCall(Who recipient, Message msg)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        #endregion

        #region TransportMessage Members

        public void TransportMessage(Who recipient, Message msg)
        {
            throw new Exception("The method or operation is not implemented.");
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
    }
}


   
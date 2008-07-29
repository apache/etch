using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using Etch.Msg;
using Etch.Transport;
using Etch.Support;
using Etch.Util;
using NUnit.Framework;

namespace Etch.Support.Test
{
    [TestFixture]
    public class TestPlainMailbox : MailboxManager, Notify
    {

        [SetUp]
        public void Init()
        {
            mailboxStatus1 = false;
             mailbox = null;

             state = null;

            closed = false;
            unregistered = false;

       //     AlarmManager.SetAlarmManager(null);

            redelivered = new List<Element>();

            vf = new MyValueFactory();
	
            mt_foo = new XType( "foo" );
	
            foo_msg = new Message( mt_foo, vf );

            fred_who = new WhoAmI();
	
            mt_bar = new XType( "bar" );
	
            bar_msg = new Message( mt_bar, vf );
	
            alice_who = new WhoAmI(); 
        }
        
        [Test]
        public void construct1()
        {
            testConstruct(1L, 0, 0, 1);

            // variations on maxMessageDelay.
            testConstruct(2L, 1, 0, 1);
            testConstruct(3L, -1, 0, 1);

            // variations on lifetime.
            testConstruct(4L, 0, 1, 1);

            // variations on maxMessages.
            testConstruct(5L, 0, 0, 2);
        }


        [Test]
        [ExpectedException(typeof(NullReferenceException))]
        public void construct2()
        {
            new PlainMailbox(null, 1L, 0, 0, 1);
        }


        [Test]
        [ExpectedException(typeof(NullReferenceException))]
        public void construct3()
        {
            new PlainMailbox(this, null, 0, 0, 1);
        }


        [Test]
        [ExpectedException(typeof(ArgumentException))]
        public void construct4()
        {
            new PlainMailbox(this, 1L, 0, -1, 1);
        }


        [Test]
        [ExpectedException(typeof(ArgumentException))]
        public void construct5()
        {
            new PlainMailbox(this, 1L, 0, 0, 0);
        }


        [Test]
        public void closeDelivery1()
        {
            // open mailbox and close it for delivery while empty.

            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailbox(mb, true, false, false, false, 0);

            checkCloseDelivery(mb, true);
            checkMailbox(mb, true, false, true, true, 0);
        }


        [Test]
        public void closeDelivery2()
        {
            // open mailbox and close it for delivery while not empty.

            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, alice_who, foo_msg);
            checkMailbox(mb, false, false, false, false, 0);

            checkCloseDelivery(mb, true);
            checkMailbox(mb, false, false, true, true, 0);
        }


        [Test]
        public void closeDelivery3()
        {
            // open mailbox and close it for delivery twice.

            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
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

            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailbox(mb, true, false, false, false, 0);

            checkCloseRead(mb, true);
            checkMailbox(mb, true, false, true, true, 0);
        }


        [Test]
        public void closeRead2()
        {
            // open mailbox and close it for reading while not empty.

            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, alice_who, foo_msg);
            checkMailbox(mb, false, false, false, false, 0);

            checkCloseRead(mb, true);
            checkMailbox(mb, true, false, true, true, 1);
            checkRedelivered(0, alice_who, foo_msg);
        }


        [Test]
        public void closeRead3()
        {
            // open mailbox and close it for reading twice while empty.

            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
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

            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, alice_who, foo_msg);
            checkMailbox(mb, false, false, false, false, 0);

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
            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, fred_who, foo_msg);
            checkMailbox(mb, false, false, false, false, 0);

            checkDeliver(mb, true, alice_who, bar_msg);
            checkMailbox(mb, false, true, false, false, 0);
        }


        [Test]
        public void full2()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, fred_who, foo_msg);
            checkMailbox(mb, false, false, false, false, 0);

            checkDeliver(mb, true, alice_who, bar_msg);
            checkMailbox(mb, false, true, false, false, 0);

            checkDeliver(mb, false, fred_who, bar_msg);
            checkMailbox(mb, false, true, false, false, 0);
        }


        [Test]
        public void read1()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, alice_who, bar_msg);
            checkMailbox(mb, false, false, false, false, 0);

            checkRead(mb, true, alice_who, bar_msg);
            checkMailbox(mb, true, false, false, false, 0);

            checkCloseDelivery(mb, true);
            checkMailbox(mb, true, false, true, true, 0);
        }


        [Test]
        public void read2()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, alice_who, bar_msg);
            checkMailbox(mb, false, false, false, false, 0);

            checkCloseDelivery(mb, true);
            checkMailbox(mb, false, false, true, true, 0);

            checkRead(mb, true, alice_who, bar_msg);
            checkMailbox(mb, true, false, true, true, 0);
        }


        [Test]
        public void read3()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailbox(mb, true, false, false, false, 0);

            checkDeliver(mb, true, alice_who, bar_msg);
            checkMailbox(mb, false, false, false, false, 0);

            checkCloseDelivery(mb, true);
            checkMailbox(mb, false, false, true, true, 0);

            checkRead(mb, true, alice_who, bar_msg);
            checkMailbox(mb, true, false, true, true, 0);
        } 


        [Test]
        public void read4()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailbox(mb, true, false, false, false, 0);

            checkCloseDelivery(mb, true);
            checkMailbox(mb, true, false, true, true, 0);

            checkRead(mb, false, null, null);
            checkMailbox(mb, true, false, true, true, 0);
        } 

        
        [Test]
        public void read5()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 20, 2);
            checkMailbox(mb, true, false, false, false, 0);

            Thread.Sleep(1000);

            checkRead(mb, false, null, null);
            checkMailbox(mb, true, false, true, true, 0);
        }


       
        [Test]
        public void read6()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 100, 2);
            checkMailbox(mb, true, false, false, false, 0);

            checkRead(mb, false, null, null);
            checkMailbox(mb, true, false, true, true, 0);
        }

        
        [Test]
        public void read7()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailbox(mb, true, false, false, false, 0);

            checkRead(mb, -1, false, null, null);
            checkMailbox(mb, true, false, false, false, 0);
        }

        
        [Test]
        public void read8()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailbox(mb, true, false, false, false, 0);

            checkRead(mb, 1, false, null, null);
            checkMailbox(mb, true, false, false, false, 0);
        }


        [Test]
        public void deliver1()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailbox(mb, true, false, false, false, 0);

            checkCloseDelivery(mb, true);
            checkMailbox(mb, true, false, true, true, 0);

            checkDeliver(mb, false, fred_who, bar_msg);
            checkMailbox(mb, true, false, true, true, 0);
        }


        [Test]
        public void notify1()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailboxStatus(false, null, null, false);

            Object state = new Object();

            mb.RegisterNotify(this, state, 0);
            checkMailboxStatus(false, null, null, false);

            checkCloseDelivery(mb, true);
            checkMailboxStatus(true, mb, state, true);
        }


        [Test]
        public void notify2()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailboxStatus(false, null, null, false);

            Object state = new Object();

            mb.RegisterNotify(this, state, 1000);
            checkMailboxStatus(false, null, null, false);

            Thread.Sleep(2000);
            checkMailboxStatus(true, mb, state, true);
        }


        [Test]
        public void notify3()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailboxStatus(false, null, null, false);

            Object state = new Object();

            mb.RegisterNotify(this, state, 0);
            checkMailboxStatus(false, null, null, false);

            Thread.Sleep(20);
            checkMailboxStatus(false, null, null, false);
        }


        [Test]
        public void notify4()
        {
            PlainMailbox mb = new PlainMailbox(this, 1L, -1, 0, 2);
            checkMailboxStatus(false, null, null, false);

            Object state = new Object();

            mb.RegisterNotify(this, state, 0);
            checkMailboxStatus(false, null, null, false);

            mb.Message(alice_who, foo_msg);
            checkMailboxStatus(true, mb, state, false);
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

        private void testConstruct(long? messageId, int maxMessageDelay, int lifetime,
            int maxMessages)
        {
            PlainMailbox mb = new PlainMailbox(this, messageId, maxMessageDelay,
                lifetime, maxMessages);

            Assert.AreEqual(this, mb.GetMailboxManager());
            Assert.AreEqual(messageId, mb.GetMessageId());
            Assert.AreEqual(maxMessageDelay, mb.GetMaxMessageDelay());
            Assert.AreEqual(lifetime, mb.GetLifetime());
            Assert.AreEqual(maxMessages, mb.GetMaxMessages());
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
        // Notify methods //
        ////////////////////

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

        private void checkMailboxStatus(bool mailboxStatus, Mailbox mailbox,
            Object state, bool closed)
        {
            Assert.AreEqual(mailboxStatus, this.mailboxStatus1);
            Assert.AreSame(mailbox, this.mailbox);
            Assert.AreSame(state, this.state);
            Assert.AreEqual(closed, this.closed);
        }

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

            public XType GetType(int? id)
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
        }
        public class WhoAmI : Who
        {
            
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


   
using System;
using System.Text;
using System.Diagnostics;
using System.Threading;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using Etch.Msg;
using Etch.Support;
using Etch.Util;

namespace Etch.Transport
{
    /// <summary>
    /// A plain implementation of a mailbox using a fixed size circular queue.
    /// </summary>
    /// 
    public class PlainMailbox : Mailbox, AlarmListener
    {

        /// <summary>
        /// Constructs the mailbox
        /// </summary>
        /// <param name="mailboxManager">The mailbox manager to use to unregister this mailbox
        /// and to deliver undelivered messages.</param>
        /// <param name="messageId"></param>
        /// <param name="maxMessageDelay"> the maximum delay in milliseconds to wait when 
        /// trying to put a message in the mailbox.</param>
        /// <param name="lifetime">time in milliseconds to keep the mailbox open.
        /// Lifetime of 0 means keep it open until explicitly closed.</param>
        /// <param name="maxMessages"></param>
        public PlainMailbox(MailboxManager mailboxManager, long? messageId, int maxMessageDelay,
            int lifetime, int maxMessages)
        {
            if (mailboxManager == null)
                throw new NullReferenceException("mailboxManager == null");

            if (messageId == null)
                throw new NullReferenceException("messageId == null");

            if (lifetime < 0)
                throw new ArgumentException("lifetime < 0");

            if (maxMessages < 1)
                throw new ArgumentException("maxMessages < 1");

            this.mailboxManager = mailboxManager;
            this.messageId = messageId;
            this.maxMessageDelay = maxMessageDelay;
            this.lifetime = lifetime;
            this.maxMessages = maxMessages;

            queue = new CircularQueue<Element>(maxMessages);

            if (lifetime > 0)
            {
                alarmSet = true;
                AlarmManager.staticAdd(this, null, lifetime);

            }


        }

        private readonly MailboxManager mailboxManager;

        private readonly long? messageId;

        private readonly int maxMessageDelay;

        private readonly int lifetime;

        private readonly CircularQueue<Element> queue;

        private readonly int maxMessages;

        private bool alarmSet;

        private Notify notify;

        private Object state;



        public MailboxManager GetMailboxManager()
        {
            return mailboxManager;
        }

        public long? GetMessageId()
        {
            return messageId;
        }


        public int GetMaxMessageDelay()
        {
            return maxMessageDelay;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>LifeTime of this mailbox</returns>
        public int GetLifetime()
        {
            return lifetime;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>Max messages of this mailbox</returns>
        public int GetMaxMessages()
        {
            return maxMessages;
        }


        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="msg"></param>
        /// Exception:
        ///     throws Exception
        public bool Message(Who sender, Message msg)
        {
            bool ok = queue.Put(new Element(sender, msg), maxMessageDelay);
            if (ok)
                fireNotify();
            return ok;
        }

        private void fireNotify()
        {
            Notify n;
            Object s;
            bool c;

            lock (queue)
            {
                n = notify;
                s = state;
                c = queue.IsClosed();
            }

            if (n != null)
                n.mailboxStatus(this, s, c);
        }


        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        /// Exception:
        ///     throws ThreadInterruptedException
        public Element Read()
        {
            Element elem = null;
            try
            {
                elem = queue.Get();
            }

            catch (Exception)
            {

            }
            return elem;
        }



        /// <summary>
        /// 
        /// </summary>
        /// <param name="maxDelay"></param>
        /// <returns></returns>
        /// Exception:
        ///     throw ThreadInterruptedException
        public Element Read(int maxDelay)
        {
            Element elem = null;
            try
            {
                elem = queue.Get(maxDelay);
            }

            catch (Exception)
            {

            }
            return elem;
        }

        public bool CloseDelivery()
        {


            lock (queue)
            {
                if (queue.IsClosed())
                    return false;


                if (alarmSet)
                {
                    alarmSet = false;
                    AlarmManager.staticRemove(this);
                }
                mailboxManager.Unregister(this);
                queue.Close();

            }


            fireNotify();
            return true;

        }

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        public bool CloseRead()
        {
            if (CloseDelivery())
            {
                Element mbe;
                while ((mbe = Read()) != null)
                    mailboxManager.Redeliver(mbe.sender, mbe.msg);
                return true;
            }

            return false;

        }



        public int Wakeup(AlarmManager manager, Object state, long due)
        {
            Console.WriteLine(" In wakeup");
            CloseDelivery();
            return 0;
        }

        public void RegisterNotify(Notify notify, Object state, int maxDelay)
        {
            if (notify == null)
                throw new NullReferenceException("notify == null");

            if (maxDelay < 0)
                throw new ArgumentException("maxDelay < 0");

            if (this.notify != null)
                throw new Exception("this.notify != null");

            bool isNotEmptyOrIsClosed;
            lock (queue)
            {
                this.notify = notify;
                this.state = state;

                if (maxDelay > 0)
                {
                    alarmSet = true;
                    AlarmManager.staticAdd(this, null, maxDelay);
                }

                isNotEmptyOrIsClosed = !queue.IsEmpty() || queue.IsClosed();
            }
            if (isNotEmptyOrIsClosed)
                fireNotify();
        }

        public void UnregisterNotify(Notify notify)
        {
            if (notify == null)
                throw new NullReferenceException("notify == null");

            if (notify != this.notify)
                throw new NullReferenceException("notify != this.notify");

            lock (queue)
            {
                if (alarmSet)
                {
                    alarmSet = false;
                    AlarmManager.staticRemove(this);
                }

                this.notify = null;
                this.state = null;
            }
        }



        public bool IsEmpty()
        {
            return queue.IsEmpty();
        }

        public bool IsClosed()
        {
            return queue.IsClosed();
        }

        public bool IsFull()
        {
            return queue.IsFull();
        }


    }
}

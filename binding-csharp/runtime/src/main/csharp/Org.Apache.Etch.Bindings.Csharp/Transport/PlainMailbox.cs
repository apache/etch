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

namespace Org.Apache.Etch.Bindings.Csharp.Transport
{
    /// <summary>
    /// A plain implementation of a mailbox using a fixed size circular queue.
    /// </summary>
    /// 
    public class PlainMailbox : Mailbox, AlarmListener
    {
        /// <summary>
        /// Constructs the PlainMailbox
        /// </summary>
        /// <param name="mailboxManager">The mailbox manager to use to unregister this mailbox
        /// and to deliver undelivered messages.</param>
        /// <param name="messageId"></param>
        public PlainMailbox(MailboxManager mailboxManager, long messageId)
        {
            if (mailboxManager == null)
                throw new NullReferenceException("mailboxManager == null");

            this.mailboxManager = mailboxManager;
            this.messageId = messageId;

            queue = new CircularQueue<Element>(1);
        }

        private readonly MailboxManager mailboxManager;

        private readonly long messageId;

        private readonly CircularQueue<Element> queue;

        private bool alarmSet;

        private Notify notify;

        private Object state;

        public MailboxManager GetMailboxManager()
        {
            return mailboxManager;
        }

        public long GetMessageId()
        {
            return messageId;
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
            bool ok = queue.Put(new Element(sender, msg), -1);
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
            //Console.WriteLine(" In wakeup");
            CloseDelivery();
            return 0;
        }

        public void RegisterNotify(Notify newNotify, Object state, int maxDelay)
        {
            if (newNotify == null)
                throw new ArgumentNullException("newNotify == null");

            if (maxDelay < 0)
                throw new ArgumentException("maxDelay < 0");

            bool isNotEmptyOrIsClosed;

            lock (queue)
            {
                if (this.notify != null)
                    throw new Exception("this.notify != null");

                this.notify = newNotify;
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

        public void UnregisterNotify(Notify oldNotify)
        {
            if (oldNotify == null)
                throw new ArgumentNullException("oldNotify == null");

            if (this.notify == null)
                return;

            lock (queue)
            {
                if (oldNotify != this.notify)
                    throw new ArgumentException("oldNotify != this.notify");

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

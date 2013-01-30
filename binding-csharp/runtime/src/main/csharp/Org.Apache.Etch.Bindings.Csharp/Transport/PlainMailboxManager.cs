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
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace Org.Apache.Etch.Bindings.Csharp.Transport
{
    /// <summary>
    /// MailboxManager is a MessageHandler which accepts packets for
    /// possible delivery to a mailbox, or to another message handler
    /// if an appropriate mailbox cannot be found. MailboxManager is
    /// forwarding them to another MessageSource. If requested, a
    /// mailbox is created with a message's msgid and added to the
    /// set of mailboxes waiting for messages.
    /// </summary>
    public class PlainMailboxManager : MailboxManager
    {
        /// <summary>
        /// Constructs the PlainMailboxManager.
        /// </summary>
        /// <param name="transport">a transport to send messages </param>
        /// <param name="uri">the uri of this transport stack</param>
        /// <param name="resources">the resources of this transport stack</param>
        public PlainMailboxManager(TransportMessage transport, string uri, Resources resources)
            : this(transport, new URL(uri), resources)
        {
            // nothing else.
        }

        /// <summary>
        /// Constructs the PlainMailboxManager.
        /// </summary>
        /// <param name="transport">a transport to send messages </param>
        /// <param name="uri">the uri of this transport stack</param>
        /// <param name="resources">the resources of this transport stack</param>
        public PlainMailboxManager(TransportMessage transport, URL uri, Resources resources)
        {
            this.transport = transport;
            transport.SetSession(this);
        }

        private readonly TransportMessage transport;

        public override string ToString()
        {
            return String.Format( "PlainMailboxManager/{0} ", transport );
        }
    	
	    private readonly IdGenerator idGen = new IdGenerator( DateTime.Now.Ticks, 37 );

        /////////////////////
        // Mailbox methods //
        /////////////////////

        private Dictionary<long, Mailbox> mailboxes =
            new Dictionary<long, Mailbox>();

        /// <summary>
        /// Adds a mailbox to the set of mailbox receiving responses
        /// to messages.
        /// </summary>
        /// <param name="mb"></param>
        public void Register( Mailbox mb )
	    {
            long msgid = mb.GetMessageId();
		    lock (mailboxes)
		    {
                if (!up)
                    throw new InvalidOperationException("connection down");

			    if (mailboxes.ContainsKey( msgid ))
				    throw new ArgumentException( "dup msgid in mailboxes" );
    			
			    mailboxes.Add( msgid, mb );
		    }
	    }
       
        public void Unregister( Mailbox mb )
        {
            lock ( mailboxes )
            {
                mailboxes.Remove( mb.GetMessageId() );
            }
        }

        private Mailbox GetMailbox( long msgid )
        {
            lock ( mailboxes )
            {
                return mailboxes[msgid];
            }
        }
      
        public void Redeliver( Who sender, Message msg ) 
        {
            session.SessionMessage(sender, msg);
        }

        public Object SessionQuery( Object query )
        {
            return session.SessionQuery( query );
        }

        public void SessionControl( Object control, Object value )
        {
            session.SessionControl( control, value );
        }

        public void SessionNotify( Object eventObj )
        {
           if(eventObj.Equals(SessionConsts.UP))
           {
               up = true;
           }
           else if (eventObj.Equals(SessionConsts.DOWN))
           {
               up = false;
               UnRegisterAll();
           }
            session.SessionNotify(eventObj);
        }

        private bool up;

        public Object TransportQuery( Object query )
        {
            return transport.TransportQuery( query );
        }

        public void TransportControl( Object control, Object value )
        {
            transport.TransportControl( control, value );
        }

        public void TransportNotify( Object eventObj )
        {
            transport.TransportNotify( eventObj );
        }

        public void UnRegisterAll()
        {
            Mailbox[] mbs;
            lock (mailboxes) 
            {
                mbs = new Mailbox[mailboxes.Values.Count];
                mailboxes.Values.CopyTo(mbs, 0);
            }
            foreach (Mailbox mb in mbs)
            {
                mb.CloseDelivery();
            }
        }


        #region SessionMessage Members

        public bool SessionMessage(Who sender, Message msg)
        {
            long? msgid = msg.InReplyTo;
            if (msgid != null)
            {
                Mailbox mb;
                try
                {
                    mb = GetMailbox(msgid.Value);
                }
                catch
                {
                    mb = null;
                }
                if (mb != null)
                {
                    try
                    {
                        return mb.Message(sender, msg);

                    }
                    catch (Exception)
                    {
                        // timeout or mailbox closed - fall through
                    }
                }
                // no such mailbox - fall through
                return false;
            }
            // no such mailbox or no msgid - fall through
            return session.SessionMessage(sender, msg);

          
        }

        #endregion

        #region MailboxManager Members


        public Mailbox TransportCall(Who recipient, Message msg)
        {
            if (msg.MessageId != null)
                throw new Exception(" message has already been sent");
            if (msg.InReplyTo != null)
                throw new Exception(" message is marked as a reply");

            long mid = idGen.Next();

            msg.MessageId = mid;
            Mailbox mb = new PlainMailbox(this, mid);
            Register(mb);
            try
            {
                transport.TransportMessage(recipient, msg);
            }
            catch(Exception e)
            {
                Unregister(mb);
                throw e;
            }
            return mb;
        }

        #endregion

        #region TransportMessage Members

        public void TransportMessage(Who recipient, Message msg)
        {
            transport.TransportMessage(recipient,msg);
        }

        #endregion

        #region Transport<SessionMessage> Members


        public void SetSession(SessionMessage session)
        {
            this.session = session;
        }

        public SessionMessage GetSession()
        {
            return this.session;
        }

        private SessionMessage session;

        #endregion
    }
}

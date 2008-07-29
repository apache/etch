using System;
using System.Text;
using System.Net;
using System.Collections;
using System.Collections.Generic;
using Etch.Msg;
using Etch.Support;
using Etch.Util;

namespace Etch.Transport
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
        /// 
        /// </summary>
        /// <param name="transport">a transport to send messages </param>
        /// <param name="maxDelay">maximum amount of time to wait for a full mailbox
        /// to allow a new message before giving up and delivering to the
        /// message handler. 0 means wait forever, -1 means don't wait at all</param>
        public PlainMailboxManager( TransportMessage transport, int maxDelay )
        {
            this.transport = transport;
            this.maxDelay = maxDelay;
            transport.SetSession(this);
        }

        private TransportMessage transport;

        object lockObj = new object();

        public PlainMailboxManager(TransportMessage transport, string uri, Resources resources)
            :
           this(transport, new URL(uri), resources)
        {
        }


        private PlainMailboxManager(TransportMessage transport, URL uri, Resources resources)
            : 
            this( transport, 0 )
        {
        }

        private readonly int maxDelay;

        private SessionMessage session;

        

        public override string ToString()
        {
            return String.Format( "MailboxManager {0} ",transport );
        }

        ////////////////////////////
        // MessageHandler methods //
        ////////////////////////////

        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="msg"></param>
        /// Exception:
        ///     throws Exception
        //public bool Message( Who sender, Message msg ) 
        //{
        //    long? msgid = msg.InReplyTo;
        //    if (msgid != null)
        //    {
        //        Mailbox mb;
        //        try
        //        {
        //            mb = GetMailbox( msgid );
        //        }
        //        catch 
        //        {
        //            mb = null;
        //        }
        //        if (mb != null)
        //        {
        //            try
        //            {
        //                return mb.Message( sender, msg );
					    
        //            }
        //            catch ( Exception )
        //            {
        //                // timeout or mailbox closed - fall through
        //            }
        //        }
        //        // no such mailbox - fall through
        //    }
        //    // no such mailbox or no msgid - fall through
        //    Boolean delivered = session.SessionMessage( sender, msg );

        //    if ( !delivered )
        //        Console.WriteLine( "Mailbox.undelivered  | msg | " + msg );

        //    return delivered;
        //}

        ///////////////////////////
        // MessageSource methods //
        ///////////////////////////

        /// <summary>
        /// 
        /// </summary>
        /// <param name="recipient"></param>
        /// <param name="msg"></param>
        /// Exception:
        ///     throws Exception
        //public void Messagex( Who recipient, Message msg ) 
        //{
        //    msg.MessageId = idGen.Next();
        //    src.Messagex( recipient, msg );
        //}

        /// <summary>
        /// 
        /// </summary>
        /// <param name="recipient"></param>
        /// <param name="msg"></param>
        /// <param name="maxMessages"></param>
        /// <param name="lifetime"></param>
        /// <returns></returns>
        /// Exception:
        ///     throws Exception
        //public Mailbox Message( Who recipient, Message msg, int maxMessages,
        //                    int lifetime ) 
        //{
        //    if (lifetime < 0)
        //        throw new Exception( "lifetime < 0" );
    		
        //    long msgid = idGen.Next();
        //    msg.MessageId = msgid;
    		
        //    Mailbox mb =
        //        new PlainMailbox( this, (long?) msgid, maxDelay, lifetime, maxMessages );
        //    Register( mb );
    		
        //    try
        //    {
        //        src.Messagex( recipient, msg );
        //    }
        //    catch ( Exception e )
        //    {
        //        Unregister( mb );
        //        throw e;
        //    }
    		
        //    //return (Mailbox<MessageSource>) mb;
        //    return mb;
        //}
    	
	    private readonly IdGenerator idGen = new IdGenerator( (long) DateTime.Now.Millisecond, 37 );

      

        /////////////////////
        // Mailbox methods //
        /////////////////////

        private Dictionary<long?, Mailbox> mailboxes = 
            new Dictionary<long?, Mailbox>();


        /// <summary>
        /// Adds a mailbox to the set of mailbox receiving responses
        /// to messages.
        /// </summary>
        /// <param name="mb"></param>
        public void Register( Mailbox mb )
	    {
            long? msgid = mb.GetMessageId();
		    lock (mailboxes)
		    {
			    if (mailboxes.ContainsKey( msgid ))
				    throw new Exception( "dup msgid in mailboxes" );
    			
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

        private Mailbox GetMailbox( long? msgid )
        {
            lock ( mailboxes )
            {
                return mailboxes[ msgid ];
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
            lock (lockObj) 
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
                    mb = GetMailbox(msgid);
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

            msg.MessageId = idGen.Next();
            Mailbox mb =
                new PlainMailbox(this, (long?)msg.MessageId, maxDelay, 0, 1);
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


        #endregion
    }
}

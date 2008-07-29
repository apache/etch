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
    /// also a MessageSource, tagging messages with msgids and then
    /// forwarding them to another MessageSource. If requested, a
    /// mailbox is created with a message's msgid and added to the
    /// set of mailboxes waiting for messages.
    /// </summary>
    public class MailboxManager : MessageHandler<MessagizerMessageSource>, MailboxMessageSource
    {
        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="defaultHandler">a message handler to receive messages which
        /// cannot be delivered to a mailbox.</param>
        /// <param name="maxDelay">maximum amount of time to wait for a full mailbox
        /// to allow a new message before giving up and delivering to the
        /// message handler. 0 means wait forever, -1 means don't wait at all</param>
        public MailboxManager( MessageHandler<MailboxMessageSource> defaultHandler, int maxDelay )
        {
            this.handler = defaultHandler;
            this.maxDelay = maxDelay;
        }

        public MailboxManager( URL uri, Resources resources ) : 
            this( null, 0 )
        {
        }

        private readonly int maxDelay;

        

        public override string ToString()
        {
            return String.Format( "MailboxManager {0} ", src );
        }

        ////////////////////////////
        // MessageHandler methods //
        ////////////////////////////

        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="src"></param>
        /// <param name="sender"></param>
        /// <param name="msg"></param>
        /// Exception:
        ///     throws Exception
        public bool Message( Who sender, Message msg ) 
	    {
            long? msgid = msg.InReplyTo;
		    if (msgid != null)
		    {
                Mailbox mb;
                try
                {
                    mb = GetMailbox( msgid );
                }
                catch 
                {
                    mb = null;
                }
			    if (mb != null)
			    {
				    try
				    {
					    return mb.Message( sender, msg );
					    
				    }
				    catch ( Exception )
				    {
					    // timeout or mailbox closed - fall through
				    }
			    }
			    // no such mailbox - fall through
		    }
		    // no such mailbox or no msgid - fall through
            Boolean delivered = handler.Message( sender, msg );

            if ( !delivered )
                Console.WriteLine( "Mailbox.undelivered  | msg | " + msg );

            return delivered;
	    }

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
        public void Messagex( Who recipient, Message msg ) 
	    {
            msg.MessageId = idGen.Next();
            src.Messagex( recipient, msg );
	    }

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
        public Mailbox Message( Who recipient, Message msg, int maxMessages,
		                    int lifetime ) 
	    {
		    if (lifetime < 0)
			    throw new Exception( "lifetime < 0" );
    		
		    long msgid = idGen.Next();
            msg.MessageId = msgid;
    		
		    Mailbox mb =
			    new PlainMailbox( this, (long?) msgid, maxDelay, lifetime, maxMessages );
		    Register( mb );
    		
		    try
		    {
			    src.Messagex( recipient, msg );
		    }
		    catch ( Exception e )
		    {
			    Unregister( mb );
			    throw e;
		    }
    		
		    //return (Mailbox<MessageSource>) mb;
            return mb;
	    }
    	
	    private readonly IdGenerator idGen = new IdGenerator( (long) DateTime.Now.Millisecond, 37 );

        /// <summary>
        /// 
        /// </summary>
        /// <param name="reset"></param>
        /// Exception:
        ///     throws Exception
	    // public void Close( bool reset ) 
	    // {
		//     msgSrc.Close( reset );
        // }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        /// Exception:
        ///     throws Exception
	    // public EndPoint LocalAddress() 
	    // {
		//     return msgSrc.LocalAddress();
        // }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        /// Exception:
        ///     throws Exception
	    // public EndPoint RemoteAddress() 
	    // {
		//     return msgSrc.RemoteAddress();
        // }

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
	    // public void ShutdownInput()
	    // {
		//     msgSrc.ShutdownInput();
        // }

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
	    // public void ShutdownOutput() 
	    // {
		//     msgSrc.ShutdownOutput();
        // }

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
	    // public void Stop() 
	    // {
		//     msgSrc.Stop();
        // }

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

        /// <summary>
        /// Removes the mailbox from the set of mailbox receiving responses
        /// to messages.
        /// </summary>
        /// <param name="mb"></param>
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

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="msg"></param>
        /// Exception:
        ///     throws Exception
        public void Redeliver( Who sender, Message msg ) 
        {
            handler.Message(sender, msg);
        }

        ///////////////////////////
        // SourceHandler methods //
        ///////////////////////////

        public MessagizerMessageSource GetSource()
        {
            return src;
        }


        public void SetSource( MessagizerMessageSource src )
        {
            this.src = src;
        }

        private MessagizerMessageSource src;

        public Object SessionQuery( Object query )
        {
            return handler.SessionQuery( query );
        }

        public void SessionControl( Object control, Object value )
        {
            handler.SessionControl( control, value );
        }

        public void SessionNotify( Object eventObj )
        {
            handler.SessionNotify( eventObj );
        }

        ////////////////////
        // Source methods //
        ////////////////////

        public Object GetHandler()
        {
            return handler;
        }

        public void SetHandler(  Object handler )
        {
            this.handler = ( MessageHandler<MailboxMessageSource> ) handler;
            ( ( MessageHandler<MailboxMessageSource> ) handler ).SetSource( this );
        }

        private MessageHandler<MailboxMessageSource> handler;

        public Object TransportQuery( Object query )
        {
            return src.TransportQuery( query );
        }

        public void TransportControl( Object control, Object value )
        {
            src.TransportControl( control, value );
        }

        public void TransportNotify( Object eventObj )
        {
            src.TransportNotify( eventObj );
        }

    }
}

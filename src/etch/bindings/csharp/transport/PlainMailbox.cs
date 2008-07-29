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
    /// <typeparam name="S">the message source for the message handler.</typeparam>
    public class PlainMailbox : Mailbox, AlarmListener 
    {

        /// <summary>
        /// Constructs the mailbox
        /// </summary>
        /// <param name="mgr"></param>
        /// <param name="msgid"></param>
        /// <param name="maxMessageDelay"></param>
        /// <param name="lifetime">time in milliseconds to keep the mailbox open.
        /// Lifetime of 0 means keep it open until explicitly closed.</param>
        /// <param name="maxMessages"></param>
        public PlainMailbox( MailboxManager mgr, long? msgid, int maxMessageDelay,
            int lifetime, int maxMessages )
	    {
		    if (lifetime < 0)
			    throw new ArgumentException( "lifetime < 0" );
    		
		    if (maxMessages < 1)
			    throw new ArgumentException( "maxMessages < 1" );
    		
		    this.mgr = mgr;
		    this.msgid = msgid;
		    this.maxMessageDelay = maxMessageDelay;
		    this.lifetime = lifetime;

            if ( lifetime > 0 )
            {
                //AlarmManager.StaticAdd( this, null, lifetime );
                throw new Exception( "Alarm manager not implemented" );
            }
    		
		    queue = new CircularQueue<Element>( maxMessages );
	    }
    	
	    private readonly MailboxManager mgr;
    	
	    private readonly long? msgid;
    	
	    private readonly int maxMessageDelay;
    	
	    private readonly int lifetime;

        private readonly CircularQueue<Element> queue;

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
		    return queue.Put( new Element( sender, msg ), maxMessageDelay );
	    }
        
        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        /// Exception:
        ///     throws ThreadInterruptedException
        public Element Read() 
	    {
            return queue.Get();
	    }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="maxDelay"></param>
        /// <returns></returns>
        /// Exception:
        ///     throw ThreadInterruptedException
        public Element Read( int maxDelay )
	    {
		    return queue.Get( maxDelay );
	    }

        public void CloseDelivery()
        {
            if ( lifetime > 0 )
            {
                //AlarmManager.StaticRemove( this );
                throw new Exception( "AlarmManager not implemented" );
            }
            mgr.Unregister(  this );
            queue.Close();
        }

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        public void CloseRead() 
	    {
		    CloseDelivery();
		    Element mbe;
		    while ((mbe = Read()) != null)
			    mgr.Redeliver( mbe.sender, mbe.msg );
	    }

        /*
        public int Wakeup( AlarmManager manager, Object state, long due )
        {
            CloseDelivery();
            return 0;
        }
        */

        public long? GetMessageId()
        {
            return msgid;
        }
    }
}

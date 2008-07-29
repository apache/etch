///$Id$
///
///Created by Champakesan, Badri Narayanan on Jun 25, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.


using System;
using Etch.Msg;
using Etch.Transport;

namespace Etch.Support
{
    /// <summary>An interface used to deliver responses to a message. Support for the mechanism is somewhat 
    /// dependent upon properties of the transport and message format.</summary>
    /// 
    public interface Mailbox 
    {
        /// <returns>the message id of this mailbox.</returns>
        long? GetMessageId();

        /// <returns>the next message to be read from the mailbox, or null if the mailbox is empty and closed. 
        /// Wait forever for such a message to be delivered.</returns>
        /// Exception: 
        ///             throws InterruptedException thread interrupt.
        Element Read();

       /// <param name="maxDelay">maxDelay the maximum amount of time in milliseconds to wait to read a message 
       /// from an empty mailbox. 0 means wait forever, -1 means don't wait at all.</param>
       /// <returns>the message read from the mailbox, or null if the mailbox is empty and closed, or if the time 
       /// limit was exceeeded.</returns>
       /// Exception:
       ///              throws InterruptedException thread interrupt.
       Element Read(int maxDelay);

       /// <summary>Closes the mailbox so that no more messages can be delivered. Queued messages remain to be read. 
       /// Reading an empty closed mailbox returns null.</summary>
       /// <returns> true if this call closed the mailbox (that is, if action was taken), false if the mailbox was already closed. </returns>
       bool CloseDelivery();

       /// <summary>Closes the mailbox so that no more messages will be delivered or read. Any remaining queued 
       /// messages are delivered to a default handler.</summary>
       /// throws Exception 
       bool CloseRead();

       ///<summary>Registers a Notify interface implementation to receive a callback
	   ///when a mailbox's status is changed.</summary>
	   ///<param> notify a Notify interface implementation to report the
	   ///delivery status to. </param>
	   ///<param> state a state value to pass thru to the Notify interface
	   /// implementation. </param>
	   ///<param> maxDelay the maximum amount of time in milliseconds to
	   ///wait for delivery of a message to the mailbox. 0 means wait
	   ///forever. The mailbox is closed upon timeout.<param>
    	
	   void RegisterNotify( Notify notify, Object state, int maxDelay );
	
	
	   /// <summary>Unregisters a Notify interface implementation from receiving a callback
	   /// when a mailbox's status is changed. Cancels any timeout. </summary>
	   ///<param> notify a Notify interface implementation which was previously
	   ///registered. </param>    	
	   void UnregisterNotify( Notify notify );


        bool Message(Who sender, Message msg);





    }
    
        ///<summary>The message as queued, including src and sender.</summary>
        public class Element : Who
        {
            /// <param name="sender">sender the message sender.</param>
            /// <param name="msg">msg the message.</param>
            public Element(Who sender, Message msg)
            {
                this.sender = sender;
                this.msg = msg;
            }

            /// <summary>The message sender.</summary>
            public Who sender;

            /// <summary>the message.</summary>
            public Message msg;
        }

        public interface Notify
        {

		    /// <summary>Notifies of mailbox status change.</summary>
		    ///<param> mb the mailbox whose status has changed.</param>
		    ///<param> state the state object passed in the register
		    /// method.</param>
		    /// <param> closed true if the mailbox timeout has expired and
		    ///the mailbox is now closed to delivery, false if a message
		    /// has arrived.</param>
		 
            void mailboxStatus(Mailbox mb, Object state, bool closed);
            
        }
}
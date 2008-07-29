///$Id$
///Created by Champakesan, Badri Narayanan on Jun 13, 2007.
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

using System;

namespace Etch.Msg
{
    ///<summary> A message is modeled as a command and some argments. The command is an arbitrary
    /// integer value, and the arguments are key / value pairs, where the key is an arbitrary integer 
    /// value and the value is any one of the standard C# objects, an ArrayValue, a StructValue, or any type
    /// which may be serialized by the ValueFactory.</summary>
    public sealed class Message : StructValue
    {
        ///<summary>Constructs the Message.</summary>
        ///<param name="type">type of the message (command).</param> 
        ///<param name="vf"> vf the value factory.</param>
        public Message(XType type, ValueFactory vf) : base( type )
        {
            this.vf = vf;
        }

        ///<summary>Constructs the Message.</summary>
        ///<param name="type">type of the message (command).</param> 
        ///<param name="length"> the expected number of name/value pairs</param>
        ///<param name="vf">vf the value factory.</param>
        public Message(XType type, int length, ValueFactory vf)
            : base(type,length)
        {
            this.vf = vf;
        }



        private readonly ValueFactory vf;

        ///<returns>the value factory.</returns>     
        public ValueFactory _vf
        {
            get
            {
                return vf;
            }
        }

   
       /// <summary>Creates a message which is a reply to the current message. The current message's value 
       /// factory is copied to the new message. The message-id of the current message (if any) is copied into 
       /// the in-reply-to field of the new message.</summary>
       /// <param name="rType"> rType the type of the reply.</param>
       /// <returns> a reply message.</returns>       
        public Message Reply( XType rType )
        {
            Message rmsg = new Message( rType, vf );
            rmsg.InReplyTo = MessageId; //rmsg.InReplyTo(GetMessageId()); changed
            return rmsg;
        }

        public Message Reply()
        {
            Message rmsg = new Message( GetXType.GetResult(), vf );
            rmsg.InReplyTo = MessageId;
            return rmsg;
        }

        /// <returns>the connection specific unique identifier of this message, or null if there was no 
        /// such identifier.</returns>
        public long? MessageId
        {
            get
            {
                return vf.GetMessageId(this);
            }
            set
            {
                vf.SetMessageId(this, value);
            }
        }

       /// <returns>the message-id of the message that this is a response to. Null if this is an original message 
       /// or if the original message did not have a message-id.</returns>
        public long? InReplyTo
        {
            get
            {
                return vf.GetInReplyTo(this);
            }
            set
            {
                vf.SetInReplyTo(this, value);
            }
        }
    }
}

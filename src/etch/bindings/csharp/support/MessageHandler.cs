///$Id$
///
///Created by Champakesan, Badri Narayanan on Jun 22, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
using System;
using Etch.Msg;

namespace Etch.Support
{

   /// <summary>Interface used to deliver messages from a message source.</summary>
    public interface MessageHandler
    {
        /// <summary>Delivers a message from a message source.</summary>
        /// <param name="sender">sender the message sender (meaning depends upon the message source).</param>
        /// <param name="msg">msg the message from the message source.</param> throws Exception
        bool Message(Who sender, Message msg);
    }
}
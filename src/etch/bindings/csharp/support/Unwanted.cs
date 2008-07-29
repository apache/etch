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

    /// <summary>Interface which a service implementation may implement which is used by the service stub to give 
    /// notice of an unwanted message. This is a message which was not wanted by the service stubs (its id did not 
    /// match any service message).</summary>
    public interface Unwanted
    {
        /// <summary>Notifies the service implementation that the message is unwanted, that is, its id does 
        /// not match any defined</summary>
        /// <param name="sender">sender the transport defined sender.</param>
        /// <param name="msg">msg the message that was not wanted.</param>
        void _Unwanted( Who sender, Message msg);
    }
}
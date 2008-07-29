///$Id$
///
///Created by Champakesan, Badri Narayanan on Jun 22, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

using System;
using Etch.Msg;
using Etch.Transport;

namespace Etch.Support{

    ///<summary>Adapter between remote and message source.</summary>
    public interface DeliveryService : Etch.Transport.TransportMessage,SessionMessage
    {
	
        

        /// <summary>
        /// Sends the message which begins a call sequence.
        /// </summary>
        /// <param name="msg">the message to send.</param>
        /// <returns>a mailbox which can be used to read response</returns>
        Mailbox BeginCall( Message msg );

        /// <summary>Finishes a call sequence by waiting for the response message.</summary>
        /// <param name="mb"></param>
        /// <param name="responseType">responseType the type of the expected response.</param>
        /// which would contain any result value or thrown exception.</param>
	    /// <returns>the value of the response field if it isn't an exception.</returns>
        /// Exception:
        ///             throws Exception if there is a problem sending or a timeout waiting or if the result 
        ///             value was an exception.
        Object EndCall(Mailbox mb, XType responseType );

       
    }
}

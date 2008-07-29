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
    public interface DeliveryService : Etch.Transport.Transport
    {
	
        /// <summary>Sends the message to the recipient, but does not wait for any response.</summary>
        /// <param name="msg">msg the message to send</param>
        /// Exception:
        ///             throws Exception if there is a problem sending
	    void Send( Message msg );

        /// <summary>
        /// Sends the message which begins a call sequence.
        /// </summary>
        /// <param name="msg">the message to send.</param>
        /// <returns>a mailbox which can be used to read response</returns>
        Mailbox BeginCall( Message msg );

	    /// <param name="mb"></param>
        /// <param name="responseType">responseType the type of the expected response.</param>
	    /// <param name="responseField">responseField the field of the expected response 
        /// which would contain any result value or thrown exception.</param>
	    /// <param name="timeout">timeout time in ms to wait for a response.</param>
	    /// <returns>the value of the response field if it isn't an exception.</returns>
        /// Exception:
        ///             throws Exception if there is a problem sending or a timeout waiting or if the result 
        ///             value was an exception.
        Object EndCall(Mailbox mb, XType responseType,
		            Field responseField, int timeout );

    }
}

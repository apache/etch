///$Id$
///
///Created by Champakesan, Badri Narayanan on Jun 22, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.


using System;
using Etch.Transport;

namespace Etch.Support
{
/// <summary>Interface which a service implementation may implement which is used by the service stub to 
/// give notice of a closed message source.</summary>
    public interface Close
    {
	    /// <summary>Notifies the service implementation that the message source is closed.</summary>
	    /// <param name="src"> src the message source that is closed.</param>
	    void _Close( MessageSource src );
    }
}
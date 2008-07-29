using System;
using System.Text;
using System.Collections.Generic;

namespace Etch.Transport
{
    /// <summary>
    /// Interface used to deliver messages from the messagizer.
    /// </summary>
    public interface MessageHandler<S>  : SourceHandler<S>, Etch.Support.MessageHandler where S : Source
    {
        // nothing.
    }
}

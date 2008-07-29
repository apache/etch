using System;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;
using Etch.Support;

namespace Etch.Transport
{
    public interface TransportMessage : Transport<SessionMessage>
    {
        void TransportMessage(Who recipient, Message msg);
    }
}

using System;
using System.Collections.Generic;
using System.Text;
using Etch.Support;

namespace Etch.Transport
{
    public interface TransportPacket : Transport<SessionPacket>
    {

        int HeaderSize();
        void TransportPacket(Who recipient, FlexBuffer buf);
    }
}

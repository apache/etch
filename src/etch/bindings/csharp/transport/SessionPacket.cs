using System;
using System.Collections.Generic;
using System.Text;
using Etch.Support;

namespace Etch.Transport
{
    public interface SessionPacket : Session
    {
        void SessionPacket(Who sender, FlexBuffer buf);
    }
}

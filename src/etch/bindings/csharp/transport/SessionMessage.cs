using System;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;
using Etch.Support;

namespace Etch.Transport
{
    public interface SessionMessage : Session
    {
        bool SessionMessage(Who sender, Message msg);
    }
}

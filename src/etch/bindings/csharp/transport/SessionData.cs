using System;
using System.Collections.Generic;
using System.Text;
using Etch.Support;

namespace Etch.Transport
{
    public interface SessionData : Session
    {
        void SessionData(Who sender, FlexBuffer buf);
    }
}

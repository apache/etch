using System;
using System.Collections.Generic;
using System.Text;
using Etch.Support;

namespace Etch.Transport
{
    public interface TransportData : Transport<SessionData>
    {

        void TransportData(Who recipient, FlexBuffer buf);
    }
}

using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;

namespace Etch.Transport
{
    public interface SessionListener : Session
    {
        void SessionAccepted(Socket socket);
    }
}

using System;
using System.Collections.Generic;
using System.Text;
using Etch.Support;
using Etch.Msg;

namespace Etch.Transport
{
    public interface ServerFactory
    {
        void NewServer( DefaultDeliveryService d );

	    ValueFactory NewValueFactory();
    }
}

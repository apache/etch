using System;
using System.Collections.Generic;
using System.Text;
using Etch.Util;
using Etch.Msg;

namespace Etch.Transport
{
    public class DefaultDeliveryService : AbstractDeliveryService
    {

        public DefaultDeliveryService(MailboxManager transport, string uri, Resources resources ) : base(transport,uri,resources)
        {
           
        }

       
    }
}

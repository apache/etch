using System;
using System.Collections.Generic;
using System.Text;
using Etch.Support;
using Etch.Msg;

namespace Etch.Transport
{
    /// <summary>
    /// Interface to use for constructing new server instances by 
    /// TransportHelper
    /// </summary>
    public interface ServerFactory
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="d"> the delivery service to be used with the new service</param>
        void NewServer( DeliveryService d );

       
    }
}

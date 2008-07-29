///$Id$
///
///Created by Champakesan, Badri Narayanan on Jun 22, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

using System;

namespace Etch.Support
{

    /// <summary>
    /// Constructs the EtchRuntimeException. Don't init anything.
    /// </summary>
    public class _Etch_RuntimeException
        : Exception
    {
        
        /// <summary>
        /// Constructs the EtchRuntimeException. Don't init anything.
        /// </summary>
        public _Etch_RuntimeException()
        {
            // don't init anything.
        }

       /// <summary>Constructs the EtchRuntimeException</summary>
       /// <param name = "msg">msg description of the exception that was caught by the stub on the remote side.</param>
        public _Etch_RuntimeException(String msg)
        {
            this.msg = msg;
        }

       
        public String GetMessage()
        {
            return msg;
        }

        /// <summary>Description of the exception that was caught by the stub on the remote side. </summary>
        public String msg;
    }
}
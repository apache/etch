// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    public class RemoteBase
    {
        /// <summary>Constructs the RemoteBase.</summary>
        /// <param name="svc">svc</param>
        /// <param name="vf">vf </param>
        public RemoteBase(DeliveryService svc, ValueFactory vf)
        {
            this._svc = svc;
            this._vf = vf;
        }

       
        private DeliveryService _svc;

        /// <summary>The value factory used to create messages and encode/decode types.</summary>
        public ValueFactory _vf;

        public override string ToString()
        {
            return String.Format("{0}/{1}", GetType(), _svc);
        }

        /// <summary> Constructs a new message to send using _send(Message) or _beginCall(Message)</summary>
        /// <param name="type">type</param>
        /// <returns>a new message</returns>
        public Message _NewMessage(XType type)
        {
            return new Message(type, _vf);
        }

        	
	    ///<summary>Sends the message to the recipient, but does not wait for any response.</summary>
	    ///<param name = msg> the message to send </param>
	    ///<throws> Exception if there is a problem sending </throws>
	    public void _Send( Message msg )
	    {
		    _svc.TransportMessage(null,msg);
	    }
	
	
        ///<summary>Sends the message which begins a call sequence.</summary>
        ///<param name = msg> the message to send </param>
        ///<returns>A mailbox which can be used to read the response, using _EndCall</returns>
	    public Mailbox _BeginCall( Message msg )
	    {
		    return _svc.BeginCall( msg );
	    }

       
	    ///<summary>Finishes a call sequence by waiting for the response message.</summary>
	    ///<param name = mb>a mailbox which will be used to read the response, returned by
	    /// _BeginCall(Message) </param>
	    ///<param name = responseType> the type of the expected response </param>
	    ///<returns>the value of the response field if it isn't an exception.</returns>
	    ///<throws> Exception if there is a problem sending or a timeout waiting or
	    /// if the result value was an exception.</throws>
	    public Object _EndCall( Mailbox mb, XType responseType ) 
	    {
		    return _svc.EndCall( mb, responseType );
	    }


        /// <summary>
        /// Gets a configuration or operational value from the source. The
        /// request is passed down the chain of sources until some source
        /// recognises the query, whereupon it returns the requested value.
        /// </summary>
        /// 
        /// <param name="query">an object representing a query, which could be as
        /// simple as a string, integer, or enum, or more complex such as
        /// a class with instance variables for query terms.</param>
        /// <returns>the requested value, or null if not defined.</returns>
        /// 
        /// Exception:
        ///     throws Exception if the query is not recognised
        ///     by any source (which is to say, if the last source in the source
        ///     chain does not recognise it, it should throw this exception).
        ///     
        public object _TransportQuery( object query )
        {
            return _svc.TransportQuery( query );
        }

        /// <summary>
        /// Sets a configuration or operational value in the source. The
        /// request is passed down the chain of sources until some source
        /// recognises the control, whereupon it stores the specified value
        /// and returns.
        /// </summary>
        /// 
        /// <param name="control">an object representing a control, which could be as
        /// simple as a string, integer, or enum, or more complex such as
        /// a class with instance variables for control terms.</param>
        /// 
        /// <param name="value">the value to set.</param>
        /// 
        /// Exception: 
        ///     throws ArgumentException if the value is not the right
        ///     type or if the value is inappropriate.
        ///     
        ///     throws Exception if the control is not recognised
        ///     by any source (which is to say, if the last source in the source
        ///     chain does not recognise it, it should throw this exception).
        /// 
        public void _TransportControl( object control, object value )
        {
            _svc.TransportControl( control, value );
        }

        /// <summary>
        /// Notifies the chain of sources of the specified event. Unlike query
        /// and control operations above, events are always passed down to the
        /// bottom to allow all sources to notice them.
        /// </summary>
        /// 
        /// <param name="eventObj">a class which represents the event, possibly with
        /// parameters. The simplest event could be a string, integer,
        /// or enum, but any class instance will do (as long as some source
        /// in the chain expects it).</param>
        /// 
        public void _TransportNotify( object eventObj )
        {
            _svc.TransportNotify( eventObj );
        }

        /// <summary>
        /// Start the transport
        /// </summary>
        public void _Start()
        {
            _svc.TransportControl( TransportConsts.START, null );
        }

        /// <summary>
        /// Waits for the transport to come up.
        /// </summary>
        /// <param name="maxDelay"></param>
        public void _WaitUp( int maxDelay )
        {
            _svc.TransportQuery( new TransportConsts.WaitUp( maxDelay ) );
        }

        /// <summary>
        /// Starts the transport and waits for it to come up.
        /// </summary>
        /// <param name="maxDelay"></param>
        public void _StartAndWaitUp( int maxDelay )
        {
           _Start();
            _WaitUp(maxDelay);
        }

        /// <summary>
        /// Stops the transport.
        /// </summary>
        public void _Stop()
        {
            _svc.TransportControl( TransportConsts.STOP, null );
        }

        /// <summary>
        /// Waits for the transport to go down.
        /// </summary>
        /// <param name="maxDelay"></param>
        public void _WaitDown( int maxDelay )
        {
            _svc.TransportQuery( new TransportConsts.WaitDown( maxDelay ) );
        }

        /// <summary>
        /// Stops the transport and waits for it to go down.
        /// </summary>
        /// <param name="maxDelay">maxdelay in milliseconds</param>
        public void _StopAndWaitDown( int maxDelay )
        {
            _Stop();
            _WaitDown(maxDelay);
        }

    }
}
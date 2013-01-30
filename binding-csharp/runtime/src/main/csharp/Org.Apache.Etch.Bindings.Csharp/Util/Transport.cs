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

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// Out-of-band query, control, and notification interface for transports.
    /// </summary>
    public interface Transport<S> where S:Session 
    {
        /// <summary>
        /// Gets a configuration or operational value from the transport. The
        /// request is passed down the chain of transports until some transport
        /// recognises the query, whereupon it returns the requested value.
        /// </summary>
        /// <param name="query">an object representing a query, which could be as
        /// simple as a string, integer, or enum, or more complex such as
        /// a class with instance variables for query terms.</param>
        /// <returns>the requested value, or null if not defined.</returns>
        /// Exception:
        ///     throws Exception if the query is not recognised
        ///     by any transport (which is to say, if the last transport in the transport
        ///     chain does not recognise it, it should throw this exception). Typically
        ///     this would be some sort of transport mechanism such as tcp, udp, jms,\
        ///     http, sip, etc.
        ///     
        Object TransportQuery( Object query );

        /// <summary>
        /// Sets a configuration or operational value in the transport. The
        /// request is passed down the chain of transports until some transport
        /// recognises the control, whereupon it stores the specified value
        /// and returns.
        /// </summary>
        /// <param name="control">an object representing a control, which could be as
        /// simple as a string, integer, or enum, or more complex such as
        /// a class with instance variables for control terms.</param>
        /// <param name="value">the value to be set</param>
        /// Exception:
        ///     throws ArgumentException if the value is not the right
        ///     type or if the value is inappropriate.
        ///     
        ///     throws Exception if the query is not recognised
        ///     by any transport (which is to say, if the last transport in the transport
        ///     chain does not recognise it, it should throw this exception). Typically
        ///     this would be some sort of transport mechanism such as tcp, udp, jms,\
        ///     http, sip, etc.
        ///     
        void TransportControl( Object control, Object value );

        /// <summary>
        /// Notifies the chain of transports of the specified event. Unlike query
        /// and control operations above, events are always passed down to the
        /// bottom to allow all transports to notice them (though it is possible
        /// that an event might be delayed for a bit).
        /// </summary>
        /// <param name="eventObj">a class which represents the event, possibly with
        /// parameters. The simplest event could be a string, integer, or enum,
        /// but any class instance will do (as long as some transport in the
        /// chain expects it).</param>
        /// 
        void TransportNotify( Object eventObj );


        void SetSession(S session);

        S GetSession();

        
    }
}

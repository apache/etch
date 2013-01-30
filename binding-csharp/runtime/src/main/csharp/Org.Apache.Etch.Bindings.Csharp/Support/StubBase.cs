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
using Org.Apache.Etch.Bindings.Csharp.Transport;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{

    /// <summary>
    /// Base class of stub implementations
    /// </summary>
    public class StubBase : SessionMessage
    {
        /// <summary>Constructs the StubBase.</summary>
        /// <param name="src">the message source</param>
        /// <param name="obj">the target of decoded messages</param>
        /// <param name="queued">thread pool used to run AsyncReceiverMode.QUEUED methods.</param>
        /// <param name="free">thread pool used to run AsyncReceiverMode.FREE methods.</param>
        public StubBase(DeliveryService src, object obj, Pool queued, Pool free)
        {
            _src = src;
            _obj = obj;
            _queued = queued;
            _free = free;
            src.SetSession(this);
        }

        /// <summary>
        /// The message source.
        /// </summary>
        protected readonly DeliveryService _src;

        /// <summary>
        /// The target of decoded messages.
        /// </summary>
        protected readonly object _obj;

        /// <summary>
        /// Thread pool used to run AsyncReceiverMode.QUEUED methods.
        /// </summary>
        protected readonly Pool _queued;

        /// <summary>
        /// Thread pool used to run AsyncReceiverMode.FREE methods.
        /// </summary>
        protected readonly Pool _free;

        

        public Object SessionQuery(Object query)
        {
            if (_obj is ObjSession)
                return ((ObjSession)_obj)._SessionQuery(query);
            throw new NotSupportedException("unknown query: " + query);
        }

        public void SessionControl(Object control, Object value)
        {
            if (_obj is ObjSession)
                ((ObjSession)_obj)._SessionControl(control, value);
            else
                throw new NotSupportedException("unknown control: " + control);
        }

        public void SessionNotify(Object eventObj)
        {
            SessionNotify(_obj, eventObj);
        }

        public static void SessionNotify( object obj, object eventObj )
        {
            if (obj is ObjSession)
                ((ObjSession)obj)._SessionNotify(eventObj);
            else if (eventObj is Exception)
                Console.WriteLine(eventObj);
        }

        /// <summary>
        /// Delegate that helps the stub dispatch the received message
        /// </summary>
        /// <param name="_src"></param>
        /// <param name="_obj"></param>
        /// <param name="sender"></param>
        /// <param name="msg"></param> 
        public delegate void StubHelperRun(DeliveryService _src, Object _obj, Who sender, Message msg);


        #region SessionMessage Members

        public bool SessionMessage(Who sender, Message msg)
        {
            XType type = msg.GetXType;

            StubHelperRun helper = (StubHelperRun) type.GetStubHelper();
            if (helper == null)
                return false;

            switch (type.GetAsyncMode())
            {
                case AsyncMode.QUEUED:
                    try
                    {
                        _queued.Run(
                            delegate { helper(_src, _obj, sender, msg); },
                            delegate(Exception e) { SessionNotify(_obj, e); });
                    }
                    catch (Exception e)
                    {
                        SessionNotify(_obj, e);
                    }
                    break;
                case AsyncMode.FREE:
                    try
                    {
                        _free.Run(
                            delegate { helper(_src, _obj, sender, msg); },
                            delegate(Exception e) { SessionNotify(_obj, e); });
                    }
                    catch (Exception e)
                    {
                        SessionNotify(_obj, e);
                    }
                    break;
                case AsyncMode.NONE:
                    try
                    {
                        helper(_src, _obj, sender, msg);
                    }
                    catch (Exception e)
                    {
                        SessionNotify(_obj, e);
                    }
                    break;
                default:
                    throw new ArgumentException("unknown async mode "+type.GetAsyncMode());
            }

            return true;
        }

        #endregion
    }
}
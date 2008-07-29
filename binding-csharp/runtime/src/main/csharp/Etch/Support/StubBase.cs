// $Id$
// 
// Copyright 2007-2008 Cisco Systems Inc.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

using System;
using Etch.Msg;
using Etch.Transport;
using Etch.Util;

namespace Etch.Support
{

    /// <summary>
    /// Base class of stub implementations
    /// </summary>
    public class StubBase : SessionMessage
    {
        /// <summary>Constructs the StubBase.</summary>
        /// <param name="obj">obj</param>
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
        /// The object used to implement stub
        /// </summary>
        private readonly object _obj;

        /// <summary>
        /// The thread pool used to run AsyncReceiverMode.QUEUED methods.
        /// </summary>
        public readonly Pool _queued;

        /// <summary>
        /// The thread pool used to run AsyncReceiverMode.FREE methods.
        /// </summary>
        public readonly Pool _free;

        protected readonly DeliveryService _src;

        

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
            if (_obj is ObjSession)
                ((ObjSession)_obj)._SessionNotify(eventObj);
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

            //StubHelper<StubBase<Object>> helper = ( StubHelper<StubBase<Object>> ) type.GetStubHelper();
            //if ( helper == null )
            //    return false;
            StubHelperRun helper = (StubHelperRun)type.GetStubHelper();
            /*      StubHelper stubHelper = (StubHelper)type.GetStubHelper();
                  StubHelperRun helper = (StubHelperRun) stubHelper.GetDelegate(); */
            if (helper == null)
                return false;
            if (type.GetAsyncMode() == AsyncMode.QUEUED)
                _queued.Run(delegate() { helper(_src, _obj, sender, msg); }, delegate(Exception e)
                {
                    try { SessionNotify(e); }
                    catch (Exception e1)
                    {
                        Console.WriteLine(e1);
                    }
                });
            else if (type.GetAsyncMode() == AsyncMode.FREE)
                _free.Run(delegate()
                              {
                                  helper(_src, _obj, sender, msg);
                              },
                              delegate(Exception e)
                              {
                                  try
                                  {
                                      SessionNotify(e);
                                  }
                                  catch (Exception e1)
                                  {
                                      Console.WriteLine(e1);
                                  }
                              });
            else
                helper(_src, _obj, sender, msg);

            return true;
        }

        #endregion
    }
}
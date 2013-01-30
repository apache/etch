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
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace Org.Apache.Etch.Bindings.Csharp.Transport
{
    public class DefaultDeliveryService : DeliveryService
    {
        public DefaultDeliveryService(MailboxManager transport, string uri, Resources resources)
            : this(transport, new URL(uri), resources)
        {
        }


        public DefaultDeliveryService(MailboxManager transport, URL uri, Resources resources)
        {
            this.transport = transport;
            transport.SetSession(this);
            disableTimeout = uri.GetBooleanTerm(DISABLE_TIMEOUT, false);
        }

        
        /// <summary>
        /// Parameter to Globally Disable Timeout.
        /// </summary>
        
        public const string DISABLE_TIMEOUT = "DefaultDeliveryService.disableTimeout";
        
        
        private bool disableTimeout = false;


        private SessionMessage _stub;

        private MailboxManager transport;

        

        /// <summary>
        /// Removes all the stubs in the set of stubs
        /// </summary>
        public void RemoveStub()
        {
            _stub = null;
        }

        public Object SessionQuery(object query)
        {
            if (_stub != null)
                return _stub.SessionQuery(query);

            throw new NotSupportedException("unknown query: " + query);
        }

        public void SessionControl(Object control, Object value)
        {
            if (_stub != null)
                _stub.SessionControl(control, value);

            throw new NotSupportedException("unknown control: " + control);
        }

        public void SessionNotify(Object eventObj)
        {
            if (eventObj is string)
            {
                string stringObj = (string) eventObj;
                if (stringObj == SessionConsts.UP)
                {
                    status.Set(SessionConsts.UP);
                }
                else if (stringObj == SessionConsts.DOWN)
                {
                    status.Set(SessionConsts.DOWN);
                }
                
            }
           
                _stub.SessionNotify(eventObj);
                
  
            

        }

        private Monitor<string> status = new Monitor<string>("session status");

       
        public Object TransportQuery(Object query)
        {
            if(query.GetType() == typeof(TransportConsts.WaitUp))
            {
                waitUp(((TransportConsts.WaitUp) query)._maxDelay);
                return null;
            }
            else if (query.GetType() == typeof(TransportConsts.WaitDown))
            {
                waitDown(((TransportConsts.WaitDown)query)._maxDelay);
                return null;
            }
            else
            {
                return transport.TransportQuery(query);
            }

        }

        private void waitUp(int maxDelay)
        {
            status.WaitUntilEq(SessionConsts.UP,maxDelay);
        }

        private void waitDown(int maxDelay)
        {
            status.WaitUntilEq(SessionConsts.DOWN, maxDelay);
        }

        public void TransportControl(Object control, Object value)
        {
            if (control is string)
            {
                string stringObj = (string) control;

                if (stringObj == TransportConsts.START_AND_WAIT_UP)
                {
                    transport.TransportControl(TransportConsts.START, null);
                    waitUp((int) value);
                }
                else if (stringObj == TransportConsts.STOP_AND_WAIT_DOWN)
                {
                    transport.TransportControl(TransportConsts.STOP,null);
                    waitDown((int)value);
                }
                else
                {
                    transport.TransportControl(control, value);
                }       
            }
            else
            {
                transport.TransportControl(control, value); 
            }
           
        }

        public void TransportNotify(Object eventObj)
        {
            transport.TransportNotify(eventObj);
        }

       

        public Mailbox BeginCall(Message msg)
        {
            return transport.TransportCall(null, msg);
        }

        public Object EndCall(Mailbox mb, XType responseType)
        {
            try
            {
                int timeout = disableTimeout ? 0 : responseType.Timeout;
                Element mbe = mb.Read(timeout);
                if (mbe == null)
                    throw new TimeoutException("timeout waiting for " + responseType);
                Message rmsg = mbe.msg;
                rmsg.CheckType(responseType);
                Object r = rmsg.Get(responseType.ResponseField);
                if (r is Exception)
                {
                    Exception e = (Exception)r;
                    throw e;
                }
                return r;
            }
            finally
            {
                mb.CloseRead();
            }
        }

        public override string ToString()
        {
            return transport.ToString();
        }

    

     
        #region SessionMessage Members

        public bool SessionMessage(Who sender, Message msg)
        {
           return  _stub.SessionMessage(sender, msg);
        }

        /// <summary>
        /// Adds a stub to a list of stub
        /// </summary>
        /// <param name="stb"></param>
        public void SetSession(SessionMessage stb)
        {
            if (_stub != null)
                throw new Exception("Unsupported -- only one stub for now");
            _stub = stb;
        }

        #endregion

        #region TransportMessage Members

        public void TransportMessage(Who recipient, Message msg)
        {
            transport.TransportMessage(recipient,msg);
        }

        #endregion

        #region Transport<SessionMessage> Members


        public SessionMessage GetSession()
        {
            return _stub;
        }

        #endregion
    }
}

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
    abstract public class DefaultServerFactory : ServerFactory
    {
        protected DefaultServerFactory(Transport<ServerFactory> listener, object implFactory)
        {
            this.listener = listener;
            if (implFactory is Session)
                SetSession((Session)implFactory);
            listener.SetSession(this);
        }

        private readonly Transport<ServerFactory> listener;

        public Object SessionQuery(Object query)
        {
            if (session != null)
                return session.SessionQuery(query);

            throw new NotSupportedException("unknown query " + query);
        }

        public void SessionControl(Object control, Object value)
        {
            if (session != null)
            {
                session.SessionControl(control, value);
                return;
            }

            throw new NotSupportedException("unknown control " + control);
        }

        public void SessionNotify(Object evnt)
        {
            if (session != null)
                session.SessionNotify(evnt);
        }

        #region Transport Members

        public Session GetSession()
        {
            return session;
        }

        public void SetSession(Session session)
        {
            this.session = session;
        }

        private Session session;

        public object TransportQuery(object query)
        {
            return listener.TransportQuery(query);
        }

        public void TransportControl(object control, object value)
        {
            listener.TransportControl(control, value);
        }

        public void TransportNotify(object eventObj)
        {
            listener.TransportNotify(eventObj);
        }

        #endregion

        #region ServerFactory Members

        abstract public void NewServer(TransportMessage transport, string uri, Resources resources);

        abstract public ValueFactory NewValueFactory(string uri);

        #endregion
    }
}

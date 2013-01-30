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
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace Org.Apache.Etch.Bindings.Csharp.Transport.Filter
{
    abstract public class AbstractMessageFilter : TransportMessage, SessionMessage
    {
        protected AbstractMessageFilter(TransportMessage transport, URL uri, Resources resources)
        {
            this.transport = transport;
            transport.SetSession(this);
        }

        /// <summary>
        /// Our transport.
        /// </summary>
        protected readonly TransportMessage transport;

        /// <summary>
        /// Returns a string such as "Filter/"+transport.
        /// </summary>
        /// <returns>a string such as "Filter/"+transport.</returns>
        abstract public override string ToString();

        public virtual void TransportMessage(Who recipient, Message msg)
        {
            transport.TransportMessage(recipient, msg);
        }

        public SessionMessage GetSession()
        {
            return session;
        }

        public void SetSession(SessionMessage session)
        {
            this.session = session;
        }

        /// <summary>
        /// Our session.
        /// </summary>
        protected SessionMessage session;

        public virtual object TransportQuery(object query)
        {
            return transport.TransportQuery(query);
        }

        public virtual void TransportControl(object control, object value)
        {
            transport.TransportControl(control, value);
        }

        public void TransportNotify(object evnt)
        {
            transport.TransportNotify(evnt);
        }

        public virtual bool SessionMessage(Who sender, Message msg)
        {
            return session.SessionMessage(sender, msg);
        }

        public object SessionQuery(object query)
        {
            return session.SessionQuery(query);
        }

        public void SessionControl(object control, object value)
        {
            session.SessionControl(control, value);
        }

        public void SessionNotify(object evnt)
        {
            if (evnt == (object) SessionConsts.UP)
                if (!SessionUp())
                    return;

            if (evnt == (object) SessionConsts.DOWN)
                if (!SessionDown())
                    return;

            session.SessionNotify(evnt);
        }

        /**
         * Tells the filter that the session is up.
         * @return true if the event should be passed on to our session, false
         * if it should be swallowed.
         * @throws Exception 
         */
        protected virtual bool SessionUp()
        {
            // nothing to do.
            return true;
        }

        /**
         * Tells the filter that the session is down.
         * @return true if the event should be passed on to our session, false
         * if it should be swallowed.
         * @throws Exception 
         */
        protected virtual bool SessionDown()
        {
            // nothing to do.
            return true;
        }
    }
}

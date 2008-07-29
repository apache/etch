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
using Etch.Util;

namespace Etch.Support
{
    abstract public class DefaultServerFactory : ServerFactory
    {
        public DefaultServerFactory(object implFactory)
        {
            this.implFactory = implFactory;
        }

        private readonly object implFactory;

        public Object SessionQuery(Object query)
        {
            if (implFactory is Session)
                return ((Session)implFactory).SessionQuery(query);

            throw new NotSupportedException("unknown query " + query);
        }

        public void SessionControl(Object control, Object value)
        {
            if (implFactory is Session)
            {
                ((Session)implFactory).SessionControl(control, value);
                return;
            }

            throw new NotSupportedException("unknown control " + control);
        }

        public void SessionNotify(Object evnt)
        {
            if (implFactory is Session)
                ((Session)implFactory).SessionNotify(evnt);
            else if (evnt is Exception)
                throw new Exception("caught exception", (Exception)evnt);
        }

        #region ServerFactory Members

        abstract public void NewServer(Etch.Support.DeliveryService d, Etch.Msg.ValueFactory vf);

        abstract public Etch.Msg.ValueFactory NewValueFactory();

        #endregion
    }
}

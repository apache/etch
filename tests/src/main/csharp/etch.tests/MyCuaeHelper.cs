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
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Transport;
using Org.Apache.Etch.Bindings.Csharp.Util;
using org.apache.etch.tests;

namespace etch.tests
{
    abstract public class MyCuaeHelper : CuaeHelper
    {
        public static ServerFactory NewListener(string uri,
            Resources resources, MyCuaeServerFactory implFactory)
        {
            Resources res = InitResources(resources);
            Transport<ServerFactory> listener = TransportFactory.GetListener(uri, res);
            return new MyServerFactory(listener, implFactory);
        }

        public new class MyServerFactory : DefaultServerFactory
        {
            public MyServerFactory(Transport<ServerFactory> listener, MyCuaeServerFactory implFactory)
                : base(listener, implFactory)
            {
                _listener = listener;
                _implFactory = implFactory;
            }

            private readonly Transport<ServerFactory> _listener;

            private readonly MyCuaeServerFactory _implFactory;

            public override void NewServer(TransportMessage m, string uri, Resources resources)
            {
                ValueFactory vf = (ValueFactory)resources.Get(TransportConsts.VALUE_FACTORY);
                MailboxManager x = new PlainMailboxManager(m, uri, resources);
                DeliveryService d = new DefaultDeliveryService(x, uri, resources);
                Pool qp = (Pool)resources[QUEUED_POOL];
                Pool fp = (Pool)resources[FREE_POOL];
                _implFactory.NewMyCuaeServer(d, qp, fp, (MyValueFactoryCuae)vf);
                d.TransportControl(TransportConsts.START, null);
            }

            public override ValueFactory NewValueFactory(string uri)
            {
                return new ValueFactoryCuae(uri);
            }

            public override string ToString()
            {
                return "MyCuaeHelper.ServerFactory/" + _listener;
            }
        }

        public interface MyCuaeServerFactory
        {
            SessionMessage NewMyCuaeServer(DeliveryService svc, Pool qp, Pool fp,
                MyValueFactoryCuae vf);
        }
    }
}

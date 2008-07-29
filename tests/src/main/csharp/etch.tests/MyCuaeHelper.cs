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
using System.Collections.Generic;
using System.Text;
using Etch.Support;
using etch.tests;
using Etch.Transport;
using Etch.Msg;
using Etch.Util;

namespace etch.tests
{
    abstract public class MyCuaeHelper : CuaeHelper
    {
        public static Transport<ServerFactory> NewListener(string uri,
            Resources resources, MyCuaeServerFactory implFactory)
        {
            Resources res = InitResources(resources);
            return TransportFactory.GetListener(uri, res, new MyServerFactory(uri, res, implFactory));
        }

        public new class MyServerFactory : DefaultServerFactory
        {
            private string _uri;
            private Resources _resources;
            private MyCuaeServerFactory _implFactory;

            public MyServerFactory(string uri, Resources resources, MyCuaeServerFactory implFactory)
                : base(implFactory)
            {
                _uri = uri;
                _resources = resources;
                _implFactory = implFactory;
            }

            public override void NewServer(DeliveryService d, ValueFactory vf)
            {
                Pool qp = (Pool)_resources[QUEUED_POOL];
                Pool fp = (Pool)_resources[FREE_POOL];
                _implFactory.NewMyCuaeServer(d, qp, fp, (MyValueFactoryCuae)vf);
            }

            public override ValueFactory NewValueFactory()
            {
                return new ValueFactoryCuae(_uri);
            }
        }

        public interface MyCuaeServerFactory
        {
            SessionMessage NewMyCuaeServer(DeliveryService svc, Pool qp, Pool fp,
                MyValueFactoryCuae vf);
        }
    }
}

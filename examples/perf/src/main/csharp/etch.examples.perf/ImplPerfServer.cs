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

namespace org.apache.etch.examples.perf
{
    public class ImplPerfServer : BasePerfServer
    {
        public ImplPerfServer(RemotePerfClient client)
        {
            this.client = client;
        }

#pragma warning disable 219
        private readonly RemotePerfClient client;
#pragma warning restore 219

        #region PerfServer Members

        public override int? add(int? x, int? y)
        {
            return x + y;
        }

        public override int? sum(int[] values)
        {
            int sum = 0;
            foreach (int value in values)
                sum += value;
            return sum;
        }

        public override void report(int? code, string msg)
        {
            // do nothing
        }

        public override types.Perf.Point dist(types.Perf.Point a, types.Perf.Point b)
        {
            return new types.Perf.Point(b.x - a.x, b.y - a.y);
        }

        public override DateTime? add2(DateTime? ts, long? ms)
        {
            return ts.Value + new TimeSpan(ms.Value*10000);
        }

        public override void report2(DateTime? ts, int? code, string msg)
        {
            // do nothing.
        }

        #endregion
    }
}

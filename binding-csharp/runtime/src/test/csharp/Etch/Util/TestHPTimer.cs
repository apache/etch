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
using System.Threading;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    [TestFixture]
    public class TestHPTimer
    {
        [Test]
        public void one()
        {
            System.Console.WriteLine("one starting");

            long x = HPTimer.Now();
            for (int i = 0; i < 10000; i++)
            {
                // assert that values from HPTimer.Now() are always monotonically increasing
                long y = HPTimer.Now();
                Assert.GreaterOrEqual(y, x);
                long diff = y - x;
                Assert.GreaterOrEqual(y, 0);
                int idiff = (int) diff;
                Assert.GreaterOrEqual(idiff, 0);
                x = y;
                Thread.Sleep(1);
            }

            System.Console.WriteLine("one done");
        }

        [Test]
        public void two()
        {
            System.Console.WriteLine("two starting");

            Thread t1 = new Thread(foo);
            Thread t2 = new Thread(foo);

            t1.Start();
            t2.Start();

            t1.Join();
            t2.Join();

            System.Console.WriteLine("two done");
        }

        public void foo()
        {
            long x = HPTimer.Now();
            for (int i = 0; i < 10000; i++)
            {
                // assert that values from HPTimer.Now() are always monotonically increasing
                long y = HPTimer.Now();
                Assert.GreaterOrEqual(y, x);
                long diff = y - x;
                Assert.GreaterOrEqual(y, 0);
                int idiff = (int)diff;
                Assert.GreaterOrEqual(idiff, 0);
                x = y;
                Thread.Sleep(1);
            }
        }
    }
}

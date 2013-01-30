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
using System.Threading;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    [TestFixture]
    public class TestFreePool
    {
        private const int Q1 = 30; // 1 quanta of reliable clock tick
        private const int Q2 = 60; // 2 quanta of reliable clock tick
        private const int Q3 = 90; // 3 quanta of reliable clock tick

        [Test]
        public void close1()
        {
            FreePool p = new FreePool(2);
            p.Close();
        }

        [Test]
        [ExpectedException(typeof(Exception))]
        public void close2()
        {
            // free pool thread count exceeded or pool closed
            FreePool p = new FreePool(2);
            p.Close();

            MyPoolRunnable r = new MyPoolRunnable(0, false);
            p.Run(r.run, r.exception);
        }

        [Test]
        public void close3()
        {
            FreePool p = new FreePool(2);
            p.Close();
            p.Close();
        }

        [Test]
        [ExpectedException(typeof(Exception))]
        public void join1()
        {
            // free pool thread count exceeded or pool closed
            FreePool p = new FreePool(2);
            p.Join();

            MyPoolRunnable r = new MyPoolRunnable(0, false);
            p.Run(r.run, r.exception);
        }

        [Test]
        public void join2()
        {
            FreePool p = new FreePool(2);

            MyPoolRunnable r = new MyPoolRunnable(0, false);
            Assert.IsFalse(r.done);
            Assert.IsNull(r.ex);
            p.Run(r.run, r.exception);
            Thread.Sleep(Q2);
            Assert.IsTrue(r.done);
            Assert.IsNull(r.ex);
            p.Join();
        }

        [Test]
        public void join3()
        {
            FreePool p = new FreePool(2);

            MyPoolRunnable r = new MyPoolRunnable(Q1, false);
            Assert.IsFalse(r.done);
            Assert.IsNull(r.ex);
            p.Run(r.run, r.exception);
            Assert.IsFalse(r.done);
            Assert.IsNull(r.ex);
            p.Join();
            Assert.IsTrue(r.done);
            Assert.IsNull(r.ex);
        }

        [Test]
        public void run1()
        {
            FreePool p = new FreePool(2);

            MyPoolRunnable r = new MyPoolRunnable(0, false);
            Assert.IsFalse(r.done);
            Assert.IsNull(r.ex);

            p.Run(r.run, r.exception);

            Thread.Sleep(Q1);
            Assert.IsTrue(r.done);
            Assert.IsNull(r.ex);
        }

        [Test]
        public void run2()
        {
            FreePool p = new FreePool(2);
            for (int i = 0; i < 100; i++)
            {
                MyPoolRunnable r = new MyPoolRunnable(0, false);
                Assert.IsFalse(r.done);
                Assert.IsNull(r.ex);

                p.Run(r.run, r.exception);

                Thread.Sleep(Q1);
                Assert.IsTrue(r.done);
                Assert.IsNull(r.ex);
            }
        }

        [Test]
        public void run3()
        {
            FreePool p = new FreePool(2);

            MyPoolRunnable r = new MyPoolRunnable(0, true);
            Assert.IsFalse(r.done);
            Assert.IsNull(r.ex);

            p.Run(r.run, r.exception);

            Thread.Sleep(Q1);
            Assert.IsFalse(r.done);
            Assert.IsNotNull(r.ex);
        }

        [Test]
        public void run4()
        {
            FreePool p = new FreePool(2);

            MyPoolRunnable r = new MyPoolRunnable(Q2, false);
            Assert.IsFalse(r.done);
            Assert.IsNull(r.ex);

            p.Run(r.run, r.exception);

            Thread.Sleep(Q1);
            Assert.IsFalse(r.done);
            Assert.IsNull(r.ex);

            Thread.Sleep(Q3);
            Assert.IsTrue(r.done);
            Assert.IsNull(r.ex);
        }

        [Test]
        public void run5()
        {
            FreePool p = new FreePool(2);

            MyPoolRunnable r1 = new MyPoolRunnable(Q2, false);
            Assert.IsFalse(r1.done);
            Assert.IsNull(r1.ex);

            MyPoolRunnable r2 = new MyPoolRunnable(Q2, false);
            Assert.IsFalse(r2.done);
            Assert.IsNull(r2.ex);

            p.Run(r1.run, r1.exception);
            p.Run(r2.run, r2.exception);

            Thread.Sleep(Q1);
            Assert.IsFalse(r1.done);
            Assert.IsNull(r1.ex);

            Assert.IsFalse(r2.done);
            Assert.IsNull(r2.ex);

            Thread.Sleep(Q3);
            Assert.IsTrue(r1.done);
            Assert.IsNull(r1.ex);

            Assert.IsTrue(r2.done);
            Assert.IsNull(r2.ex);
        }

        [Test]
        [ExpectedException(typeof(Exception))]
        public void run6()
        {
            // free pool thread count exceeded
            FreePool p = new FreePool(2);

            MyPoolRunnable r1 = new MyPoolRunnable(Q2, false);
            Assert.IsFalse(r1.done);
            Assert.IsNull(r1.ex);

            MyPoolRunnable r2 = new MyPoolRunnable(Q2, false);
            Assert.IsFalse(r2.done);
            Assert.IsNull(r2.ex);

            MyPoolRunnable r3 = new MyPoolRunnable(Q2, false);
            Assert.IsFalse(r3.done);
            Assert.IsNull(r3.ex);

            p.Run(r1.run, r1.exception);
            p.Run(r2.run, r2.exception);
            p.Run(r3.run, r3.exception);
        }

        [Test]
        public void run7()
        {
            FreePool p = new FreePool(2);

            MyPoolRunnable r1 = new MyPoolRunnable(Q2, false);
            Assert.IsFalse(r1.done);
            Assert.IsNull(r1.ex);

            MyPoolRunnable r2 = new MyPoolRunnable(Q2, false);
            Assert.IsFalse(r2.done);
            Assert.IsNull(r2.ex);

            MyPoolRunnable r3 = new MyPoolRunnable(Q2, false);
            Assert.IsFalse(r3.done);
            Assert.IsNull(r3.ex);

            p.Run(r1.run, r1.exception);
            p.Run(r2.run, r2.exception);
            try { p.Run(r3.run, r3.exception); } catch (Exception) { }

            Thread.Sleep(Q1);
            Assert.IsFalse(r1.done);
            Assert.IsNull(r1.ex);

            Assert.IsFalse(r2.done);
            Assert.IsNull(r2.ex);

            Assert.IsFalse(r3.done);
            Assert.IsNull(r3.ex);

            Thread.Sleep(Q3);
            Assert.IsTrue(r1.done);
            Assert.IsNull(r1.ex);

            Assert.IsTrue(r2.done);
            Assert.IsNull(r2.ex);

            Assert.IsFalse(r3.done);
            Assert.IsNull(r3.ex);
        }
    }

    class MyPoolRunnable
    {
        public MyPoolRunnable( int delay, bool excp )
        {
            this.delay = delay;
            this.excp = excp;
        }

        private readonly int delay;

        private readonly bool excp;

        public bool done;

        public Exception ex;

        public void run()
        {
            if (delay > 0)
                Thread.Sleep(delay);
            if (excp)
                throw new Exception();
            done = true;
        }

        public void exception( Exception e )
        {
            ex = e;
        }
    }
}

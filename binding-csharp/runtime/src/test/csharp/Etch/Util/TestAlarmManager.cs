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

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    [TestFixture]
    public class TestAlarmManager
    {
        private const int Q1 = 50;

        private const int Q2 = 100;

        private const int Q3 = 150;

        private readonly Object s1 = new Object();

        private readonly Object s2 = new Object();

        [Test]
        public void start1()
        {
            AlarmManager am = new AlarmManager();
            Assert.IsFalse(am.IsStarted());
            am.Start();
            Assert.IsTrue(am.IsStarted());
        }

        [Test]
        [ExpectedException(typeof(Exception))]
        public void start2()
        {
            // already started
            AlarmManager am = new AlarmManager();
            Assert.IsFalse(am.IsStarted());
            am.Start();
            Assert.IsTrue(am.IsStarted());
            am.Start();
        }

        [Test]
        [ExpectedException(typeof(Exception))]
        public void stop1()
        {
            // is not started
            AlarmManager am = new AlarmManager();
            Assert.IsFalse(am.IsStarted());
            am.Stop();
        }

        [Test]
        public void stop2()
        {
            AlarmManager am = new AlarmManager();
            Assert.IsFalse(am.IsStarted());
            am.Start();
            Assert.IsTrue(am.IsStarted());
            am.Stop();
            Assert.IsFalse(am.IsStarted());
        }

        [Test]
        public void static1()
        {
            AlarmManager.shutdown();

            Assert.IsNull(AlarmManager.GetAlarmManager(false));
            Assert.IsNull(AlarmManager.GetAlarmManager(false));

            AlarmManager am1 = AlarmManager.GetAlarmManager(true);
            Assert.IsNotNull(am1);
            Assert.IsTrue(am1.IsStarted());

            AlarmManager am2 = AlarmManager.GetAlarmManager(true);
            Assert.AreSame(am2, am1);
            Assert.IsTrue(am1.IsStarted());

            AlarmManager.shutdown();
            Assert.IsFalse(am1.IsStarted());
            Assert.IsNull(AlarmManager.GetAlarmManager(false));

            AlarmManager.shutdown();
        }

        [Test]
        public void listener1()
        {
            MyAlarmListener listener = new MyAlarmListener(0);

            Assert.AreEqual(0, listener.delay);
            Assert.IsFalse(listener.wake.Get());
            Assert.IsNull(listener.state);

            listener.Wakeup(null, null, 12345);
            Assert.AreEqual(0, listener.delay);
            Assert.IsTrue(listener.wake.Get());
            Assert.IsNull(listener.state);

            listener.Wakeup(null, s1, 23456);
            Assert.AreEqual(0, listener.delay);
            Assert.IsTrue(listener.wake.Get());
            Assert.AreSame(s1, listener.state);

            MyAlarmListener listener1 = new MyAlarmListener(1);
            Assert.AreEqual(1, listener1.delay);
        }

        [Test]
        [ExpectedException(typeof(Exception))]
        public void add1()
        {
            // is not started
            AlarmManager am = new AlarmManager();

            MyAlarmListener listener = new MyAlarmListener(0);

            am.Add(listener, null, Q1);
        }

        [Test]
        [ExpectedException(typeof(ArgumentNullException))]
        public void add2()
        {
            // listener == null
            AlarmManager am = new AlarmManager();
            am.Start();

            am.Add(null, null, Q1);
        }

        [Test]
        [ExpectedException(typeof(ArgumentException))]
        public void add3()
        {
            // delay <= 0
            AlarmManager am = new AlarmManager();
            am.Start();

            MyAlarmListener listener = new MyAlarmListener(0);

            am.Add(listener, null, -1);
        }

        [Test]
        public void add4()
        {
            AlarmManager am = new AlarmManager();
            am.Start();

            MyAlarmListener listener = new MyAlarmListener(0);

            //Log("Adding");
            am.Add(listener, null, Q1);
            //Log("Added");
            Assert.IsFalse(listener.wake.Get());
            Assert.IsNull(listener.state);

            Thread.Sleep(Q2);
            Assert.IsTrue(listener.wake.Get());
            Assert.IsNull(listener.state);
        }

        [Test]
        public void add5()
        {
            AlarmManager am = new AlarmManager();
            am.Start();

            MyAlarmListener listener = new MyAlarmListener(0);

            am.Add(listener, s1, Q1);
            Assert.IsFalse(listener.wake.Get());
            Assert.IsNull(listener.state);

            Thread.Sleep(Q2);
            Assert.IsTrue(listener.wake.Get());
            Assert.AreSame(s1, listener.state);
        }

        [Test]
        public void add6()
        {
            AlarmManager am = new AlarmManager();
            am.Start();

            MyAlarmListener listener = new MyAlarmListener(0);

            am.Add(listener, s1, Q3);
            Assert.IsFalse(listener.wake.Get());
            Assert.IsNull(listener.state);

            am.Add(listener, s2, Q1);
            Assert.IsFalse(listener.wake.Get());
            Assert.IsNull(listener.state);

            Thread.Sleep(Q2);
            Assert.IsTrue(listener.wake.Get());
            Assert.AreSame(s2, listener.state);
        }

        [Test]
        public void add7()
        {
            AlarmManager am = new AlarmManager();
            am.Start();

            MyAlarmListener listener = new MyAlarmListener(0);

            listener.excp = true;
            am.Add(listener, s1, Q1);
            Assert.IsFalse(listener.wake.Get());
            Assert.IsNull(listener.state);

            Thread.Sleep(Q2);
            Assert.IsTrue(listener.wake.Get());
            Assert.AreSame(s1, listener.state);
        }

        [Test]
        [ExpectedException(typeof(Exception))]
        public void remove1()
        {
            // is not started
            AlarmManager am = new AlarmManager();

            MyAlarmListener listener = new MyAlarmListener(0);

            am.Remove(listener);
        }

        [Test]
        public void remove2()
        {
            AlarmManager am = new AlarmManager();
            am.Start();

            MyAlarmListener listener = new MyAlarmListener(0);

            am.Remove(listener);
        }

        [Test]
        public void remove3()
        {
            AlarmManager am = new AlarmManager();
            am.Start();

            MyAlarmListener listener = new MyAlarmListener(0);

            am.Add(listener, null, Q1);
            Assert.IsFalse(listener.wake.Get());
            Assert.IsNull(listener.state);

            am.Remove(listener);
            Assert.IsFalse(listener.wake.Get());
            Assert.IsNull(listener.state);

            Thread.Sleep(Q2);
            Assert.IsFalse(listener.wake.Get());
            Assert.IsNull(listener.state);
        }

        [Test]
        public void repeat1()
        {
            AlarmManager am = new AlarmManager();
            am.Start();

            MyAlarmListener listener = new MyAlarmListener(0);

            listener.delay = Q2;
            am.Add(listener, null, Q2);

            listener.wake.WaitUntilEqAndSet(true, Q3, false);
            Console.WriteLine("due0 = {0}", listener.due);
            long t0 = listener.due;

            listener.wake.WaitUntilEqAndSet(true, Q3, false);
            Console.WriteLine("due1 = {0}", listener.due);
            long d1 = listener.due - t0;

            listener.wake.WaitUntilEqAndSet(true, Q3, false);
            Console.WriteLine("due2 = {0}", listener.due);
            long d2 = listener.due - t0;

            listener.wake.WaitUntilEqAndSet(true, Q3, false);
            Console.WriteLine("due3 = {0}", listener.due);
            long d3 = listener.due - t0;

            Thread.Sleep(Q1);
            listener.delay = 0;

            listener.wake.WaitUntilEqAndSet(true, Q3, false);
            Console.WriteLine("due4 = {0}", listener.due);
            long d4 = listener.due - t0;

            long m1 = (long) Math.Round(d1 / (double)d1);
            long m2 = (long) Math.Round(d2 / (double)d1);
            long m3 = (long) Math.Round(d3 / (double)d1);
            long m4 = (long) Math.Round(d4 / (double)d1);

            Console.WriteLine("times {0} {1} {2} {3} {4}", t0, d1, d2, d3, d4);

            Assert.AreEqual(1, m1);
            Assert.AreEqual(2, m2);
            Assert.AreEqual(3, m3);
            Assert.AreEqual(4, m4);
        }

        [Test]
        public void repeat2()
        {
            AlarmManager am = new AlarmManager();
            am.Start();

            MyAlarmListener listener = new MyAlarmListener(0);

            listener.delay = -Q2;
            am.Add(listener, null, Q2);

            listener.wake.WaitUntilEqAndSet(true, Q3, false);
            Console.WriteLine("due0 = {0}", listener.due);
            long t0 = listener.due;

            listener.wake.WaitUntilEqAndSet(true, Q3, false);
            Console.WriteLine("due1 = {0}", listener.due);
            long d1 = listener.due - t0;

            listener.wake.WaitUntilEqAndSet(true, Q3, false);
            Console.WriteLine("due2 = {0}", listener.due);
            long d2 = listener.due - t0;

            listener.wake.WaitUntilEqAndSet(true, Q3, false);
            Console.WriteLine("due3 = {0}", listener.due);
            long d3 = listener.due - t0;

            Thread.Sleep(Q1);
            listener.delay = 0;

            listener.wake.WaitUntilEqAndSet(true, Q3, false);
            Console.WriteLine("due4 = {0}", listener.due);
            long d4 = listener.due - t0;

            long m1 = (long) Math.Round(d1 / (double)d1);
            long m2 = (long) Math.Round(d2 / (double)d1);
            long m3 = (long) Math.Round(d3 / (double)d1);
            long m4 = (long) Math.Round(d4 / (double)d1);

            Console.WriteLine("times {0} {1} {2} {3} {4}", t0, d1, d2, d3, d4);

            Assert.AreEqual(1, m1);
            Assert.AreEqual(2, m2);
            Assert.AreEqual(3, m3);
            Assert.AreEqual(4, m4);
        }

        [Test]
        [ExpectedException(typeof(ThreadInterruptedException))]
        public void repeat3()
        {
            // timeout
            AlarmManager am = new AlarmManager();
            am.Start();

            MyAlarmListener listener = new MyAlarmListener(0);

            listener.delay = Q2;
            am.Add(listener, null, Q2);

            listener.wake.WaitUntilEqAndSet(true, Q3, false);

            listener.wake.WaitUntilEqAndSet(true, Q3, false);

            listener.wake.WaitUntilEqAndSet(true, Q3, false);

            Thread.Sleep(Q1);
            listener.delay = 0;

            listener.wake.WaitUntilEqAndSet(true, Q3, false);
            // alarm is now canceled

            // this should timeout...
            listener.wake.WaitUntilEqAndSet(true, Q3, false);
        }
    }

    class MyAlarmListener : AlarmListener
    {
        public MyAlarmListener(int delay)
        {
            this.delay = delay;
        }

        public int delay;

        public bool excp;

        public Monitor<bool> wake = new Monitor<bool>("wake", false);

        public object state;

        public long due;

        public int Wakeup(AlarmManager manager, object state, long due)
        {
            this.state = state;
            this.due = due;
            wake.Set(true);

            if (excp)
                throw new Exception();

            return delay;
        }
    }
}

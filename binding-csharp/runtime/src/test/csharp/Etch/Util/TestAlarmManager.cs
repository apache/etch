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
using System.Threading;
using NUnit.Framework;

namespace Etch.Util
{
    [TestFixture]
    public class TestAlarmManager
    {
        private const int Q1 = 30;
        private const int Q2 = 60;

        [Test]
        public void start1()
        {
        }

        [Test]
        public void start2()
        {
        }

        [Test]
        public void start3()
        {
        }
    }

    class MyAlarmListener: AlarmListener
    {
        public MyAlarmListener( int delay )
        {
            this.delay = delay;
        }

        public int delay;

        public bool wake;

        public object state;

        public int Wakeup(AlarmManager manager, object state, long due)
        {
            wake = true;
            this.state = state;
            return delay;
        }
    }
}

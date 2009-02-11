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
using NUnit.Framework;
using org.apache.etch.tests;

namespace etch.tests
{
    public class TestMainTest1Listener
    {
        [Test]
        public void NewTestServerMethod()
        {
            MainTest1Listener m = new MainTest1Listener();
            Object o = m.NewTest1Server( null );
            Assert.IsNotNull( o );
            Assert.IsTrue( o is ImplTest1Server );
        }

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine( "TestListener" );
        }

        private class ListenerThread
        {
            private Boolean myHasErrors;

            public ListenerThread()
            {
                myHasErrors = false;
            }

            public void Run()
            {
                try
                {
    //                MainTest1Listener.Main( null );
                }
                catch ( Exception e )
                {
                    Console.WriteLine( e );
                    myHasErrors = false;
                }
            }

            public Boolean HasErrors()
            {
                return myHasErrors;
            }
        }
    }
}

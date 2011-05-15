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
using Org.Apache.Etch.Bindings.Csharp.Util;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    [TestFixture]
    public class TestTodoManager
    {

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestTodoManager" );
        }

        [SetUp]
        public void Setup()
        {
            MyTodo.ResetGlobalId();
        }

        [Test]
        [ExpectedException(typeof(Exception))]
        public void TestManagerNotStarted()
        {
            TodoManager mgr = new TodoManager( 50, 10, 1, 5, 5000, 1 );
            mgr.Add( new MyTodo() );
        }

        [Test]
        [ExpectedException( typeof( Exception ) )]
        public void TestShutDown()
        {
            TodoManager mgr = TodoManager.GetTodoManager();
            mgr.Add( new MyTodo() );

            TodoManager.ShutDown();
            
            // adding after shutting down should cause exception
            mgr.Add( new MyTodo() );
        }

        [Test]
        public void TestWorkersAndEntries()
        {
            TodoManager mgr = new TodoManager( 50, 10, 1, 5, 5000, 1 );
            mgr.Start();

            Assert.AreEqual( 0, mgr.NumEntries() );
            Assert.AreEqual( 0, mgr.NumWorkers() );
            
            mgr.Add( new MyTodo() );

            Assert.AreEqual( 1, mgr.NumEntries() );
            Assert.AreEqual( 1, mgr.NumWorkers() );
            
            // experiment with a total of 50 entries.
            for ( int i = 0; i < 49; i++ )
            {
                mgr.Add( new MyTodo() );
            }

            Assert.AreEqual( 50, MyTodo.LastIdGenerated() );
            
            WaitTillComplete( 4000 );
            //Assert.AreEqual( 5, mgr.NumWorkers() );

            
            // test if all todo's were executed. 
            Assert.AreEqual( 0, mgr.NumEntries() );

            TodoManager.ShutDown(mgr);
        }

        //[Test]
        public void TestSeparateThreads()
        {
            TodoManager mgr = TodoManager.GetTodoManager();
            TodoManager.GetTodoManager().Add( new MyTodo() );

            // Start another thread to add items to the todomanager. 
            ( new Thread( new ThreadStart( AddUsingDifferentThread ) ) ).Start();

            // simple "lock"
            while ( simpleLock != true )
                ;

            WaitTillComplete( 4000 );

            TodoManager.ShutDown();
        }

        // Thread that adds independently to the queue. 
        public void AddUsingDifferentThread()
        {
            TodoManager.GetTodoManager().Add( new MyTodo() );
            
            simpleLock = true;
        }
        private bool simpleLock = false;

        /// <summary>
        /// Wait until all Todos are run. 
        /// </summary>
        /// <param name="maxWait">Max time in milliseconds 
        /// to wait</param>
        private void WaitTillComplete( long maxWait )
        {
            DateTime now = DateTime.Now;

            long nowTime = 0;
            long endTime = nowTime + maxWait;

            TimeSpan span;
            long spanTime = 0;

            while ( true && ( nowTime < endTime ) )
            {
                if ( MyTodo.TodosRemaining() == 0 )
                    return;

                span = DateTime.Now - now;
                now = DateTime.Now;
                spanTime = (long) span.TotalMilliseconds;
                nowTime += spanTime;
            }

            throw new Exception( "Time exceeded" );
        }

        /// <summary>
        /// "Todo" tasks put inside the TodoManager queue. 
        /// </summary>
        public class MyTodo : Todo
        {
            /// <summary>
            /// unique id for every "todo" created
            /// </summary>
            private static int globalId = 0;
            private static int lastRun = 0;
            private int uniqueId = 0;

            public MyTodo()
            {
                globalId++;
                uniqueId = globalId;
                lastRun++;
            }

            public static int LastIdGenerated()
            {
                return globalId;
            }

            /// <summary>
            /// Reset the global id.
            /// </summary>
            public static void ResetGlobalId()
            {
                globalId = 0;
                lastRun = 0;
            }

            /// <summary>
            /// How many todos remain to be run
            /// </summary>
            /// <returns>number of todos remaining</returns>
            public static int TodosRemaining()
            {
                return lastRun;
            }

            public void Doit( TodoManager mgr )
            {
                lastRun--;
            }

            public void Exception( TodoManager mgr, Exception e )
            {
                Console.WriteLine( "exception thrown by todo " + uniqueId );
                Console.WriteLine( e.Message );
                Console.WriteLine( e );
            }
        }
    }
}

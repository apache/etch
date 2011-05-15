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
using System.Runtime.CompilerServices;
using System.Threading;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// A standalone version of a processor for todo items
    /// </summary>
    public class TodoManager : AbstractStartable
    {
        /// <summary>
        /// Constructs the TodoManager
        /// </summary>
        /// <param name="maxEntries">the maximum number of entries in the queue</param>
        /// <param name="entryDelay">milliseconds to delay a caller who tries to
        /// add a entry over the limit.</param>
        /// <param name="minWorkers">the minimum number of workers to keep waiting</param>
        /// <param name="maxWorkers">the maximum number of workers to allow.</param>
        /// <param name="workerLinger">milliseconds a worker will wait for a Todo
        /// before considering quitting.</param>
        /// <param name="threshold">the per worker threshold for queue length. if
        /// queue length exceeds this amount, a new worker is added if allowed.</param>
        /// 
        public TodoManager( int maxEntries, int entryDelay, int minWorkers,
            int maxWorkers, int workerLinger, int threshold )
        {
            if ( maxEntries < 1 )
                throw new ArgumentException( "maxEntries < 1" );

            if ( minWorkers < 0 )
                throw new ArgumentException( "minWorkers < 0" );

            if ( maxWorkers < minWorkers )
                throw new ArgumentException( "maxWorkers < minWorkers" );

            if ( maxWorkers < 1 )
                throw new ArgumentException( "maxWorkers < 1" );

            if ( workerLinger < 1 )
                throw new ArgumentException( "workerLinger < 1" );

            this.maxEntries = maxEntries;
            this.entryDelay = entryDelay;
            this.minWorkers = minWorkers;
            this.maxWorkers = maxWorkers;
            this.workerLinger = workerLinger;
            this.threshold = threshold;
        }

        private int maxEntries;
    	
	    private int entryDelay;
    	
	    private int minWorkers;
    	
	    private int maxWorkers;
    	
	    private int workerLinger;
    	
	    private int threshold;

        protected override void Start0()
        {
            // nothing to do
        }

        protected override void Stop0()
        {
            lock (this)
            {
                Monitor.PulseAll(this);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="todo"></param>
        /// Exception:
        ///     throws ThreadInterruptedException
        [ MethodImpl ( MethodImplOptions.Synchronized ) ]
        public void Add( Todo todo )
        {
            CheckIsStarted();

            int n = AddEntry( todo );
            Monitor.Pulse( this );

            ConsiderStartingAWorker( n ) ;

            if ( n > maxEntries )
                Thread.Sleep( entryDelay );
        }

        public void Run()
        {
            bool needAdjust = true;
            try
            {
                Todo todo;
                while ( ( todo = GetNextTodo() ) != null )
                {
                    try
                    {
                        todo.Doit( this );
                    }
                    catch ( Exception e )
                    {
                        todo.Exception( this, e );
                    }
                }
                needAdjust = false;
            }
            finally
            {
                if ( needAdjust )
                    workers.Adjust( -1 );
            }
        }

        # region Workers

        /// <summary>
        /// 
        /// </summary>
        /// <returns>number of workers</returns>
        public int NumWorkers()
        {
            return workers.Get();
        }

        [MethodImpl( MethodImplOptions.Synchronized )]
        private void ConsiderStartingAWorker( int qlen )
        {
            int n = NumWorkers();

            if ( n >= maxWorkers )
                return;

            // Start a new worker if there are none or if the 
            // queue length per worker has exceeded the threshold
            if ( n == 0 || ( (qlen + n-1) / n ) > threshold )
                StartAWorker();
        }

        private void StartAWorker()
        {
            workers.Adjust( 1 );
            Thread t = new Thread(Run);
            t.Start();
        }

        private IntCounter workers = new IntCounter();

        # endregion Workers

        # region Queue

        private Entry head;
        private Entry tail;
        private IntCounter entries = new IntCounter();

        /// <summary>
        /// Adds the todo to the tail of the queue.
        /// </summary>
        /// <param name="todo">the todo to add.</param>
        /// <returns>the current queue length.</returns>
        /// 
        [MethodImpl( MethodImplOptions.Synchronized )]
        private int AddEntry( Todo todo )
        {
            Entry e = new Entry();

            e.todo = todo;

            if ( tail != null )
                tail.next = e;
            else
                head = e;   // first instance

            tail = e;

            return entries.Adjust( 1 );
        }

        /// <summary>
        /// Remove an entry from the queue.
        /// </summary>
        /// <returns>a todo from the head of the queue, or 
        /// null if empty</returns>
        /// 
        [MethodImpl( MethodImplOptions.Synchronized )]
        private Todo RemoveEntry()
        {
            if ( head == null )
                return null;

            Entry e = head;
            head = e.next;

            if ( head == null )
                tail = null;

            entries.Adjust( -1 );

            return e.todo;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>number of TODOs</returns>
        public int NumEntries()
        {
            return entries.Get();
        }

        /// <summary>
        /// An entry in the todo queue
        /// </summary>
        public class Entry
        {
            /// <summary>
            /// The todo to be performed
            /// </summary>
            public Todo todo;

            /// <summary>
            /// The next todo in the queue
            /// </summary>
            public Entry next;
        }

        # endregion Queue

        # region BLAH

        [MethodImpl( MethodImplOptions.Synchronized )]
        private Todo GetNextTodo()
        {
            Todo todo = null;
            bool lingered = false;

            while ( IsStarted() && ( todo = RemoveEntry() ) == null )
            {
                try
                {
                    if ( lingered && workers.Get() > minWorkers )
                    {
                        workers.Adjust( -1 );
                        return null;
                    }

                    Monitor.Wait( this, workerLinger );

                    // we lingered. we might have been woken because
                    // we're stopping, or a todo might have been
                    // queued.

                    lingered = true;
                }
                catch ( ThreadInterruptedException )
                {
                    workers.Adjust( -1 );
                    return null;
                }
            }
            return todo;
        }

        # endregion BLAH

        # region Static Stuff

        /// <summary>
        /// 
        /// </summary>
        /// <param name="todo"></param>
        /// Exception: 
        ///     throws Exception
        ///     
        public static void AddTodo( Todo todo )
        {
            try
            {
                GetTodoManager().Add(todo);
            }
            catch(Exception e)
            {
                todo.Exception(null,e);
            }
            
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the configured TodoManager. If there isn't one, it makes
        /// one with one worker thread.</returns>
        /// Exception: 
        ///     if there is a problem creating the TodoManager
        public static TodoManager GetTodoManager()
        {
            if ( todomanager == null )
            {
                lock ( lockObject  )
                {
                    if ( todomanager == null )
                    {
                        todomanager = new TodoManager( 50, 10, 0, 5, 5000, 0 );
                        todomanager.Start();
                    }
                }
            }
            return todomanager;
        }


        /// <summary>
        /// 
        /// </summary>
        /// <param name="newTodoManager"></param>
        /// <returns>the old todo manager</returns>
        /// 
        [MethodImpl( MethodImplOptions.Synchronized )]
        public static TodoManager SetTodoManager( TodoManager newTodoManager )
        {
            TodoManager oldTodoManager = todomanager;
            todomanager = newTodoManager;
            return oldTodoManager;
        }

        /// <summary>
        /// Shuts down the currently configured static todo manager if any.
        /// </summary>
        /// Exception:
        ///     throws Exception
        public static void ShutDown()
        {
            TodoManager oldTodoManager = SetTodoManager( null );
            if ( oldTodoManager != null )
                oldTodoManager.Stop();
        }

        /// <summary>
        /// Shuts down the given todo manager if any.
        /// </summary>
        /// Exception:
        ///     throws Exception
        public static void ShutDown(TodoManager toDoManager)
        {
            if ( toDoManager != null )
                toDoManager.Stop();
        }

        private static TodoManager todomanager;

        /// <summary>
        /// Since C# doesn't allow locking on an entire class, the substitute 
        /// here is locking a static variable of the class.
        /// </summary>
        private static object lockObject = new Object() ;

        # endregion Static Stuff
    }
}

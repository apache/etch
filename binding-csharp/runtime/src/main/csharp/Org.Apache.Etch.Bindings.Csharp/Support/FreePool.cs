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
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Threading;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    /// <summary>
    /// A free implementation of pool
    /// </summary>
    public class FreePool : Pool
    {
        /// <summary>
        /// Constructs a FreePool with specified maxSize.
        /// </summary>
        /// <param name="maxSize">maximum number of free threads at one time
        /// after which reject requests. </param>
        public FreePool( int maxSize )
        {
            this.maxSize = maxSize;
        }

        /// <summary>
        /// Constructs a FreePool with maxSize 50
        /// </summary>
        public FreePool()
            : this( 50 )
        { }

        private readonly int maxSize;

        private readonly Dictionary<Thread, Thread> group = new Dictionary<Thread, Thread>();

        private Boolean open = true;

        /// <summary>
        /// Closes the pool. This just marks the pool as being closed, it doesn't
        /// actually do anything to the currently running thread. But no more
        /// threads are allowed to start.
        /// </summary>
        public void Close()
        {
            open = false;
        }

        /// <summary>
        /// Joins each of the threads in this pool until there
        /// are none left. The pool will be closed first.
        /// Exception:
        ///     throws ThreadInterruptedException
        /// </summary>
        public void Join()
        {
            Close();
            while (true)
            {
                Thread x;
                lock (group)
                {
                    Dictionary<Thread, Thread>.Enumerator e = group.GetEnumerator();
                    if (!e.MoveNext())
                        break;
                    
                    x = e.Current.Key;
                    group.Remove(x);
                }
                x.Join();
            }
        }

        /// <summary>
        /// Finds the number of active threads in this pool
        /// </summary>
        /// <returns>the number of active threads</returns>
        public int ActiveCount()
        {
            return group.Count;
        }

        #region Pool Members

        [MethodImpl( MethodImplOptions.Synchronized )]
        public void Run( RunDelegate d1, ExceptionDelegate d2 )
        {
            if (!open || ActiveCount() >= maxSize)
                throw new Exception("free pool thread count exceeded or pool closed");

            Thread t = new Thread(
                delegate()
                {
                    try
                    {
                        d1();
                    }
                    catch (Exception e)
                    {
                        d2(e);
                    }
                    finally
                    {
                        lock (group)
                        {
                            group.Remove(Thread.CurrentThread);
                        }
                    }
                });

            lock (group)
            {
                group.Add(t, t);
            }

            t.Start();
        }

        #endregion
    }
}

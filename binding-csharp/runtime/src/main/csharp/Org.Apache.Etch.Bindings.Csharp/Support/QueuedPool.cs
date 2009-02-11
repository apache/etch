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
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    /// <summary>
    /// A queued implementation of pool
    /// </summary>
    public class QueuedPool : Pool
    {
        /// <summary>
        /// Constructs the queued pool with a specified todo manager.
        /// </summary>
        /// <param name="mgr"></param>
        public QueuedPool( TodoManager mgr )
        {
            this.mgr = mgr;
        }

        /// <summary>
        /// Constructs the queued pool with a default todo manager.
        /// </summary>
        /// Exception: 
        ///     throws Exception
        ///     
        public QueuedPool() : this( TodoManager.GetTodoManager() )
        {}

        private TodoManager mgr;

        public void Run( RunDelegate d1, ExceptionDelegate d2 )
        {
            MyTodo inst = new MyTodo ( d1, d2 );
            mgr.Add( inst );
        }

        /// <summary>
        /// Implementation of Todo
        /// </summary>
        public class MyTodo : Todo
        {
            public MyTodo ( RunDelegate d1, ExceptionDelegate d2 )
            {
                this.d1 = d1;
                this.d2 = d2;
            }

            public void Doit( TodoManager mgr )
            {
                d1();
            }

            public void Exception( TodoManager mgr, Exception e ) 
            {
                d2( e );
            }

            private RunDelegate d1;
            private ExceptionDelegate d2;
        }
    }
}

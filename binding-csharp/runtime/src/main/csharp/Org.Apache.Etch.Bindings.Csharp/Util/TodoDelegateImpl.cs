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

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// This class defines delegate for the methods
    /// defined in Todo Interface. 
    /// </summary>
    public class TodoDelegateImpl : Todo
    {
        private DoitDelegate doIt = null;
        private ExceptionToDoDelegate exc = null;



        public TodoDelegateImpl(DoitDelegate d, ExceptionToDoDelegate e)
        {
            this.doIt = d;
            this.exc = e;
        }

        public virtual void Doit(TodoManager mgr)
        {
            if (doIt == null)
                throw new Exception("Doit method not defined");

            doIt(mgr);
        }

        public virtual void Exception(TodoManager mgr, Exception e)
        {
            if (exc == null)
                throw new Exception("Exception method not defined");

            exc(mgr, e);

        }


        /// <summary>
        /// Performs the action
        /// </summary>
        /// <param name="mgr">the todo manager where this todo was queued</param>

        public delegate void DoitDelegate(TodoManager mgr);

        /// <summary>
        /// Reports an exception that occurred while running the todo.
        /// </summary>
        /// <param name="mgr">the todo manager where this todo was queued.</param>
        /// <param name="e">the exception that the todo threw.</param>
        /// 
        public delegate void ExceptionToDoDelegate(TodoManager mgr, Exception e);

    }
}

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
using Org.Apache.Etch.Bindings.Csharp.Msg;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    public class StubHelper
    {
        public StubHelper(Delegate del, AsyncMode mode)
        {
            this.mode = mode;
            this.del = del;
        }

        /// <summary>Constructs a StubHelper which uses synchronous mode to dispatch
        /// method from message.</summary>
        public StubHelper() : this(AsyncMode.NONE)
        {
        }


        ///<summary>Constructs a StubHelper which uses specified mode to
        ///dispatch method from message.</summary>
        ///<param name="mode">param mode</param>
        public StubHelper(AsyncMode mode)
        {
            this.mode = mode;
        }

        private AsyncMode mode;
        private Delegate del;

        ///<returns>true if the mode is AsyncMode.QUEUED.</returns>
        public bool IsQueued()
        {
            return mode == AsyncMode.QUEUED;
        }

        ///<returns>true if the mode is AsyncMode.FREE.</returns>
        public bool IsFree()
        {
            return mode == AsyncMode.FREE;
        }

        public Delegate GetDelegate()
        {
            return del;
        }
    }
}

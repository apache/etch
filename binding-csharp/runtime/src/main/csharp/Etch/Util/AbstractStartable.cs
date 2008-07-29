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
using System.Runtime.CompilerServices;

namespace Etch.Util
{
    /// <summary>
    /// Generic implementation of Startable
    /// </summary>
    abstract public class AbstractStartable : Startable
    {
        [MethodImpl(MethodImplOptions.Synchronized)]
        public void Start()
        {
            if (IsStarted())
                throw new Exception("Is already started");

            try
            {
                started = true;
                Start0();
            }
            catch (Exception e)
            {
                SetStopped();
                Stop0();
                throw e;
            }
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        public void Stop()
        {
            CheckIsStarted();

            started = false;

            Stop0();
        }

        private bool started;

        private void SetStarted()
        {
            started = true;
        }

        protected void SetStopped()
        {
            started = false;
        }

        public bool IsStarted()
        {
            return started;
        }

        /// <summary>
        /// Check that the startable is started
        /// Exception:
        ///     ThreadInterruptedException if not started
        /// </summary>
        /// 
        public void CheckIsStarted()
        {
            if (!IsStarted())
                throw new System.Threading.ThreadInterruptedException("is not started");
        }

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        abstract protected void Start0();

        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        abstract protected void Stop0();

    }
}

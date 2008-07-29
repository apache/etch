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

namespace Etch.Util
{
    /// <summary>
    /// Description of Startable
    /// </summary>
    public interface Startable
    {
        /// <summary>
        /// 
        /// </summary>
        /// Exception:
        ///     throws Exception
        void Start();

        /// Exception:
        ///     throws Exception
        void Stop();

        /// <summary>
        /// 
        /// </summary>
        /// <returns>true if started</returns>
        bool IsStarted();

        /// <summary>
        /// Check the startable is started
        /// </summary>
        void CheckIsStarted();
    
    }
}

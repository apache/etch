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

namespace Etch.Util
{
    /// <summary>
    /// RunnerHandler receives notification of runner events.
    /// </summary>
    public interface RunnerHandler
    {
        /// <summary>
        /// Reports that the thread controlling the runner has started.
        /// </summary>
        void Started( );

        /// <summary>
        /// Reports that the thread controlling the runner has stopped.
        /// </summary>
        void Stopped();

        /// <summary>
        /// Reports that the thread controlling the runner has caught an
        /// exception.
        /// </summary>
        /// <param name="what">A short description of the origin</param>
        /// <param name="e">Exception that was caught</param>
        void Exception( String what, Exception e );

    }
}

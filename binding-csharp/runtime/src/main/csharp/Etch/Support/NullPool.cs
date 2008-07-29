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
using System.Threading;

namespace Etch.Support
{
    /// <summary>
    /// A pool which runs runnable directly
    /// </summary>
    public class NullPool : Pool
    {
        #region Pool Members

        public void Run( RunDelegate d1, ExceptionDelegate d2 )
        {
            new Thread(
                new ThreadStart( delegate()
                                {
                                    try
                                    {
                                        d1();
                                    }
                                    catch ( Exception e )
                                    {
                                        d2( e );
                                    }
                                } ) ).Start();
        }

        #endregion
    }
}

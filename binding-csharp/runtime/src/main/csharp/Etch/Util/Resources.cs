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
using System.Collections.Generic;

namespace Etch.Util
{
    public class Resources : Dictionary<String, Object>
    {
        public Resources()
        {
            // ignore
        }

        public Resources( Resources r )
            : base( r )
        { }

        public Object Get( String key )
        {
            try
            {
                return this[ key ];
            }
            catch ( KeyNotFoundException )
            {
                return null;
            }
        }
    }
}

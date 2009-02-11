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
    /// Class to help construct transport stacks.
    /// </summary>
    public class TransportHelper
    {
        #region RESOURCES

        /// <summary>
        /// The Pool to use to execute queued async receiver messages. */
	    /// </summary>
        public const String QUEUED_POOL = "QUEUED_POOL";
    	
	    /// <summary>
	    /// The Pool to use to execute free async receiver messages.
	    /// </summary>
 	    public const String FREE_POOL = "FREE_POOL";
    	
	    /// <summary>
	    /// Binary transport format
	    /// </summary>
	    public const String BINARY = "binary";
    	
	    /// <summary>
        /// Xml transport format
	    /// </summary>
	    public const String XML = "xml";

        #endregion

        #region UTILITIES

        /// <summary>
        /// Initializes standard resources.
        /// </summary>
        /// <param name="resources">some initial values for resources. May be null to accept
        /// all the defaults.</param>
        /// <returns>copy of resources initialized with default values for standard items</returns>
        public static Resources InitResources( Resources resources )
        {
            if ( resources == null )
                resources = new Resources();
            else
                resources = new Resources(resources);

            if ( !resources.ContainsKey( QUEUED_POOL ) )
                resources.Add( QUEUED_POOL, new QueuedPool() );

            if ( !resources.ContainsKey( FREE_POOL ) )
                resources.Add( FREE_POOL, new FreePool() );

            return resources;
        }

        #endregion
    }
}

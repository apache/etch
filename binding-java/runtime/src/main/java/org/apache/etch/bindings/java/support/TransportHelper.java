/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.bindings.java.support;

import org.apache.etch.util.Resources;

/**
 * Class to help construct transport stacks.
 */
public class TransportHelper
{
	///////////////
	// RESOURCES //
	///////////////

	/** The Pool to use to execute queued async receiver messages. */
	public static final String QUEUED_POOL = "QUEUED_POOL";
	
	/** The Pool to use to execute free async receiver messages. */
	public static final String FREE_POOL = "FREE_POOL";
	
	/** Binary transport format */
	public final static String BINARY = "binary";
	
	/** Xml transport format */
	public final static String XML = "xml";

	///////////////
	// UTILITIES //
	///////////////
	
	/**
	 * Initializes standard resources.
	 * 
	 * @param resources some initial values for resources. May be null to accept
	 * all the defaults.
	 * @return copy of resources initialized with default values for standard
	 * items.
	 * @throws Exception
	 */
	public static Resources initResources( Resources resources ) throws Exception
	{
		if (resources == null)
			resources = new Resources();
		else
			resources = new Resources( resources );
		
		if (!resources.containsKey( QUEUED_POOL ))
			resources.put( QUEUED_POOL, new QueuedPool() );
		
		if (!resources.containsKey( FREE_POOL ))
			resources.put( FREE_POOL, new FreePool() );
		
		return resources;
	}
}


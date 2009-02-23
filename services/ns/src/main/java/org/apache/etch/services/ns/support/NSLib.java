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

package org.apache.etch.services.ns.support;

import java.util.Map;

import org.apache.etch.services.ns.NameService.Entry;
import org.apache.etch.util.core.io.Session;


/**
 * 
 * Class that acts as an interface between core etch and the naming service.
 *
 * NOTE: One NSLib instance deals with only one connection to the name service
 * 
 */
public abstract class NSLib
{
	public final static int RECONNECT_DELAY = 4000;
	private final static Object instanceSync = new Object();
	
	public NSLib(){}
	public static NSLib instance;
	
	/**
	 * Get an instance of NSLib
	 * 
	 * @return a new instance of NSLib
	 */
	public static NSLib getInstance()
	{
		
		/*if ( instance == null )
		{
			synchronized ( instanceSync )
			{
				if ( instance == null )
					instance = instance.getImplInstance();		
			}
		}*/
		return instance;
	}

	/**
	 * Get the default NSUri for this NSLib
	 * @return the default NSUri
	 */
	public static String staticGetDefaultNsUri()
	{
		return getInstance().getDefaultNsUri();
	}
	
	/**
	 * Sets the default NSUri for this NSLib
	 * @param defaultNsUri the new default NsUri
	 */
	public static void staticSetDefaultNsUri( String defaultNsUri )
	{
		getInstance().setDefaultNsUri( defaultNsUri );
	}
	
	/**
	 * 
	 * @param newInstance
	 * @return old instance 
	 */
	public static NSLib setInstance( NSLib newInstance )
	{
		synchronized ( instanceSync ) 
		{
			NSLib oldInstance = instance;
			instance = newInstance;
			return oldInstance;
		}
	}

	public static void staticShutdown()
	{
		NSLib oldInstance = setInstance( null );
		if ( oldInstance != null )
			oldInstance.shutdown();
	}
	
	/**
	 * Obtain instance of the specific NSLib implementation
	 * @return
	 */
	public abstract NSLib getImplInstance();
	
	/**
	 * Cleanup method for the NSLib implementation
	 */
	public abstract void shutdown();

	/**
	 * Get the default naming service uri in case none specific in the uris
	 * @return default naming service uri
	 */
	public abstract String getDefaultNsUri();
	
	/**
	 * Set the default naming service uri
	 * @param defaultNsUri default naming service uri
	 */
	public abstract void setDefaultNsUri( String defaultNsUri );
	
	/**
	 * Adapter for the Name Service register, please refer to ns.etch for additional 
	 * documentation
	 * 
	 * @param session session listener session object to report errors
	 * to the name service 
	 * @param nsUri name service uri. If null, defaultNsUri is used.
	 */
	public static void staticRegister( Session session, String uri, Map<?,?> qualities, int ttl )
	{
		NSLib.getInstance().register( session, uri, qualities, ttl );
	}

	/**
	 * Unregister a listener from the name service
	 * 
	 * @param session listener session object to report errors
	 * @param uri uri provided by listener 
	 */
	public static void staticUnregister( Session session, String uri, boolean deleteEntry )
	{
		NSLib.getInstance().unregister( session, uri, deleteEntry );
	}
	
	/**
	 * Look up the target uri of the supplied source from the name service
	 * 
	 * @param nsUri name service uri. If null, defaultNsUri is used.
	 * @param sourceUri sourceUri of the target service
	 * @return an entry containing all the details of the target service 
	 */
	public static Entry staticLookup( String uri )
	{
		return NSLib.getInstance().lookup( uri );
	}
	
	public abstract void register( Session session, String uri, Map<?,?> qualities, int ttl );
	
	public abstract void unregister( Session session, String uri, boolean deleteEntry );

	public abstract Entry lookup( String uri );

}
